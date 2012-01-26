UISpinBox = extends(UILineEdit)

function UISpinBox.create()
  local spinbox = UISpinBox.internalCreate()
  spinbox:setValidCharacters('0123456789')
  spinbox.m_minimum = 0
  spinbox.m_maximum = 0
  spinbox:setCurrentIndex(0)
  return spinbox
end

function UISpinBox:setCurrentIndex(index)
  if index >= self.m_minimum and index <= self.m_maximum then
    self.m_currentIndex = index
    self:setText(index)
    self:onIndexChange(index)
  end
end

function UISpinBox:setMinimum(minimum)
  if minimum > self.m_maximum then
    print("[UISpinBox:setMinimum]: minimum value cant be greater than maximum")
    return false
  end
  if self.m_currentIndex < minimum then
    self:setCurrentIndex(minimum)
  end
  self.m_minimum = minimum
end

function UISpinBox:setMaximum(maximum)
  if maximum < self.m_minimum then
    print("[UISpinBox:setMaximum]: maximum value cant be lower than minimum")
    return false
  end
  if self.m_currentIndex > maximum then
    self:setCurrentIndex(maximum)
  end
  self.m_maximum = maximum
end

function UISpinBox:getCurrentIndex()
  return self.m_currentIndex
end

function UISpinBox:onMouseWheel(mousePos, direction)
  if direction == MouseWheelUp then
    self:setCurrentIndex(self.m_currentIndex + 1)
  elseif direction == MouseWheelDown then
    self:setCurrentIndex(self.m_currentIndex - 1)
  end
  return true
end

function UISpinBox:onStyleApply(styleName, styleNode)
  if styleNode.options then
    for k,option in pairs(styleNode.options) do
      self:addOption(option)
    end
  end
end

function UISpinBox:onTextChange(text)
  local number = tonumber(text)
  if not number or number > self.m_maximum or number < self.m_minimum then
    -- todo: restore old text instead of setting minimum
    self:setCurrentIndex(self.m_minimum)
  end
end

function UISpinBox:onIndexChange(index)
  -- nothing todo
end
