function dumpWidgets()
  for i=1,rootWidget:getChildCount() do
    print(rootWidget:getChildByIndex(i):getId())
  end
end

function drawDebugBoxes(enable)
  if enable == nil then enable = true end
  g_ui.setDebugBoxesDrawing(enable)
end

function hideMap()
  local map = rootWidget:recursiveGetChildById('gameMapPanel')
  if map then map:hide() end
end

function showMap()
  local map = rootWidget:recursiveGetChildById('gameMapPanel')
  if map then map:show() end
end

function displayItem(id)
  local itemWidget = createWidget('Item', rootWidget)
  itemWidget:setItem(Item.create(id))
end

