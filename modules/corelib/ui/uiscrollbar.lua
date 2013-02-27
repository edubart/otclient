-- @docclass
UIScrollBar = extends(UIWidget)

-- private functions
local function calcValues(self)
  local slider = self:getChildById('sliderButton')
  local decrementButton = self:getChildById('decrementButton')
  local incrementButton = self:getChildById('incrementButton')

  local pxrange, center
  if self.orientation == 'vertical' then
    pxrange = (self:getHeight() - decrementButton:getHeight() - decrementButton:getMarginTop() - decrementButton:getMarginBottom()
                                - incrementButton:getHeight() - incrementButton:getMarginTop() - incrementButton:getMarginBottom())
    center = self:getY() + math.floor(self:getHeight() / 2)
  else -- horizontal
    pxrange = (self:getWidth() - decrementButton:getWidth() - decrementButton:getMarginLeft() - decrementButton:getMarginRight()
                               - incrementButton:getWidth() - incrementButton:getMarginLeft() - incrementButton:getMarginRight())
    center = self:getX() + math.floor(self:getWidth() / 2)
  end

  local range = self.maximum - self.minimum + 1

  local proportion

  if self.pixelsScroll then
    proportion = pxrange/(range+pxrange)
  else
    proportion = math.min(math.max(self.step, 1), range)/range
  end

  local px = math.max(proportion * pxrange, 10)
  px = px - px % 2 + 1

  local offset = 0
  if range == 0 or self.value == self.minimum then
    if self.orientation == 'vertical' then
      offset = -math.floor((self:getHeight() - px) / 2) + decrementButton:getMarginRect().height
    else
      offset = -math.floor((self:getWidth() - px) / 2) + decrementButton:getMarginRect().width
    end
  elseif range > 1 and self.value == self.maximum then
    if self.orientation == 'vertical' then
      offset = math.ceil((self:getHeight() - px) / 2) - incrementButton:getMarginRect().height
    else
      offset = math.ceil((self:getWidth() - px) / 2) - incrementButton:getMarginRect().width
    end
  elseif range > 1 then
    offset = (((self.value - self.minimum) / (range - 1)) - 0.5) * (pxrange - px)
  end

  return range, pxrange, px, offset, center
end

local function updateValueDisplay(widget)
  if widget == nil then return end

  if widget:getShowValue() then
    widget:setText(widget:getValue() .. (widget:getSymbol() or ''))
  end
end

local function updateSlider(self)
  local slider = self:getChildById('sliderButton')
  if slider == nil then return end

  local range, pxrange, px, offset, center = calcValues(self)
  if self.orientation == 'vertical' then
    slider:setHeight(px)
    slider:setMarginTop(offset)
  else -- horizontal
    slider:setWidth(px)
    slider:setMarginLeft(offset)
  end
  updateValueDisplay(self)

  local status = (self.maximum ~= self.minimum)

  self:setOn(status)
  for _i,child in pairs(self:getChildren()) do
    child:setEnabled(status)
  end
end

local function parseSliderPos(self, slider, pos, move)
  local delta, hotDistance
  if self.orientation == 'vertical' then
    delta = move.y
    hotDistance = pos.y - slider:getY()
  else
    delta = move.x
    hotDistance = pos.x - slider:getX()
  end

  if (delta > 0 and hotDistance + delta > self.hotDistance) or
     (delta < 0 and hotDistance + delta < self.hotDistance) then
    local range, pxrange, px, offset, center = calcValues(self)
    local newvalue = self.value + delta * (range / (pxrange - px))
    self:setValue(newvalue)
  end
end

local function parseSliderPress(self, slider, pos, button)
  if self.orientation == 'vertical' then
    self.hotDistance = pos.y - slider:getY()
  else
    self.hotDistance = pos.x - slider:getX()
  end
end

-- public functions
function UIScrollBar.create()
  local scrollbar = UIScrollBar.internalCreate()
  scrollbar:setFocusable(false)
  scrollbar.value = 0
  scrollbar.minimum = -999999
  scrollbar.maximum = 999999
  scrollbar.step = 1
  scrollbar.orientation = 'vertical'
  scrollbar.pixelsScroll = false
  scrollbar.showValue = false
  scrollbar.symbol = nil
  scrollbar.mouseScroll = true
  return scrollbar
end

function UIScrollBar:onSetup()
  self.setupDone = true
  local sliderButton = self:getChildById('sliderButton')
  g_mouse.bindAutoPress(self:getChildById('decrementButton'), function() self:decrement() end, 300)
  g_mouse.bindAutoPress(self:getChildById('incrementButton'), function() self:increment() end, 300)
  g_mouse.bindPressMove(sliderButton, function(mousePos, mouseMoved) parseSliderPos(self, sliderButton, mousePos, mouseMoved) end)
  g_mouse.bindPress(sliderButton, function(mousePos, mouseButton) parseSliderPress(self, sliderButton, mousePos, mouseButton) end)

  updateSlider(self)
end

function UIScrollBar:onStyleApply(styleName, styleNode)
  for name,value in pairs(styleNode) do
    if name == 'maximum' then
      self:setMaximum(tonumber(value))
    elseif name == 'minimum' then
      self:setMinimum(tonumber(value))
    elseif name == 'step' then
      self:setStep(tonumber(value))
    elseif name == 'orientation' then
      self:setOrientation(value)
    elseif name == 'value' then
      self:setValue(value)
    elseif name == 'pixels-scroll' then
      self.pixelsScroll = true
    elseif name == 'show-value' then
      self.showValue = true
    elseif name == 'symbol' then
      self.symbol = value
    elseif name == 'mouse-scroll' then
      self.mouseScroll = value
    end
  end
end

function UIScrollBar:decrement(count)
  count = count or self.step
  self:setValue(self.value - count)
end

function UIScrollBar:increment(count)
  count = count or self.step
  self:setValue(self.value + count)
end

function UIScrollBar:setMaximum(maximum)
  if maximum == self.maximum then return end
  self.maximum = maximum
  if self.minimum > maximum then
    self:setMinimum(maximum)
  end
  if self.value > maximum then
    self:setValue(maximum)
  else
    updateSlider(self)
  end
end

function UIScrollBar:setMinimum(minimum)
  if minimum == self.minimum then return end
  self.minimum = minimum
  if self.maximum < minimum then
    self:setMaximum(minimum)
  end
  if self.value < minimum then
    self:setValue(minimum)
  else
    updateSlider(self)
  end
end

function UIScrollBar:setRange(minimum, maximum)
  self:setMinimum(minimum)
  self:setMaximum(maximum)
end

function UIScrollBar:setValue(value)
  value = math.max(math.min(value, self.maximum), self.minimum)
  if self.value == value then return end
  local delta = value - self.value
  self.value = value
  updateSlider(self)
  if self.setupDone then
    signalcall(self.onValueChange, self, math.round(value), delta)
  end
end

function UIScrollBar:setMouseScroll(scroll)
  self.mouseScroll = scroll
end

function UIScrollBar:setStep(step)
  self.step = step
end

function UIScrollBar:setOrientation(orientation)
  self.orientation = orientation
end

function UIScrollBar:setText(text)
  local valueLabel = self:getChildById('valueLabel')
  if valueLabel then
    valueLabel:setText(text)
  end
end

function UIScrollBar:onGeometryChange()
  updateSlider(self)
end

function UIScrollBar:onMouseWheel(mousePos, mouseWheel)
  if not self.mouseScroll then
    return false
  end
  if mouseWheel == MouseWheelUp then
    if self.orientation == 'vertical' then
      self:decrement()
    else
      self:increment()
    end
  else
    if self.orientation == 'vertical' then
      self:increment()
    else
      self:decrement()
    end
  end
  return true
end

function UIScrollBar:getMaximum() return self.maximum end
function UIScrollBar:getMinimum() return self.minimum end
function UIScrollBar:getValue() return math.round(self.value) end
function UIScrollBar:getStep() return self.step end
function UIScrollBar:getOrientation() return self.orientation end
function UIScrollBar:getShowValue() return self.showValue end
function UIScrollBar:getSymbol() return self.symbol end
function UIScrollBar:getMouseScroll() return self.mouseScroll end