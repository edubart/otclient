UICheckBox = extends(UIWidget)

function UICheckBox.create()
  local checkbox = UICheckBox.internalCreate()
  checkbox:setFocusable(false)
  checkbox:setTextAlign(AlignLeft)
  return checkbox
end

function UICheckBox:onMouseRelease(mousePos, mouseButton)
  if self:isPressed() and self:containsPoint(mousePos) then
    self:setChecked(not self:isChecked())
  end
end
