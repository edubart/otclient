-- @docclass
UIResizeBorder = extends(UIWidget)

function UIResizeBorder.create()
  local resizeborder = UIResizeBorder.internalCreate()
  resizeborder:setFocusable(false)
  resizeborder.minimum = 0
  resizeborder.maximum = 1000
  return resizeborder
end

function UIResizeBorder:onHoverChange(hovered)
  if hovered then
    if g_mouse.isCursorChanged() or g_mouse.isPressed() then return end
    if self:getWidth() > self:getHeight() then
      g_mouse.setVerticalCursor()
      self.vertical = true
    else
      g_mouse.setHorizontalCursor()
      self.vertical = false
    end
    self.hovering = true
    if not self:isPressed() then
      g_effects.fadeIn(self)
    end
  else
    if not self:isPressed() and self.hovering then
      g_mouse.restoreCursor()
      g_effects.fadeOut(self)
      self.hovering = false
    end
  end
end

function UIResizeBorder:onMouseMove(mousePos, mouseMoved)
  if self:isPressed() then
    if self.vertical then
      local delta = mousePos.y - self:getY() - self:getHeight()/2
      local parent = self:getParent()
      local newSize = math.min(math.max(parent:getHeight() + delta, self.minimum), self.maximum)
      parent:setHeight(newSize)
      signalcall(parent.onHeightChange, parent, newSize)
    else
      local delta = mousePos.x - self:getX() - self:getWidth()/2
      local parent = self:getParent()
      local newSize = math.min(math.max(parent:getWidth() + delta, self.minimum), self.maximum)
      parent:setWidth(newSize)
      signalcall(parent.onWidthChange, parent, newSize)
    end
    return true
  end
end

function UIResizeBorder:onMouseRelease(mousePos, mouseButton)
  if not self:isHovered() then
    g_mouse.restoreCursor()
    g_effects.fadeOut(self)
    self.hovering = false
  end
end

function UIResizeBorder:onStyleApply(styleName, styleNode)
  for name,value in pairs(styleNode) do
    if name == 'maximum' then
      self:setMaximum(tonumber(value))
    elseif name == 'minimum' then
      self:setMinimum(tonumber(value))
    end
  end
end

function UIResizeBorder:onVisibilityChange(visible)
  if visible and self.maximum == self.minimum then
    self:hide()
  end
end

function UIResizeBorder:setMaximum(maximum)
  self.maximum  = maximum
  if self.maximum == self.minimum then
    self:hide()
  end

  local parent = self:getParent()
  if self:isVisible() and parent:getHeight() > maximum then
    parent:setHeight(maximum)
  end
end

function UIResizeBorder:setMinimum(minimum)
  self.minimum = minimum
  if self.maximum == self.minimum then
    self:hide()
  end

  local parent = self:getParent()
  if self:isVisible() and parent:getHeight() < minimum then
    parent:setHeight(minimum)
  end
end

function UIResizeBorder:getMaximum() return self.maximum end
function UIResizeBorder:getMinimum() return self.minimum end
