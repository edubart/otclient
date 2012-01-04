UIComboBox = extends(UIWidget)

function UIComboBox.create()
  local combobox = UIComboBox.internalCreate()
  combobox.options = {}
  combobox.currentIndex = -1
  return combobox
end

function UIComboBox:setCurrentOption(text)
  if not self.options then return end
  for i,v in ipairs(self.options) do
    if v.text == text and self.currentIndex ~= i then
      self.currentIndex = i
      self:setText(text)
      self:onOptionChange(text, data)
      return
    end
  end
end

function UIComboBox:addOption(text, data)
  table.insert(self.options, { text = text, data = data })
  local index = #self.options
  if index == 1 then self:setCurrentOption(text) end
  return index
end

function UIComboBox:onMousePress(mousePos, mouseButton)
  local menu = createWidget(self:getStyleName() .. 'PopupMenu', self)
  for i,v in ipairs(self.options) do
    menu:addOption(v.text, function() self:setCurrentOption(v.text) end)
  end
  menu:setWidth(self:getWidth())
  menu:display({ x = self:getX(), y = self:getY() + self:getHeight() })
  return true
end

function UIComboBox:onOptionChange(optionText, optionData)
  -- nothing todo
end
