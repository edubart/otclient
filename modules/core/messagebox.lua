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
  window:setStyle('Window')
  window:setId("messageBoxWindow")
  window:setTitle(title)
  window:centerIn("parent")
  rootWidget:addChild(window)
  rootWidget:lockChild(window)

  -- create messagebox label
  local label = UILabel.create()
  label:setStyle('Label')
  label:setId("messageBoxLabel")
  label:setText(text)
  label:addAnchor(AnchorHorizontalCenter, window:getId(), AnchorHorizontalCenter)
  label:addAnchor(AnchorTop, window:getId(), AnchorTop)
  label:setMargin(27, 0)
  label:resizeToText()
  window:addChild(label)

  -- set window size based on label size
  window:setWidth(label:getWidth() + 60)
  window:setHeight(label:getHeight() + 64)

  -- setup messagebox first button
  local button1 = UIButton.create()
  button1:setStyle('Button')
  button1:setId("messageBoxButton1")
  button1:addAnchor(AnchorBottom, window:getId(), AnchorBottom)
  button1:addAnchor(AnchorRight, window:getId(), AnchorRight)
  button1:setMargin(10)
  button1:setWidth(64)
  window:addChild(button1)

  if flags == MessageBoxOk then
    button1:setText("Ok")
    box.onOk = EmptyFunction
    button1.onClick = function()
      box.onOk()
      box:destroy()
    end
  elseif flags == MessageBoxCancel then
    button1:setText("Cancel")
    box.onCancel = EmptyFunction
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

