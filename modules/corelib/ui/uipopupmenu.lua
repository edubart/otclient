-- @docclass
UIPopupMenu = extends(UIWidget)

local currentMenu

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

  if g_ui.isMouseGrabbed() then
    self:destroy()
    return
  end

  if currentMenu then
    currentMenu:destroy()
  end

  if pos == nil then
    pos = g_window.getMousePosition()
  end

  rootWidget:addChild(self)
  self:setPosition(pos)
  self:grabMouse()
  --self:grabKeyboard()
  currentMenu = self
end

function UIPopupMenu:onGeometryChange(oldRect, newRect)
  local parent = self:getParent()
  if not parent then return end
  local ymax = parent:getY() + parent:getHeight()
  local xmax = parent:getX() + parent:getWidth()
  if newRect.y + newRect.height > ymax then
    local newy = newRect.y - newRect.height
    if newy > 0 and newy + newRect.height < ymax then self:setY(newy) end
  end
  if newRect.x + newRect.width > xmax then
    local newx = newRect.x - newRect.width
    if newx > 0 and newx + newRect.width < xmax then self:setX(newx) end
  end
  self:bindRectToParent()
end

function UIPopupMenu:addOption(optionName, optionCallback, shortcut)
  local optionWidget = g_ui.createWidget(self:getStyleName() .. 'Button', self)
  local lastOptionWidget = self:getLastChild()
  optionWidget.onClick = function(self)
    self:getParent():destroy()
    optionCallback()
  end
  optionWidget:setText(optionName)
  local width = optionWidget:getTextSize().width + optionWidget:getMarginLeft() + optionWidget:getMarginRight() + 15

  if shortcut then
    local shortcutLabel = g_ui.createWidget(self:getStyleName() .. 'ShortcutLabel', optionWidget)
    shortcutLabel:setText(shortcut)
    width = width + shortcutLabel:getTextSize().width + shortcutLabel:getMarginLeft() + shortcutLabel:getMarginRight()
  end

  self:setWidth(math.max(self:getWidth(), width))
end

function UIPopupMenu:addSeparator()
  g_ui.createWidget(self:getStyleName() .. 'Separator', self)
end

function UIPopupMenu:onDestroy()
  if currentMenu == self then
    currentMenu = nil
  end
  self:ungrabMouse()
end

function UIPopupMenu:onMousePress(mousePos, mouseButton)
  -- clicks outside menu area destroys the menu
  if not self:containsPoint(mousePos) then
    self:destroy()
  end
  return true
end

function UIPopupMenu:onKeyPress(keyCode, keyboardModifiers)
  if keyCode == KeyEscape then
    self:destroy()
    return true
  end
  return false
end

-- close all menus when the window is resized
local function onRootGeometryUpdate()
  if currentMenu then
    currentMenu:destroy()
  end
end
connect(rootWidget, { onGeometryChange = onRootGeometryUpdate} )
