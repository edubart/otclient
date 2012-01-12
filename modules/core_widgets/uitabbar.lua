UITabBar = extends(UIWidget)

-- private functions
local function onTabClick(tabButton)
  tabButton.tabBar:selectTab(tabButton)
end

local function tabBlink(tabButton)
  if tabButton.blinking then
    tabButton:setOn(not tabButton:isOn())
    scheduleEvent(function() tabBlink(tabButton) end, 300)
  end
end

-- public functions
function UITabBar.create()
  local tabbar = UITabBar.internalCreate()
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

  local tabButton = createWidget(self:getStyleName() .. 'Button', self)
  tabButton.tabPanel = panel
  tabButton.tabBar = self
  tabButton:setText(text)
  tabButton:setWidth(tabButton:getTextSize().width + tabButton:getPaddingLeft() + tabButton:getPaddingRight())
  connect(tabButton, { onClick = onTabClick })

  table.insert(self.tabs, tabButton)
  if #self.tabs == 1 then
    self:selectTab(tabButton)
  end

  return tabButton
end

function UITabBar:selectTab(tabButton)
  if self.contentWidget then
    local selectedWidget = self.contentWidget:getFirstChild()
    if selectedWidget then
      self.contentWidget:removeChild(selectedWidget)
    end
    self.contentWidget:addChild(tabButton.tabPanel)
    tabButton.tabPanel:fill('parent')
  end

  tabButton:setChecked(true)
  tabButton:setOn(false)
  tabButton.blinking = false
  if self.currentTabButton then
    self.currentTabButton:setChecked(false)
  end
  self.currentTabButton = tabButton
end

function UITabBar:blinkTab(tabButton)
  if not tabButton:isChecked() then
    tabButton:setOn(true)
    tabButton.blinking = true
    tabBlink(tabButton)
  end
end

function UITabBar:getTabPanel(tabButton)
  return tabButton.tabPanel
end

function UITabBar:getCurrentTabPanel()
  if self.currentTabButton then
    return self.currentTabButton.tabPanel
  end
end
