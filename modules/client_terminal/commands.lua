function dumpWidgets(widget, level)
  widget = widget or rootWidget
  level = level or 0
  for i=1,widget:getChildCount() do
    local child = widget:getChildByIndex(i)
    if child:isVisible() then
      local name = child:getId()
      if name:match('widget%d+') == nil then
        print(string.rep(' ', level) .. name)
      end
      if child:getId() ~= 'terminalBuffer' then
        dumpWidgets(child, level+1)
      end
    end
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
      if item then g_tooltip.display(item:getId()) end
    else
      g_tooltip.hide()
    end
  end
end

function autoReloadModule(name)
  local function reloadEvent()
    reloadModule(name)
    scheduleEvent(reloadEvent, 1000)
  end
  reloadEvent()
end

