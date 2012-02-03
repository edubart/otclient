UITabBar = extends(UIWidget)

-- private functions
local function onTabClick(tab)
  tab.tabBar:selectTab(tab)
end

local function tabBlink(tab)
  if not tab.blinking then return end
  tab:setOn(not tab:isOn())
  scheduleEvent(function() tabBlink(tab) end, 500)
end

-- public functions
function UITabBar.create()
  local tabbar = UITabBar.internalCreate()
  tabbar:setFocusable(false)
  tabbar.tabs = {}
  return tabbar
end

function UITabBar:setContentWidget(widget)
  self.contentWidget = widget
  if #self.tabs > 0 then
    self.contentWidget:addChild(self.tabs[1].tabPanel)
  end
end

function UITabBar:addTab(text, panel)
  if panel == nil then
    panel = createWidget(self:getStyleName() .. 'Panel')
  end

  local tab = createWidget(self:getStyleName() .. 'Button', self)
  tab.tabPanel = panel
  tab.tabBar = self
  tab:setText(text)
  tab:setWidth(tab:getTextSize().width + tab:getPaddingLeft() + tab:getPaddingRight())
  connect(tab, { onClick = onTabClick })

  table.insert(self.tabs, tab)
  if #self.tabs == 1 then
    self:selectTab(tab)
  end

  return tab
end

function UITabBar:removeTab(tab)
  local index = table.find(self.tabs, tab)
  if index == nil then return end
  if self.currentTab == tab then
    self:selectPrevTab()
  end
  table.remove(self.tabs, index)
  tab:destroy()
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
    local selectedWidget = self.contentWidget:getFirstChild()
    if selectedWidget then
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