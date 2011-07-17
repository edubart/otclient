MessageBox = {}
MessageBox.__index = MessageBox

-- messagebox flags
MessageBoxOk = 1
MessageBoxCancel = 2

function MessageBox.create(title, text, flags)
  local box = {}
  setmetatable(box, MessageBox)

  -- create messagebox window
  local window = UIWindow.new("messageBoxWindow", rootUI)
  window.title = title
  window:centerIn(rootUI)
  window:setLocked()

  -- create messagebox label
  local label = UILabel.new("messageBoxLabel", window)
  label.text = text
  label:addAnchor(AnchorHorizontalCenter, window, AnchorHorizontalCenter)
  label:addAnchor(AnchorTop, window, AnchorTop)
  label:setMargin(27, 0)

  -- set window size based on label size
  window.width = label.width + 40
  window.height = label.height + 64

  -- setup messagebox first button
  local buttonText
  local button1 = UIButton.new("messageBoxButton1", window)
  button1:addAnchor(AnchorBottom, window, AnchorBottom)
  button1:addAnchor(AnchorRight, window, AnchorRight)
  button1:setMargin(10)

  if flags == MessageBoxOk then
    buttonText = "Ok"
    box.onOk = createEmptyFunction()
    button1.onClick = function()
      box.onOk()
      box:destroy()
    end
  elseif flags == MessageBoxCancel then
    buttonText = "Cancel"
    box.onCancel = createEmptyFunction()
    button1.onClick = function()
      box.onCancel()
      box:destroy()
    end
  end
  button1.text = buttonText

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

