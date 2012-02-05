ToolTip = {}

-- private variables
local toolTipLabel
local currentHoveredWidget

-- private functions
local function moveToolTip(tooltip)
  local pos = g_window.getMousePosition()
  pos.y = pos.y + 1
  local xdif = g_window.getSize().width - (pos.x + tooltip:getWidth())
  if xdif < 2 then
    pos.x = pos.x - tooltip:getWidth() - 3
  else
    pos.x = pos.x + 10
  end
  tooltip:setPosition(pos)
end

-- public functions
function ToolTip.display(text)
  if text == nil then return end
  ToolTip.hide()
  toolTipLabel = createWidget('Label', rootWidget)
  toolTipLabel:setId('toolTip')
  toolTipLabel:setBackgroundColor('#111111bb')
  toolTipLabel:setText(text)
  toolTipLabel:resizeToText()
  toolTipLabel:resize(toolTipLabel:getWidth() + 4, toolTipLabel:getHeight() + 4)
  toolTipLabel.onMouseMove = moveToolTip
  moveToolTip(toolTipLabel)
end

function ToolTip.hide()
  if toolTipLabel then
    toolTipLabel:destroy()
    toolTipLabel = nil
  end
end

-- UIWidget hooks
local function onWidgetHoverChange(widget, hovered)
  if hovered then
    if widget.tooltip then
      ToolTip.display(widget.tooltip)
      currentHoveredWidget = widget
    end
  else
    if widget == currentHoveredWidget then
      ToolTip:hide()
      currentHoveredWidget = nil
    end
  end
end

local function onWidgetStyleApply(widget, styleName, styleNode)
  if styleNode.tooltip then
    widget.tooltip = styleNode.tooltip
  end
end

connect(UIWidget, {  onStyleApply = onWidgetStyleApply,
                     onHoverChange = onWidgetHoverChange})

-- UIWidget extensions
function UIWidget:setTooltip(text)
  self.tooltip = text
end

function UIWidget:getTooltip()
  return self.tooltip
end

