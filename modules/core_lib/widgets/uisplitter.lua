UISplitter = extends(UIWidget)

function UISplitter.create()
  local splitter = UISplitter.internalCreate()
  splitter:setFocusable(false)
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


function UISplitter:onMouseMove(mousePos, mouseMoved)
  if self:isPressed() then
    if self.vertical then
      local delta = mousePos.y - self:getY()
      local currentMargin = self:getMarginBottom()
      local newMargin = self:canUpdateMargin(self:getMarginBottom() - delta)
      if newMargin ~= currentMargin then
        self.newMargin = newMargin
        if not self.event or self.event:isExecuted() then
          self.event = addEvent(function()
            self:setMarginBottom(self.newMargin)
          end)
        end
      end
    else
      --TODO
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
  if styleNode['relative-margin'] then
    self.relativeMargin = styleNode['relative-margin']
  end
end

function UISplitter:canUpdateMargin(newMargin)
  return newMargin
end
