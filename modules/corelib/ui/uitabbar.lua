-- @docclass
UITabBar = extends(UIWidget)

-- private functions
local function onTabClick(tab)
  tab.tabBar:selectTab(tab)
end

local function updateMargins(tabBar, ignored)
  if #tabBar.tabs == 0 then return end
 
  local currentMargin = 0
  for i = 1, #tabBar.tabs do
    if tabBar.tabs[i] ~= ignored then 
      if i == 1 then
        tabBar.tabs[i]:setMarginLeft(0)
      else
        tabBar.tabs[i]:setMarginLeft(tabBar.tabSpacing * (i - 1) + currentMargin)
      end
    end
    currentMargin = currentMargin + tabBar.tabs[i]:getWidth()
  end
end

local function onTabMousePress(tab, mousePos, mouseButton)
  if mouseButton == MouseRightButton then
    if tab.menuCallback then tab.menuCallback(tab, mousePos, mouseButton) end
    return true
  end
end

local function onTabDragEnter(tab)
  tab:raise()
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
    local newMargin = tab:getMarginLeft() + mouseMoved.x
    if newMargin >= -tab.tabBar.tabSpacing and newMargin < tab.tabBar:getWidth() - tab:getWidth() then
      tab:setMarginLeft(newMargin)
    end

    local tabs = tab.tabBar.tabs
    local lastMargin = -tab.tabBar.tabSpacing
    for i = 1, #tabs do
      local nextMargin = tabs[i + 1] and (tabs[i + 1] == tab and (tabs[i]:getMarginLeft() + tabs[i]:getWidth() + tab.tabBar.tabSpacing) or tabs[i + 1]:getMarginLeft()) or tab.tabBar:getWidth()
      if (tab:getMarginLeft()+(tabs[i]:getWidth()/2)) >= lastMargin and (tab:getMarginLeft()+(tabs[i]:getWidth()/2)) < nextMargin then
        if tabs[i] ~= tab then
          local newIndex = table.find(tab.tabBar.tabs, tab.tabBar.tabs[i])
          table.remove(tab.tabBar.tabs, table.find(tab.tabBar.tabs, tab))
          table.insert(tab.tabBar.tabs, newIndex, tab)
          updateMargins(tab.tabBar, tab)
          break
        else
          updateMargins(tab.tabBar, tab)
          break
        end
      end
      lastMargin = tab.tabBar.tabs[i]:getMarginLeft() == 0 and -tab.tabBar.tabSpacing or tab.tabBar.tabs[i]:getMarginLeft()
    end
  end
end

local function tabBlink(tab)
  if not tab.blinking then return end
  tab:setOn(not tab:isOn())
  tab.blinkEvent = scheduleEvent(function() tabBlink(tab) end, 500)
end

-- public functions
function UITabBar.create()
  local tabbar = UITabBar.internalCreate()
  tabbar:setFocusable(false)
  tabbar.tabs = {}
  tabbar.selected = nil  -- dragged tab
  tabbar.tabSpacing = 5
  tabbar.tabsMoveable = false
  return tabbar
end

function UITabBar:setContentWidget(widget)
  self.contentWidget = widget
  if #self.tabs > 0 then
    self.contentWidget:addChild(self.tabs[1].tabPanel)
  end
end

function UITabBar:setTabSpacing(tabSpacing)
  self.tabSpacing = tabSpacing
  updateMargins(self)
end

function UITabBar:addTab(text, panel, menuCallback)
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
function UITabBar:moveTab(tab, units)
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

function UITabBar:onStyleApply(styleName, styleNode)
  if styleNode['moveable'] then
    self.tabsMoveable = styleNode['moveable']
  end
end

function UITabBar:removeTab(tab)
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

function UITabBar:getTab(text)
  for k,tab in pairs(self.tabs) do
    if tab:getText():lower() == text:lower() then
      return tab
    end
  end
end

function UITabBar:selectTab(tab)
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

function UITabBar:selectNextTab()
  if self.currentTab == nil then return end
  local index = table.find(self.tabs, self.currentTab)
  if index == nil then return end
  local nextTab = self.tabs[index + 1] or self.tabs[1]
  if not nextTab then return end
  self:selectTab(nextTab)
end

function UITabBar:selectPrevTab()
  if self.currentTab == nil then return end
  local index = table.find(self.tabs, self.currentTab)
  if index == nil then return end
  local prevTab = self.tabs[index - 1] or self.tabs[#self.tabs]
  if not prevTab then return end
  self:selectTab(prevTab)
end

function UITabBar:blinkTab(tab)
  if tab:isChecked() or tab.blinking then return end
  tab.blinking = true
  tabBlink(tab)
end

function UITabBar:getTabPanel(tab)
  return tab.tabPanel
end

function UITabBar:getCurrentTabPanel()
  if self.currentTab then
    return self.currentTab.tabPanel
  end
end

function UITabBar:getCurrentTab()
  return self.currentTab
end
