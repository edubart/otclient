UIMessageBox = extends(UIWindow)

function UIMessageBox.create(title, message)
  local messagebox = UIMessageBox.internalCreate()

  messagebox:setText(title)
  local messageLabel = self:getChildById('messageLabel')
  label:setText(message)
  label:resizeToText()

  window:setWidth(math.max(label:getWidth() + self:getPaddingLeft() + self:getPaddingRight(), self:getWidth()))
  window:setHeight(label:getHeight() + self:getPaddingTop() + self:getPaddingBottom())

  return messagebox
end

function UIMessageBox:setTitle(title)
end

function UIMessageBox:setMessage(message)
end

function