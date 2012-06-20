UISplitter = extends(UIWidget)

function UISplitter.create()
  local splitter = UISplitter.internalCreate()
  splitter:setFocusable(false)
  splitter.relativeMargin = 'bottom'
  return splitter
end

function UISplitter:onHoverChange(hovered)
  if hovered then
    if Mouse.isCursorChanged() or Mouse.isPressed() then return end
    if self:getWidth() > self:getHeight() then
      Mouse.setVerticalCursor()
      self.vertical = true
    else
      Mouse.setHorizontalCursor()
      self.vertical = false
    end
    self.hovering = true
    if not self:isPressed() then
      Effects.fadeIn(self)
    end
  else
    if not self:isPressed() and self.hovering then
      Mouse.restoreCursor()
      Effects.fadeOut(self)
      self.hovering = false
    end
  end
end

function UISplitter:onMouseMove(mousePos, mouseMoved)
  if self:isPressed() then
    --local currentmargin, newmargin, delta
    if self.vertical then
      local delta = mousePos.y - self:getY() - self:getHeight()/2
      local newMargin = self:canUpdateMargin(self:getMarginBottom() - delta)
      local currentMargin = self:getMarginBottom()
      if newMargin ~= currentMargin then
        self.newMargin = newMargin
        if not self.event or self.event:isExecuted() then
          self.event = addEvent(function()
            self:setMarginBottom(self.newMargin)
          end)
        end
      end
    else
      local delta = mousePos.x - self:getX() - self:getWidth()/2
      local newMargin = self:canUpdateMargin(self:getMarginRight() - delta)
      local currentMargin = self:getMarginRight()
      if newMargin ~= currentMargin then
        self.newMargin = newMargin
        if not self.event or self.event:isExecuted() then
          self.event = addEvent(function()
            self:setMarginRight(self.newMargin)
          end)
        end
      end
    end
    return true
  end
end

function UISplitter:onMouseRelease(mousePos, mouseButton)
  if not self:isHovered() then
    Mouse.restoreCursor()
    Effects.fadeOut(self)
    self.hovering = false
  end
end

function UISplitter:onStyleApply(styleName, styleNode)
  if styleNode['relative-margin'] then
    self.relativeMargin = styleNode['relative-margin']
  end
end

function UISplitter:canUpdateMargin(newMargin)
  return newMargin
end
