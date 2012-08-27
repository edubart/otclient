-- @docclass
UIComboBox = extends(UIWidget)

function UIComboBox.create()
  local combobox = UIComboBox.internalCreate()
  combobox.options = {}
  combobox.currentIndex = -1
  return combobox
end

function UIComboBox:clearOptions()
  self.options = {}
  self.currentIndex = -1
  self:clearText()
end

function UIComboBox:setCurrentOption(text)
  if not self.options then return end
  for i,v in ipairs(self.options) do
    if v.text == text and self.currentIndex ~= i then
      self.currentIndex = i
      self:setText(text)
      self:onOptionChange(text, v.data)
      return
    end
  end
end

function UIComboBox:setCurrentIndex(index)
  if index >= 1 and index <= #self.options then
    local v = self.options[index]
    self.currentIndex = index
    self:setText(v.text)
    self:onOptionChange(v.text, v.data)
  end
end

function UIComboBox:getCurrentOption()
  if table.hasKey(self.options, self.currentIndex) then
    return self.options[self.currentIndex]
  end
end

function UIComboBox:addOption(text, data)
  table.insert(self.options, { text = text, data = data })
  local index = #self.options
  if index == 1 then self:setCurrentOption(text) end
  return index
end

function UIComboBox:removeOption(text)
  for i,v in ipairs(self.options) do
    if v.text == text then
      table.remove(self.options, i)
      if self.currentIndex == i then
        self:setCurrentIndex(1)
      elseif self.currentIndex > i then
        self.currentIndex = self.currentIndex - 1
      end
      return
    end
  end
end

function UIComboBox:onMousePress(mousePos, mouseButton)
  local menu = g_ui.createWidget(self:getStyleName() .. 'PopupMenu')
  menu:setId(self:getId() .. 'PopupMenu')
  for i,v in ipairs(self.options) do
    menu:addOption(v.text, function() self:setCurrentOption(v.text) end)
  end
  menu:setWidth(self:getWidth())
  menu:display({ x = self:getX(), y = self:getY() + self:getHeight() })
  connect(menu, { onDestroy = function() self:setOn(false) end })
  self:setOn(true)
  return true
end

function UIComboBox:onMouseWheel(mousePos, direction)
  if direction == MouseWheelUp and self.currentIndex > 1 then
    self:setCurrentIndex(self.currentIndex - 1)
  elseif direction == MouseWheelDown and self.currentIndex < #self.options then
    self:setCurrentIndex(self.currentIndex + 1)
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
