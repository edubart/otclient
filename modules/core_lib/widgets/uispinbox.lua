UISpinBox = extends(UILineEdit)

function UISpinBox.create()
  local spinbox = UISpinBox.internalCreate()
  spinbox:setValidCharacters('0123456789')
  spinbox.minimum = 0
  spinbox.maximum = 0
  spinbox:setCurrentIndex(0)
  spinbox:setText("0")
  return spinbox
end

function UISpinBox:setCurrentIndex(index)
  if index >= self.minimum and index <= self.maximum then
    if self:getText():len() > 0 then
      self:setText(index)
    end
    self.currentIndex = index
    self:onIndexChange(index)
  end
end

function UISpinBox:setMinimum(minimum)
  if minimum > self.maximum then
    print("[UISpinBox:setMinimum]: minimum value cant be greater than maximum")
    return false
  end
  if self.currentIndex < minimum then
    self:setCurrentIndex(minimum)
  end
  self.minimum = minimum
end

function UISpinBox:setMaximum(maximum)
  if maximum < self.minimum then
    print("[UISpinBox:setMaximum]: maximum value cant be lower than minimum")
    return false
  end
  if self.currentIndex > maximum then
    self:setCurrentIndex(maximum)
  end
  self.maximum = maximum
end

function UISpinBox:getCurrentIndex()
  return self.currentIndex
end

function UISpinBox:onMouseWheel(mousePos, direction)
  if direction == MouseWheelUp then
    self:setCurrentIndex(self.currentIndex + 1)
  elseif direction == MouseWheelDown then
    self:setCurrentIndex(self.currentIndex - 1)
  end
  return true
end

function UISpinBox:onTextChange(text, oldText)

  if text:len() == 0 then
    self:setCurrentIndex(self.minimum)
    return
  end

  local number = tonumber(text)
  if not number or number > self.maximum or number < self.minimum then
    self:setText(oldText)
    return
  end
  
  self:setCurrentIndex(number)
end

function UISpinBox:onIndexChange(index)
  -- nothing todo
end

function UISpinBox:onStyleApply(styleName, styleNode)
  -- tonumber converts to 0 if not valid
  if styleNode.maximum and tonumber(styleNode.maximum) then
    self:setMaximum(tonumber(styleNode.maximum))
  end
  
  if styleNode.minimum and tonumber(styleNode.minimum) then
    self:setMinimum(tonumber(styleNode.minimum))
  end
end

