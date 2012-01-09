function dumpWidgets()
  for i=1,rootWidget:getChildCount() do
    print(rootWidget:getChildByIndex(i):getId())
  end
end

function drawDebugBoxes(enable)
  if enable == nil then enable = true end
  g_ui.setDebugBoxesDrawing(enable)
end

function displayItem(id)
  local itemWidget = createWidget('Item', rootWidget)
  --itemWidget:setItem(Item.create(11703))
end

addEvent(function() displayItem(408) end)