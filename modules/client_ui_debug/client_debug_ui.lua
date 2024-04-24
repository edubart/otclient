local clientUiDebug
local clientUiDebugLabel
local clientUiDebugHighlightWidget

function onClientUiDebuggerMouseMove(mouseBindWidget, mousePos, mouseMove)
    local widgets = rootWidget:recursiveGetChildrenByPos(mousePos)

    local smallestWidget
    for _, widget in pairs(widgets) do
        if (widget:getId() ~= 'highlightWidget' and widget:getId() ~= 'toolTip') then
            if (not smallestWidget or
                    (widget:getSize().width <= smallestWidget:getSize().width and widget:getSize().height <= smallestWidget:getSize().height)
            ) then
                smallestWidget = widget
            end
        end
    end
    if smallestWidget then
        clientUiDebugHighlightWidget:setPosition(smallestWidget:getPosition())
        clientUiDebugHighlightWidget:setSize(smallestWidget:getSize())
        clientUiDebugHighlightWidget:raise()
    end

    local widgetNames = {}
    for wi = #widgets, 1, -1 do
        local widget = widgets[wi]
        if (widget:getId() ~= 'highlightWidget') then
            table.insert(widgetNames, widget:getClassName() .. '#' .. widget:getId())
        end
    end
    clientUiDebugLabel:setText(table.concat(widgetNames, " -> "))
end

function init()
    connect(rootWidget, {
        onMouseMove = onClientUiDebuggerMouseMove,
    })
    clientUiDebug = g_ui.displayUI('client_ui_debug')
    clientUiDebugLabel = clientUiDebug:getChildById('clientUiDebugLabel')
    clientUiDebugHighlightWidget = g_ui.createWidget('HighlightWidget', rootWidget)
end

function terminate()
    disconnect(rootWidget, {
        onMouseMove = onClientUiDebuggerMouseMove,
    })
    clientUiDebug:destroy()
    clientUiDebugHighlightWidget:destroy()
end