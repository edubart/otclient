ToolTip = {}

-- private variables
local currentToolTip

-- private functions
local function moveToolTip(tooltip)
  local pos = getMouseCursorPos()
  pos.y = pos.y + 1
  local xdif = getScreenSize().width - (pos.x + tooltip:getWidth())
  if xdif < 2 then
    pos.x = pos.x - tooltip:getWidth() - 3
  else
    pos.x = pos.x + 10
  end
  tooltip:moveTo(pos)
end

-- public functions
function ToolTip.display(text)
  if text then
    ToolTip.hide()
    currentToolTip = UI.loadAndDisplay('/tooltip/tooltip.otui', UI.root)
    currentToolTip.onMouseMove = moveToolTip
    local label = currentToolTip:getChildById('toolTipText')
    label:setText(text)
    label:resizeToText()
    local size = label:getSize()
    size.width = size.width + 4
    size.height = size.height + 4
    currentToolTip:setSize(size)
    moveToolTip(currentToolTip)
  end
end

function ToolTip.hide()
  if currentToolTip then
    currentToolTip:destroy()
    currentToolTip = nil
  end
end

-- hooks
local function onWidgetHoverChange(widget, hovered)
  if hovered then
    ToolTip.display(widget.tooltip)
  else
    ToolTip:hide()
  end
end

local function onWidgetStyleApply(widget, style)
  if not style then return end
  widget.tooltip = style.tooltip
end

connect(UIWidget, {  onStyleApply = onWidgetStyleApply,
                     onHoverChange = onWidgetHoverChange})

-- public extensions
function UIWidget:setTooltip(text)
  self.tooltip = text
end

function UIWidget:getTooltip()
  return self.tooltip
end

