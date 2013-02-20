-- @docclass
UISpinBox = extends(UITextEdit)

function UISpinBox.create()
  local spinbox = UISpinBox.internalCreate()
  spinbox:setFocusable(false)
  spinbox:setValidCharacters('0123456789')
  spinbox.displayButtons = true
  spinbox.minimum = 0
  spinbox.maximum = 0
  spinbox.value = 0
  spinbox.step = 1
  spinbox.firstchange = true
  spinbox:setText("0")
  return spinbox
end

function UISpinBox:onSetup()
  g_mouse.bindAutoPress(self:getChildById('up'), function() self:up() end, 300)
  g_mouse.bindAutoPress(self:getChildById('down'), function() self:down() end, 300)
end

function UISpinBox:onMouseWheel(mousePos, direction)
  if direction == MouseWheelUp then
    self:up()
  elseif direction == MouseWheelDown then
    self:down()
  end
  return true
end

function UISpinBox:onKeyPress()
  if self.firstchange then
    self.firstchange = false
    self:setText('')
  end
  return false
end

function UISpinBox:onTextChange(text, oldText)
  if text:len() == 0 then
    self:setValue(self.minimum)
    return
  end

  local number = tonumber(text)
  if not number then
    self:setText(number)
    return
  else
    if number < self.minimum then
      self:setText(self.minimum)
      return
    elseif number > self.maximum then
      self:setText(self.maximum)
      return
    end
  end

  self:setValue(number)
end

function UISpinBox:onValueChange(value)
  -- nothing todo
end

function UISpinBox:onStyleApply(styleName, styleNode)
  for name, value in pairs(styleNode) do
    if name == 'maximum' then
      addEvent(function() self:setMaximum(value) end)
    elseif name == 'minimum' then
      addEvent(function() self:setMinimum(value) end)
    elseif name == 'buttons' then
      addEvent(function()
        if value then
          self:showButtons()
        else
          self:hideButtons()
        end
      end)
    end
  end
end

function UISpinBox:showButtons()
  self:getChildById('up'):show()
  self:getChildById('down'):show()
  self.displayButtons = true
end

function UISpinBox:hideButtons()
  self:getChildById('up'):hide()
  self:getChildById('down'):hide()
  self.displayButtons = false
end

function UISpinBox:up()
  self:setValue(self.value + self.step)
end

function UISpinBox:down()
  self:setValue(self.value - self.step)
end

function UISpinBox:setValue(value)
  value = value or 0
  value = math.max(math.min(self.maximum, value), self.minimum)
  if value == self.value then return end
  if self:getText():len() > 0 then
    self:setText(value)
  end
  self.value = value

  local upButton = self:getChildById('up')
  local downButton = self:getChildById('down')
  if upButton then
    upButton:setEnabled(self.maximum ~= self.minimum and self.value ~= self.maximum)
  end
  if downButton then
    downButton:setEnabled(self.maximum ~= self.minimum and self.value ~= self.minimum)
  end

  signalcall(self.onValueChange, self, value)
end

function UISpinBox:getValue()
  return self.value
end

function UISpinBox:setMinimum(minimum)
  minimum = minimum or -9223372036854775808
  self.minimum = minimum
  if self.minimum > self.maximum then
    self.maximum = self.minimum
  end
  if self.value < minimum then
    self:setValue(minimum)
  end
end

function UISpinBox:getMinimum()
  return self.minimum
end

function UISpinBox:setMaximum(maximum)
  maximum = maximum or 9223372036854775807
  self.maximum = maximum
  if self.value > maximum then
    self:setValue(maximum)
  end
end

function UISpinBox:getMaximum()
  return self.maximum
end

function UISpinBox:setStep(step)
  self.step = step or 1
end
