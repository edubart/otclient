-- @docclass
UIMoveableTabBar = extends(UIWidget)

-- private functions
local function onTabClick(tab)
  tab.tabBar:selectTab(tab)
end

local function updateMargins(tabBar, ignored)
  if #tabBar.tabs == 0 then return end
 
  local currentMargin = 0
  for i = 1, #tabBar.tabs do
    tabBar.tabs[i]:setMarginLeft(currentMargin)
    currentMargin = currentMargin + tabBar.tabSpacing + tabBar.tabs[i]:getWidth()
  end
end

local function updateIndexes(tabBar, tab, xoff)
  local tabs = tabBar.tabs
  local currentMargin = 0
  local prevIndex = table.find(tabs, tab)
  local newIndex = prevIndex
  local xmid = xoff + tab:getWidth()/2
  for i = 1, #tabs do
    local nextTab = tabs[i]
    if xmid >= currentMargin + nextTab:getWidth()/2 then
      newIndex = table.find(tabs, nextTab)
    end
    currentMargin = currentMargin + tabBar.tabSpacing * (i - 1) + tabBar.tabs[i]:getWidth()
  end
  if newIndex ~= prevIndex then
    table.remove(tabs, table.find(tabs, tab))
    table.insert(tabs, newIndex, tab)
  end
end

local function getMaxMargin(tabBar, tab)
  if #tabBar.tabs == 0 then return end

  local maxMargin = 0
  for i = 1, #tabBar.tabs do
    if tabBar.tabs[i] ~= tab then
      maxMargin = maxMargin + tabBar.tabs[i]:getWidth()
    end
  end
  return maxMargin + tabBar.tabSpacing * (#tabBar.tabs- 1)
end

local function onTabMousePress(tab, mousePos, mouseButton)
  if mouseButton == MouseRightButton then
    if tab.menuCallback then tab.menuCallback(tab, mousePos, mouseButton) end
    return true
  end
end

local function onTabDragEnter(tab, mousePos)
  tab:raise()
  tab.hotSpot = mousePos.x - tab:getMarginLeft()
  tab.tabBar.selected = tab
  return true
end

local function onTabDragLeave(tab)
  updateMargins(tab.tabBar)
  tab.tabBar.selected = nil
  return true
end

local function onTabDragMove(tab, mousePos, mouseMoved)
  if tab == tab.tabBar.selected then
    local xoff = mousePos.x - tab.hotSpot

    -- update indexes
    updateIndexes(tab.tabBar, tab, xoff)
    updateIndexes(tab.tabBar, tab, xoff)

    -- update margins
    updateMargins(tab.tabBar)
    xoff = math.max(xoff, 0)
    xoff = math.min(xoff, getMaxMargin(tab.tabBar, tab))
    tab:setMarginLeft(xoff)
  end
end

local function tabBlink(tab, step)
  step = step or 0
  tab:setOn(not tab:isOn())

  removeEvent(tab.blinkEvent)
  if step < 4 then
    tab.blinkEvent = scheduleEvent(function() tabBlink(tab, step+1) end, 500)
  else
    tab:setOn(true)
    tab.blinkEvent = nil
  end
end

-- public functions
function UIMoveableTabBar.create()
  local tabbar = UIMoveableTabBar.internalCreate()
  tabbar:setFocusable(false)
  tabbar.tabs = {}
  tabbar.selected = nil  -- dragged tab
  tabbar.tabSpacing = 0
  tabbar.tabsMoveable = false
  return tabbar
end

function UIMoveableTabBar:setContentWidget(widget)
  self.contentWidget = widget
  if #self.tabs > 0 then
    self.contentWidget:addChild(self.tabs[1].tabPanel)
  end
end

function UIMoveableTabBar:setTabSpacing(tabSpacing)
  self.tabSpacing = tabSpacing
  updateMargins(self)
end

function UIMoveableTabBar:addTab(text, panel, menuCallback)
  if panel == nil then
    panel = g_ui.createWidget(self:getStyleName() .. 'Panel')
    panel:setId('tabPanel')
  end

  local tab = g_ui.createWidget(self:getStyleName() .. 'Button', self)
  panel.isTab = true
  tab.tabPanel = panel
  tab.tabBar = self
  tab:setId('tab')
  tab:setDraggable(self.tabsMoveable)
  tab:setText(text)
  tab:setWidth(tab:getTextSize().width + tab:getPaddingLeft() + tab:getPaddingRight())
  tab.menuCallback = menuCallback or nil
  tab.onClick = onTabClick
  tab.onMousePress = onTabMousePress
  tab.onDragEnter = onTabDragEnter
  tab.onDragLeave = onTabDragLeave
  tab.onDragMove = onTabDragMove
  tab.onDestroy = function() tab.tabPanel:destroy() end

  table.insert(self.tabs, tab)
  if #self.tabs == 1 then
    self:selectTab(tab)
    tab:setMarginLeft(0)
  else
    local newMargin = self.tabSpacing * (#self.tabs - 1)
    for i = 1, #self.tabs - 1 do
      newMargin = newMargin + self.tabs[i]:getWidth()
    end
    tab:setMarginLeft(newMargin)
  end

  return tab
end

-- Additional function to move the tab by lua
function UIMoveableTabBar:moveTab(tab, units)
  local index = table.find(self.tabs, tab)
  if index == nil then return end
  
  local focus = false
  if self.currentTab == tab then
    self:selectPrevTab()
    focus = true
  end
  
  table.remove(self.tabs, index)
  
  local newIndex = math.min(#self.tabs+1, math.max(index + units, 1))
  table.insert(self.tabs, newIndex, tab) 
  if focus then self:selectTab(tab) end
  updateMargins(self)
  return newIndex
end

function UIMoveableTabBar:onStyleApply(styleName, styleNode)
  if styleNode['moveable'] then
    self.tabsMoveable = styleNode['moveable']
  end
end

function UIMoveableTabBar:removeTab(tab)
  local index = table.find(self.tabs, tab)
  if index == nil then return end
  if self.currentTab == tab then
    self:selectPrevTab()
  end
  table.remove(self.tabs, index)
  if tab.blinkEvent then
    removeEvent(tab.blinkEvent)
  end
  tab:destroy()
  updateMargins(self)
end

function UIMoveableTabBar:getTab(text)
  for k,tab in pairs(self.tabs) do
    if tab:getText():lower() == text:lower() then
      return tab
    end
  end
end

function UIMoveableTabBar:selectTab(tab)
  if self.currentTab == tab then return end
  if self.contentWidget then
    local selectedWidget = self.contentWidget:getLastChild()
    if selectedWidget and selectedWidget.isTab then
      self.contentWidget:removeChild(selectedWidget)
    end
    self.contentWidget:addChild(tab.tabPanel)
    tab.tabPanel:fill('parent')
  end

  if self.currentTab then
    self.currentTab:setChecked(false)
  end
  signalcall(self.onTabChange, self, tab)
  self.currentTab = tab
  tab:setChecked(true)
  tab:setOn(false)
  tab.blinking = false

  local parent = tab:getParent()
  parent:focusChild(tab, MouseFocusReason)
end

function UIMoveableTabBar:selectNextTab()
  if self.currentTab == nil then return end
  local index = table.find(self.tabs, self.currentTab)
  if index == nil then return end
  local nextTab = self.tabs[index + 1] or self.tabs[1]
  if not nextTab then return end
  self:selectTab(nextTab)
end

function UIMoveableTabBar:selectPrevTab()
  if self.currentTab == nil then return end
  local index = table.find(self.tabs, self.currentTab)
  if index == nil then return end
  local prevTab = self.tabs[index - 1] or self.tabs[#self.tabs]
  if not prevTab then return end
  self:selectTab(prevTab)
end

function UIMoveableTabBar:blinkTab(tab)
  if tab:isChecked() then return end
  tab.blinking = true
  tabBlink(tab)
end

function UIMoveableTabBar:getTabPanel(tab)
  return tab.tabPanel
end

function UIMoveableTabBar:getCurrentTabPanel()
  if self.currentTab then
    return self.currentTab.tabPanel
  end
end

function UIMoveableTabBar:getCurrentTab()
  return self.currentTab
end
