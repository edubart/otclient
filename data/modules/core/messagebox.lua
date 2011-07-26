MessageBox = {}
MessageBox.__index = MessageBox

-- messagebox flags
MessageBoxOk = 1
MessageBoxCancel = 2

function MessageBox.create(title, text, flags)
  local box = {}
  setmetatable(box, MessageBox)

  -- create messagebox window
  local window = UIWindow.create()
  id = "messageBoxWindow"
  window.title = title
  window:centerIn(rootUI)
  window:setLocked(true)
  rootUI:addChild(window)

  -- create messagebox label
  local label = UILabel.create()
  id = "messageBoxLabel"
  label.text = text
  label:addAnchor(AnchorHorizontalCenter, window.id, AnchorHorizontalCenter)
  label:addAnchor(AnchorTop, window.id, AnchorTop)
  --label:setMargin(27, 0)
  window:addChild(label)

  -- set window size based on label size
  window.width = label.width + 40
  window.height = label.height + 64

  -- setup messagebox first button
  local buttonText
  local button1 = UIButton.new()
  id = "messageBoxButton1"
  button1:addAnchor(AnchorBottom, window.id, AnchorBottom)
  button1:addAnchor(AnchorRight, window.id, AnchorRight)
  --button1:setMargin(10)
  button1.width = 64
  window:addChild(button1)

  if flags == MessageBoxOk then
    button1.text = "Ok"
    box.onOk = createEmptyFunction()
    button1.onClick = function()
      box.onOk()
      box:destroy()
    end
  elseif flags == MessageBoxCancel then
    button1.text = "Cancel"
    box.onCancel = createEmptyFunction()
    button1.onClick = function()
      box.onCancel()
      box:destroy()
    end
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

