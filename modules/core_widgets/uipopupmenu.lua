UIPopupMenu = extends(UIWidget)

local displayedMenuList = {}

function UIPopupMenu.create()
  local menu = UIPopupMenu.internalCreate()
  local layout = UIVerticalLayout.create(menu)
  layout:setFitChildren(true)
  menu:setLayout(layout)
  return menu
end

function UIPopupMenu:display(pos)
  -- don't display if not options was added
  if self:getChildCount() == 0 then
    self:destroy()
    return
  end

  displayUI(self, {x = pos.x, y = pos.y})
  self:grabMouse()
  self:grabKeyboard()
  table.insert(displayedMenuList, self)
end

function UIPopupMenu:onGeometryChange()
  self:bindRectToParent()
end

function UIPopupMenu:addOption(optionName, optionCallback)
  local optionWidget = createWidget(self:getStyleName() .. 'Button', self)
  local lastOptionWidget = self:getLastChild()
  optionWidget.onClick = function()
    optionCallback()
    self:destroy()
  end
  optionWidget:setText(optionName)
  local width = optionWidget:getTextSize().width + optionWidget:getMarginLeft() + optionWidget:getMarginRight() + 6
  self:setWidth(math.max(self:getWidth(), width))
end

function UIPopupMenu:addSeparator()
  createWidget(self:getStyleName() .. 'Separator', self)
end

function UIPopupMenu:onDestroy()
  table.removevalue(displayedMenuList, self)
end

function UIPopupMenu:onMousePress(mousePos, mouseButton)
  -- clicks outside menu area destroys the menu
  if not self:containsPoint(mousePos) then
    self:destroy()
    return true
  end
  return false
end

function UIPopupMenu:onKeyPress(keyCode, keyboardModifiers, wouldFilter)
  if wouldFilter then return end
  if keyCode == KeyEscape then
    self:destroy()
    return true
  end
  return false
end

  -- close all menus when the window is resized
local function onRootGeometryUpdate()
  for i,menu in ipairs(displayedMenuList) do
    menu:destroy()
  end
end
connect(rootWidget, { onGeometryChange = onRootGeometryUpdate} )
