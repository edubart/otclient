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

function UIPopupMenu.display(otui, pos)
  local menu = UI.display(otui, {x = pos.x, y = pos.y})
  return menu
end

-- hooked events
local function onWidgetStyleApply(widget, style)
  if style and style.popupmenu then
    widget.popupmenu = style.popupmenu
  end
end

local function onWidgetMousePress(widget, mousePos, mouseButton)
  if widget.popupmenu and mouseButton == MouseRightButton then
    UIPopupMenu.display(widget.popupmenu, mousePos)
    return true
  end
  return false
end

connect(UIWidget, { onStyleApply = onWidgetStyleApply,
                    onMousePress = onWidgetMousePress })