UIMiniWindowContainer = extends(UIWidget)

function UIMiniWindowContainer.create()
  local container = UIMiniWindowContainer.internalCreate()
  container:setFocusable(false)
  container:setPhantom(true)
  return container
end

function UIMiniWindowContainer:onDrop(widget, mousePos)
  if widget:getClassName() == 'UIMiniWindow' then
  	local oldParent = widget:getParent()
  	if oldParent == self then
  		return true
  	end

  	oldParent:removeChild(widget)

  	if widget.movedWidget then
  		local index = self:getChildIndex(widget.movedWidget)
  		self:insertChild(index + widget.movedIndex, widget)
  	else
  		self:addChild(widget)
  	end
    
    return true
  end
end

function UIMiniWindowContainer:getClassName()
  return 'UIMiniWindowContainer'
end
