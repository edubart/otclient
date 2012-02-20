UIComboBox = extends(UIWidget)

function UIComboBox.create()
  local combobox = UIComboBox.internalCreate()
  combobox.m_options = {}
  combobox.m_currentIndex = -1
  return combobox
end

function UIComboBox:setCurrentOption(text)
  if not self.m_options then return end
  for i,v in ipairs(self.m_options) do
    if v.text == text and self.m_currentIndex ~= i then
      self.m_currentIndex = i
      self:setText(text)
      self:onOptionChange(text, v.data)
      return
    end
  end
end

function UIComboBox:setCurrentIndex(index)
  if index >= 1 and index <= #self.m_options then
    local v = self.m_options[index]
    self.m_currentIndex = index
    self:setText(v.text)
    self:onOptionChange(v.text, v.data)
  end
end

function UIComboBox:addOption(text, data)
  table.insert(self.m_options, { text = text, data = data })
  local index = #self.m_options
  if index == 1 then self:setCurrentOption(text) end
  return index
end

function UIComboBox:onMousePress(mousePos, mouseButton)
  local menu = createWidget(self:getStyleName() .. 'PopupMenu', self)
  for i,v in ipairs(self.m_options) do
    menu:addOption(v.text, function() self:setCurrentOption(v.text) end)
  end
  menu:setWidth(self:getWidth())
  menu:display({ x = self:getX(), y = self:getY() + self:getHeight() })
  connect(menu, { onDestroy = function() self:setOn(false) end })
  self:setOn(true)
  return true
end

function UIComboBox:onMouseWheel(mousePos, direction)
  if direction == MouseWheelUp and self.m_currentIndex > 1 then
    self:setCurrentIndex(self.m_currentIndex - 1)
  elseif direction == MouseWheelDown and self.m_currentIndex < #self.m_options then
    self:setCurrentIndex(self.m_currentIndex + 1)
  end
  return true
end

function UIComboBox:onStyleApply(styleName, styleNode)
  if styleNode.options then
    for k,option in pairs(styleNode.options) do
      self:addOption(option)
    end
  end
end

function UIComboBox:onOptionChange(optionText, optionData)
  -- nothing todo
end
