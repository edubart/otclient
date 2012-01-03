-- extends UIWidget
UIPopupMenu = extends(UIWidget)

-- public functions
function UIPopupMenu.create()
  local menu = UIPopupMenu.internalCreate()
  local layout = UIVerticalLayout.create(menu)
  layout:setFitParent(true)
  menu:setLayout(layout)
  return menu
end

function UIPopupMenu.display(menu, pos)
  displayUI(menu, {x = pos.x, y = pos.y})
  menu:bindRectToParent()
  menu:grabMouse()
  menu:grabKeyboard()
  return menu
end

function UIPopupMenu.addOption(menu, optionName, optionCallback)
  local optionWidget = createWidget(menu:getStyleName() .. 'Button', menu)
  local lastOptionWidget = menu:getLastChild()
  optionWidget.onClick = function()
    optionCallback()
    menu:destroy()
  end
  optionWidget:setText(optionName)
end

function UIPopupMenu.addSeparator(menu)
  local separatorWidget = createWidget(menu:getStyleName() .. 'Separator', separator)
end

-- hooked events
function UIPopupMenu.onMousePress(menu, mousePos, mouseButton)
  -- clicks outside menu area destroys the menu
  if not menu:containsPoint(mousePos) then
    menu:destroy()
    return true
  end
  return false
end

function UIPopupMenu.onKeyPress(menu, keyCode, keyText, keyboardModifiers)
  if keyCode == KeyEscape then
    menu:destroy()
    return true
  end
  return false
end
