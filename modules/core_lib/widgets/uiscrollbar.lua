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
    center = self:getY() + self:getHeight() / 2
  else -- horizontal
    pxrange = (self:getWidth() - decrementButton:getWidth() - decrementButton:getMarginLeft() - decrementButton:getMarginRight()
                               - incrementButton:getWidth() - incrementButton:getMarginLeft() - incrementButton:getMarginRight())
    center = self:getX() + self:getWidth() / 2
  end

  local range = self.maximum - self.minimum + 1
  local proportion = math.max(range*(self.step/range), 1)/range
  local px = math.max(math.floor(proportion * pxrange), 10)
  local offset = ((self.value / (range - 1)) - 0.5) * (pxrange - px)

  return range, pxrange, px, offset, center
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

  if self.maximum == self.minimum then
    self:disable()
  else
    self:enable()
  end
end

local function parseSliderPos(self, pos)
  local point
  if self.orientation == 'vertical' then
    point = pos.y
  else
    point = pos.x
  end
  local range, pxrange, px, offset, center = calcValues(self)
  offset = math.min(math.max(point - center, -pxrange/2), pxrange/2)
  local newvalue = math.floor(((offset / (pxrange - px)) + 0.5) * (range - 1))
  self:setValue(newvalue)
end


-- public functions
function UIScrollBar.create()
  local scrollbar = UIScrollBar.internalCreate()
  scrollbar:setFocusable(false)
  scrollbar.value = 0
  scrollbar.minimum = 0
  scrollbar.maximum = 0
  scrollbar.step = 1
  scrollbar.orientation = 'vertical'
  return scrollbar
end

function UIScrollBar:onSetup()
  addEvent(function()
    Mouse.bindAutoPress(self:getChildById('decrementButton'), function() self:decrement() end)
    Mouse.bindAutoPress(self:getChildById('incrementButton'), function() self:increment() end)
    Mouse.bindPressMove(self:getChildById('sliderButton'), function(mousePos, mouseMoved) parseSliderPos(self, mousePos) end)
    updateSlider(self)
  end)
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
    end
  end
end

function UIScrollBar:decrement()
  self:setValue(self.value - self.step)
end

function UIScrollBar:increment()
  self:setValue(self.value + self.step)
end

function UIScrollBar:setMaximum(maximum)
  if maximum == self.maximum then return end
  self.maximum  = maximum
  if self.value > maximum then
    self:setValue(maximum)
  else
    updateSlider(self)
  end
end

function UIScrollBar:setMinimum(minimum)
  if minimum == self.minimum then return end
  self.minimum = minimum
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
  signalcall(self.onValueChange, self, value, delta)
end

function UIScrollBar:setStep(step)
  self.step = step
end

function UIScrollBar:setOrientation(orientation)
  self.orientation = orientation
end

function UIScrollBar:onGeometryChange()
  updateSlider(self)
end

function UIScrollBar:getMaximum() return self.maximum end
function UIScrollBar:getMinimum() return self.minimum end
function UIScrollBar:getValue() return self.value end
function UIScrollBar:getStep() return self.step end
function UIScrollBar:getOrientation() return self.orientation end
