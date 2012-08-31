-- @docclass
UIScrollArea = extends(UIWidget)

-- public functions
function UIScrollArea.create()
  local scrollarea = UIScrollArea.internalCreate()
  scrollarea:setClipping(true)
  scrollarea.inverted = false
  scrollarea.alwaysScrollMaximum = false
  return scrollarea
end

function UIScrollArea:onStyleApply(styleName, styleNode)
  for name,value in pairs(styleNode) do
    if name == 'vertical-scrollbar' then
      addEvent(function()
        self:setVerticalScrollBar(self:getParent():getChildById(value))
      end)
    elseif name == 'horizontal-scrollbar' then
      addEvent(function()
        self:setHorizontalScrollBar(self:getParent():getChildById(value))
      end)
    elseif name == 'inverted-scroll' then
      self:setInverted(value)
    elseif name == 'always-scroll-maximum' then
      self:setAlwaysScrollMaximum(value)
    end
  end
end

function UIScrollArea:updateScrollBars()
  local scrollWidth = math.max(self:getChildrenRect().width - self:getPaddingRect().width, 0)
  local scrollHeight = math.max(self:getChildrenRect().height - self:getPaddingRect().height, 0)

  local scrollbar = self.verticalScrollBar
  if scrollbar then
    if self.inverted then
      scrollbar:setMinimum(-scrollHeight)
      scrollbar:setMaximum(0)
    else
      scrollbar:setMinimum(0)
      scrollbar:setMaximum(scrollHeight)
    end
  end

  local scrollbar = self.horizontalScrollBar
  if scrollbar then
    if self.inverted then
      scrollbar:setMinimum(-scrollWidth)
      scrollbar:setMaximum(0)
    else
      scrollbar:setMinimum(0)
      scrollbar:setMaximum(scrollWidth)
    end
  end

  if self.lastScrollWidth ~= scrollWidth then
    self:onScrollWidthChange()
  end
  if self.lastScrollHeight ~= scrollHeight then
    self:onScrollHeightChange()
  end

  self.lastScrollWidth = scrollWidth
  self.lastScrollHeight = scrollHeight
end

function UIScrollArea:setVerticalScrollBar(scrollbar)
  self.verticalScrollBar = scrollbar
  self.verticalScrollBar.onValueChange = function(scrollbar, value)
    local virtualOffset = self:getVirtualOffset()
    virtualOffset.y = value
    self:setVirtualOffset(virtualOffset)
  end
  self:updateScrollBars()
end

function UIScrollArea:setHorizontalScrollBar(scrollbar)
  self.horizontalScrollBar = scrollbar
  self.horizontalScrollBar.onValueChange = function(scrollbar, value)
    local virtualOffset = self:getVirtualOffset()
    virtualOffset.x = value
    self:setVirtualOffset(virtualOffset)
  end
  self:updateScrollBars()
end

function UIScrollArea:setInverted(inverted)
  self.inverted = inverted
end

function UIScrollArea:setAlwaysScrollMaximum(value)
  self.alwaysScrollMaximum = value
end

function UIScrollArea:onLayoutUpdate()
  self:updateScrollBars()
end

function UIScrollArea:onMouseWheel(mousePos, mouseWheel)
  if self.verticalScrollBar then
    if mouseWheel == MouseWheelUp then
      self.verticalScrollBar:decrement()
    else
      self.verticalScrollBar:increment()
    end
  elseif self.horizontalScrollBar then
    if mouseWheel == MouseWheelUp then
      self.horizontalScrollBar:increment()
    else
      self.horizontalScrollBar:decrement()
    end
  end
  return true
end

function UIScrollArea:onChildFocusChange(focusedChild, oldFocused, reason)
  if focusedChild and (reason == MouseFocusReason or reason == KeyboardFocusReason) then
    local paddingRect = self:getPaddingRect()
    if self.verticalScrollBar then
      local deltaY = paddingRect.y - focusedChild:getY()
      if deltaY > 0 then
        self.verticalScrollBar:decrement(deltaY)
      end

      deltaY = (focusedChild:getY() + focusedChild:getHeight()) - (paddingRect.y + paddingRect.height)
      if deltaY > 0 then
        self.verticalScrollBar:increment(deltaY)
      end
    else
      local deltaX = paddingRect.x - focusedChild:getX()
      if deltaX > 0 then
        self.horizontalScrollBar:decrement(deltaX)
      end

      deltaX = (focusedChild:getX() + focusedChild:getWidth()) - (paddingRect.x + paddingRect.width)
      if deltaX > 0 then
        self.horizontalScrollBar:increment(deltaX)
      end
    end
  end
end

function UIScrollArea:onScrollWidthChange()
  if self.alwaysScrollMaximum and self.horizontalScrollBar then
    self.horizontalScrollBar:setValue(self.horizontalScrollBar:getMaximum())
  end
end

function UIScrollArea:onScrollHeightChange()
  if self.alwaysScrollMaximum and self.verticalScrollBar then
    self.verticalScrollBar:setValue(self.verticalScrollBar:getMaximum())
  end
end
