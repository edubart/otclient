function UITextEdit:onStyleApply(styleName, styleNode)
  for name,value in pairs(styleNode) do
    if name == 'vertical-scrollbar' then
      addEvent(function()
        self:setVerticalScrollBar(self:getParent():getChildById(value))
      end)
    elseif name == 'horizontal-scrollbar' then
      addEvent(function()
        self:setHorizontalScrollBar(self:getParent():getChildById(value))
      end)
    end
  end
end

function UITextEdit:onMouseWheel(mousePos, mouseWheel)
  if self.verticalScrollBar and self:isMultiline() then
    if mouseWheel == MouseWheelUp then
      self.verticalScrollBar:decrement()
    else
      self.verticalScrollBar:increment()
    end
    return true
  elseif self.horizontalScrollBar then
    if mouseWheel == MouseWheelUp then
      self.horizontalScrollBar:increment()
    else
      self.horizontalScrollBar:decrement()
    end
    return true
  end
end

function UITextEdit:onTextAreaUpdate(virtualOffset, virtualSize, totalSize)
  self:updateScrollBars()
end

function UITextEdit:setVerticalScrollBar(scrollbar)
  self.verticalScrollBar = scrollbar
  self.verticalScrollBar.onValueChange = function(scrollbar, value)
    local virtualOffset = self:getTextVirtualOffset()
    virtualOffset.y = value
    self:setTextVirtualOffset(virtualOffset)
  end
  self:updateScrollBars()
end

function UITextEdit:setHorizontalScrollBar(scrollbar)
  self.horizontalScrollBar = scrollbar
  self.horizontalScrollBar.onValueChange = function(scrollbar, value)
    local virtualOffset = self:getTextVirtualOffset()
    virtualOffset.x = value
    self:setTextVirtualOffset(virtualOffset)
  end
  self:updateScrollBars()
end

function UITextEdit:updateScrollBars()
  local scrollSize = self:getTextTotalSize()
  local scrollWidth = math.max(scrollSize.width - self:getTextVirtualSize().width, 0)
  local scrollHeight = math.max(scrollSize.height - self:getTextVirtualSize().height, 0)

  local scrollbar = self.verticalScrollBar
  if scrollbar then
    scrollbar:setMinimum(0)
    scrollbar:setMaximum(scrollHeight)
    scrollbar:setValue(self:getTextVirtualOffset().y)
  end

  local scrollbar = self.horizontalScrollBar
  if scrollbar then
    scrollbar:setMinimum(0)
    scrollbar:setMaximum(scrollWidth)
    scrollbar:setValue(self:getTextVirtualOffset().x)
  end

end

-- todo: ontext change, focus to cursor