-- @docclass
UILabel = extends(UIWidget, "UILabel")

function UILabel.create()
  local label = UILabel.internalCreate()
  label:setPhantom(true)
  label:setFocusable(false)
  label:setTextAlign(AlignLeft)
  return label
end
