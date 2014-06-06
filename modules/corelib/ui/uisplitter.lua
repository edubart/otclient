-- @docclass
UISplitter = extends(UIWidget, "UISplitter")

function UISplitter.create()
  local splitter = UISplitter.internalCreate()
  splitter:setFocusable(false)
  splitter.relativeMargin = 'bottom'
  return splitter
end

function UISplitter:onHoverChange(hovered)
  -- Check if margin can be changed
  local margin = (self.vertical and self:getMarginBottom() or self:getMarginRight())
  if hovered and (self:canUpdateMargin(margin + 1) ~= margin or self:canUpdateMargin(margin - 1) ~= margin) then
    if g_mouse.isCursorChanged() or g_mouse.isPressed() then return end
    if self:getWidth() > self:getHeight() then
      self.vertical = true
      self.cursortype = 'vertical'
    else
      self.vertical = false
      self.cursortype = 'horizontal'
    end
    self.hovering = true
    g_mouse.pushCursor(self.cursortype)
    if not self:isPressed() then
      g_effects.fadeIn(self)
    end
  else
    if not self:isPressed() and self.hovering then
      g_mouse.popCursor(self.cursortype)
      g_effects.fadeOut(self)
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
    g_mouse.popCursor(self.cursortype)
    g_effects.fadeOut(self)
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
