if not UIWindow then dofile 'uiwindow' end

UIMessageBox = extends(UIWindow)

MessageBoxOk = 1
MessageBoxCancel = 2

-- messagebox cannot be created from otui files
UIMessageBox.create = nil

function UIMessageBox.display(title, message, flags)
  local messagebox = UIMessageBox.internalCreate()
  rootWidget:addChild(messagebox)

  messagebox:setStyle('MessageBoxWindow')
  messagebox:setText(title)

  local messageLabel = createWidget('MessageBoxLabel', messagebox)
  messageLabel:setText(message)
  messageLabel:resizeToText()

  messagebox:setWidth(math.max(messageLabel:getWidth() + 48, messagebox:getWidth()))
  messagebox:setHeight(math.max(messageLabel:getHeight() + 64, messagebox:getHeight()))

  -- setup messagebox first button
  local buttonRight = createWidget('MessageBoxRightButton', messagebox)

  if flags == MessageBoxOk then
    buttonRight:setText('Ok')
    connect(buttonRight, { onClick = function(self) self:getParent():ok() end })
    connect(messagebox, { onEnter = function(self) self:ok() end })
    connect(messagebox, { onEscape = function(self) self:ok() end })
  elseif flags == MessageBoxCancel then
    buttonRight:setText('Cancel')
    connect(buttonRight, { onClick = function(self) self:getParent():cancel() end })
    connect(messagebox, { onEnter = function(self) self:cancel() end })
    connect(messagebox, { onEscape = function(self) self:cancel() end })
  end

  messagebox:lock()

  return messagebox
end

function displayInfoBox(title, message)
  return UIMessageBox.display(title, message, MessageBoxOk)
end

function displayErrorBox(title, message)
  return UIMessageBox.display(title, message, MessageBoxOk)
end

function displayCancelBox(title, message)
  return UIMessageBox.display(title, message, MessageBoxCancel)
end

function UIMessageBox:ok()
  signalcall(self.onOk, self)
  self.onOk = nil
  self:destroy()
end

function UIMessageBox:cancel()
  signalcall(self.onCancel, self)
  self.onCancel = nil
  self:destroy()
end
