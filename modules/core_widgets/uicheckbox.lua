UICheckBox = extends(UIWidget)

function UICheckBox.create()
  local checkbox = UICheckBox.internalCreate()
  checkbox:setFocusable(false)
  checkbox:setAlign(AlignLeft)
  return checkbox
end
