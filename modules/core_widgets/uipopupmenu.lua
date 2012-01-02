-- extends UIWidget
UIPopupMenu = extends(UIWidget)

-- public functions
function UIPopupMenu.create()
  local menu = UIPopupMenu.internalCreate()
  local layout = UIVerticalLayout.create(menu)
  layout:setFitParent(true)
  menu:setLayout(layout)
  menu:setStyle('PopupMenu')
  return menu
end

function UIPopupMenu.display(menu, pos)
  UI.display(menu, {x = pos.x, y = pos.y})
  menu:bindRectToParent()
  menu:grabMouse()
  menu:grabKeyboard()
  return menu
end

function UIPopupMenu.addOption(menu, optionName, optionCallback)
  local optionWidget = UIButton.create()
  local lastOptionWidget = menu:getLastChild()
  optionWidget.onClick = function()
    optionCallback()
    menu:destroy()
  end
  optionWidget:setText(optionName)
  optionWidget:setStyle('PopupMenuButton')
  menu:addChild(optionWidget)
end

function UIPopupMenu.addSeparator(menu)
  local separatorWidget = UIWidget.create()
  separatorWidget:setStyle('PopupMenuSeparator')
  menu:addChild(separator)
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
