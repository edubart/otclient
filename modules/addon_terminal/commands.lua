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

function debugContainersItems()
  function UIItem:onHoverChange(hovered)
    if hovered then
      local item = self:getItem()
      if item then ToolTip.display(item:getId()) end
    else
      ToolTip.hide()
    end
  end
end
