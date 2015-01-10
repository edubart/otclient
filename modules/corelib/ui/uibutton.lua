-- @docclass
UIButton = extends(UIWidget, "UIButton")

function UIButton.create()
  local button = UIButton.internalCreate()
  button:setFocusable(false)
  return button
end

function UIButton:onMouseRelease(pos, button)
  return self:isPressed()
end
