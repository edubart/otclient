if not UIWindow then dofile 'uiwindow' end

-- @docclass
UIMessageBox = extends(UIWindow)

MessageBoxOk = 1
MessageBoxCancel = 2

-- messagebox cannot be created from otui files
UIMessageBox.create = nil

function UIMessageBox.display(title, message, flags)
  local messagebox = UIMessageBox.internalCreate()
  rootWidget:addChild(messagebox)

  messagebox:setStyle('MainWindow')
  messagebox:setText(title)

  local messageLabel = g_ui.createWidget('MessageBoxLabel', messagebox)
  messageLabel:setText(message)
  messageLabel:resizeToText()

  -- setup messagebox first button
  local buttonRight = g_ui.createWidget('MessageBoxRightButton', messagebox)

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

  messagebox:setWidth(messageLabel:getWidth() + messagebox:getPaddingLeft() + messagebox:getPaddingRight())
  messagebox:setHeight(messageLabel:getHeight() + messagebox:getPaddingTop() + messagebox:getPaddingBottom() + buttonRight:getHeight() + 10)

  --messagebox:lock()

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
