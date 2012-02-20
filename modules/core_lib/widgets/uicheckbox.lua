UICheckBox = extends(UIWidget)

function UICheckBox.create()
  local checkbox = UICheckBox.internalCreate()
  checkbox:setFocusable(false)
  checkbox:setTextAlign(AlignLeft)
  return checkbox
end

function UICheckBox:onMousePress(mousePos, mouseButton)
  self:setChecked(not self:isChecked())
  return true
end
