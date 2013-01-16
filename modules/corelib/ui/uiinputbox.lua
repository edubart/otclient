if not UIWindow then dofile 'uiwindow' end

-- @docclass
UIInputBox = extends(UIWindow)

function UIInputBox.create(title, okCallback, cancelCallback)
  local inputBox = UIInputBox.internalCreate()

  inputBox:setText(title)
  inputBox.inputs = {}
  inputBox.onEnter = function()
    local results = {}
    for _,func in pairs(inputBox.inputs) do
      table.insert(results, func())
    end
    okCallback(unpack(results))
    inputBox:destroy()
  end
  inputBox.onEscape = function()
    if cancelCallback then
      cancelCallback()
    end
    inputBox:destroy()
  end

  return inputBox
end

function UIInputBox:addLabel(text)
  local label = g_ui.createWidget('InputBoxLabel', self)
  label:setText(text)
  return label
end

function UIInputBox:addLineEdit(labelText, defaultText, maxLength)
  if labelText then self:addLabel(labelText) end
  local lineEdit = g_ui.createWidget('InputBoxLineEdit', self)
  if defaultText then lineEdit:setText(defaultText) end
  if maxLength then lineEdit:setMaxLength(maxLength) end
  table.insert(self.inputs, function() return lineEdit:getText() end)
  return lineEdit
end

function UIInputBox:addTextEdit(labelText, defaultText, maxLength, visibleLines)
  if labelText then self:addLabel(labelText) end
  local textEdit = g_ui.createWidget('InputBoxTextEdit', self)
  if defaultText then textEdit:setText(defaultText) end
  if maxLength then textEdit:setMaxLength(maxLength) end
  visibleLines = visibleLines or 1
  textEdit:setHeight(textEdit:getHeight() * visibleLines)
  table.insert(self.inputs, function() return textEdit:getText() end)
  return textEdit
end

function UIInputBox:addCheckBox(text, checked)
  local checkBox = g_ui.createWidget('InputBoxCheckBox', self)
  checkBox:setText(text)
  checkBox:setChecked(checked)
  table.insert(self.inputs, function() return checkBox:isChecked() end)
  return checkBox
end

function UIInputBox:addComboBox(labelText, ...)
  if labelText then self:addLabel(labelText) end
  local comboBox = g_ui.createWidget('InputBoxComboBox', self)
  local options = {...}
  for i=1,#options do
    comboBox:addOption(options[i])
  end
  table.insert(self.inputs, function() return comboBox:getCurrentOption() end)
  return comboBox
end

function UIInputBox:addSpinBox(labelText, minimum, maximum, value, step)
  if labelText then self:addLabel(labelText) end
  local spinBox = g_ui.createWidget('InputBoxSpinBox', self)
  spinBox:setMinimum(minimum)
  spinBox:setMaximum(maximum)
  spinBox:setValue(value)
  spinBox:setStep(step)
  table.insert(self.inputs, function() return spinBox:getValue() end)
  return spinBox
end

function UIInputBox:display(okButtonText, cancelButtonText)
  okButtonText = okButtonText or tr('Ok')
  cancelButtonText = cancelButtonText or tr('Cancel')

  local buttonsWidget = g_ui.createWidget('InputBoxButtonsPanel', self)
  local okButton = g_ui.createWidget('InputBoxButton', buttonsWidget)
  okButton:setText(okButtonText)
  okButton.onClick = self.onEnter

  local cancelButton = g_ui.createWidget('InputBoxButton', buttonsWidget)
  cancelButton:setText(cancelButtonText)
  cancelButton.onClick = self.onEscape

  buttonsWidget:setHeight(okButton:getHeight())

  rootWidget:addChild(self)
  self:setStyle('InputBoxWindow')
end

function displayTextInputBox(title, label, okCallback, cancelCallback)
  local inputBox = UIInputBox.create(title, okCallback, cancelCallback)
  inputBox:addLineEdit(label)
  inputBox:display()
end

function displayNumberInputBox(title, label, okCallback, cancelCallback, min, max, value, step)
  local inputBox = UIInputBox.create(title, okCallback, cancelCallback)
  inputBox:addSpinBox(label, min, max, value, step)
  inputBox:display()
end
