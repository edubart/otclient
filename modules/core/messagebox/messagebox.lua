MessageBox = {}
MessageBox.__index = MessageBox

-- messagebox flags
MessageBoxOk = 1
MessageBoxCancel = 2

function MessageBox.create(title, text, flags)
  local box = {}
  setmetatable(box, MessageBox)

  -- create messagebox window
  local window = UI.loadAndDisplayLocked('/core/messagebox/messagebox.otui')
  window:setTitle(title)

  local label = window:getChildById('messageBoxLabel')
  label:setStyle('Label')
  label:setText(text)
  label:resizeToText()

  -- set window size based on label size
  window:setWidth(math.max(label:getWidth() + 48, 120))
  window:setHeight(label:getHeight() + 64)
  window:updateParentLayout()

  -- setup messagebox first button
  local buttonRight = window:getChildById('messageBoxRightButton')

  if flags == MessageBoxOk then
    buttonRight:setText("Ok")
    box.onOk = EmptyFunction
    buttonRight.onClick = function()
      box.onOk()
      box:destroy()
    end
    window.onEnter = buttonRight.onClick
    window.onEscape = buttonRight.onClick
  elseif flags == MessageBoxCancel then
    buttonRight:setText("Cancel")
    box.onCancel = EmptyFunction
    buttonRight.onClick = function()
      box.onCancel()
      box:destroy()
    end
    window.onEnter = buttonRight.onClick
    window.onEscape = buttonRight.onClick
  end

  box.window = window
  return box
end

function MessageBox:destroy()
  if self.onDestroy then
    self.onDestroy()
    self.onDestroy = nil
  end
  if self.window then
    self.window:destroy()
    self.window = nil
  end
  self.onOk = nil
  self.onCancel = nil
end

-- shortcuts for creating message boxes
function displayMessageBox(title, text, flags)
  return MessageBox.create(title, text, flags)
end

function displayErrorBox(title, text)
  return MessageBox.create(title, text, MessageBoxOk)
end

function displayInfoBox(title, text)
  return MessageBox.create(title, text, MessageBoxOk)
end

function displayCancelBox(title, text)
  return MessageBox.create(title, text, MessageBoxCancel)
end

