UIMiniWindowContainer = extends(UIWidget)

function UIMiniWindowContainer.create()
  local container = UIMiniWindowContainer.internalCreate()
  container:setFocusable(false)
  container:setPhantom(true)
  return container
end

function UIMiniWindowContainer:onDrop(widget, mousePos)
  if widget:getClassName() == 'UIMiniWindow' then
    widget:setParent(self)
    return true
  end
end

function UIMiniWindowContainer:getClassName()
  return 'UIMiniWindowContainer'
end
