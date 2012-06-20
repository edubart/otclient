UISpinBox = extends(UITextEdit)

function UISpinBox.create()
  local spinbox = UISpinBox.internalCreate()
  spinbox:setValidCharacters('0123456789')
  spinbox.minimum = 0
  spinbox.maximum = 0
  spinbox.value = 0
  spinbox:setText("0")
  return spinbox
end

function UISpinBox:onMouseWheel(mousePos, direction)
  if direction == MouseWheelUp then
    self:setValue(self.value + 1)
  elseif direction == MouseWheelDown then
    self:setValue(self.value - 1)
  end
  return true
end

function UISpinBox:onTextChange(text, oldText)
  if text:len() == 0 then
    self:setValue(self.minimum)
    return
  end

  local number = tonumber(text)
  if not number or number > self.maximum or number < self.minimum then
    self:setText(oldText)
    return
  end
  
  self:setValue(number)
end

function UISpinBox:onValueChange(value)
  -- nothing todo
end

function UISpinBox:onStyleApply(styleName, styleNode)
  for name, value in pairs(styleNode) do
    if name == 'maximum' then
      self:setMaximum(value)
    elseif name == 'minimum' then
      self:setMinimum(value)
    end
  end
end

function UISpinBox:setValue(value)
  value = math.max(math.min(self.maximum, value), self.minimum)
  if value == self.value then return end
  if self:getText():len() > 0 then
    self:setText(value)
  end
  self.value = value
  signalcall(self.onValueChange, self, value)
end

function UISpinBox:setMinimum(minimum)
  self.minimum = minimum
  if self.value < minimum then
    self:setValue(minimum)
  end
end

function UISpinBox:setMaximum(maximum)
  self.maximum = maximum
  if self.value > maximum then
    self:setValue(maximum)
  end
end

function UISpinBox:getValue() return self.value end
