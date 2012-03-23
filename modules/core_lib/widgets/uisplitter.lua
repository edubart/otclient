UISplitter = extends(UIWidget)

function UISplitter.create()
  local splitter = UISplitter.internalCreate()
  splitter:setFocusable(false)
  splitter.canGrow = true
  splitter.canShrink = true
  return splitter
end

function UISplitter:onHoverChange(hovered)
  if hovered then
    if self:getWidth() > self:getHeight() then
      Mouse.setVerticalCursor()
      self.vertical = true
    else
      Mouse.setHorizontalCursor()
      self.vertical = false
    end
  elseif not self:isPressed() then
    Mouse.restoreCursor()
  end
end

function UISplitter:getAttachedTo()
  local parent = self:getParent()
  if parent and self.attachedTo then
    return parent:getChildById(self.attachedTo)
  end
end

function UISplitter:onMouseMove(mousePos, mouseMoved)
  if self:isPressed() then
    local deltay = mousePos.y - (self:getPosition().y + self:getHeight()/2)
    local deltax = mousePos.x - (self:getPosition().x + self:getWidth()/2)
    local attachedToWidget = self:getAttachedTo()
    if not attachedToWidget then return end
    if self.vertical then
      if deltay == 0 then return end
      if not self.canGrow and deltay > 0 then return end
      if not self.canShrink and deltay < 0 then return end
      attachedToWidget:setHeight(attachedToWidget:getHeight() - deltay)
    else
      if deltax == 0 then return end
      attachedToWidget:setWidth(attachedToWidget:getWidth() - deltax)
    end
    return true
  end
end

function UISplitter:onMouseRelease(mousePos, mouseButton)
  if not self:isHovered() then
    Mouse.restoreCursor()
  end
  self:ungrabMouse()
end

function UISplitter:onStyleApply(styleName, styleNode)
  if styleNode['attached-to'] then
    self.attachedTo = styleNode['attached-to']
  end
end

function UISplitter:setGrow(enabled)
  self.canGrow = enabled
end

function UISplitter:setShrink(enabled)
  self.canShrink = enabled
end
