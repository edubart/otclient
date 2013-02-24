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
      if item then
        local text = "id: " ..item:getId() .. 
                     "\n stackable: " ..tostring(item:isStackable()) ..
                     "\n marketable: " ..tostring(item:isMarketable()) ..
                     "\n vocation: "..(item:getMarketData() and item:getMarketData().restrictVocation or 'none') ..
                     "\n cloth slot: " ..item:getClothSlot()
        g_tooltip.display(text)
      end
    else
      g_tooltip.hide()
    end
  end
end

function debugPosition(enable)
  if enable == nil then enable = true end
  local label = rootWidget:getChildById('debugPositionLabel')
  if not label then
    label = g_ui.createWidget('GameLabel', rootWidget)
    label:setColor('pink')
    label:setFont('terminus-14px-bold')
    label:setId('debugPositionLabel')
    label:setPosition({x= 10, y = 40 })
    label:setPhantom(true)
    label:setTextAutoResize(true)
  end
  if enable then
    label.event = cycleEvent(function()
      local player = g_game.getLocalPlayer()
      if player then
        local pos = g_game.getLocalPlayer():getPosition()
        label:show()
        label:setText('x: ' .. pos.x .. '\ny: ' .. pos.y .. '\nz: ' .. pos.z)
      else
        label:hide()
      end
    end, 100)
  else
    removeEvent(label.event)
    label.event = nil
    label:hide()
  end
end

function autoReloadModule(name)
  local function reloadEvent()
    reloadModule(name)
    scheduleEvent(reloadEvent, 1000)
  end
  reloadEvent()
end

function createDebugUIItem(id)
  local uiitem = g_ui.createWidget('Item', rootWidget)
  uiitem:setPosition({x= 200, y = 200 })
  uiitem:setItemId(id)
  uiitem:show()
end

function debugPings()
  g_game.setPingDelay(0)
  connect(g_game, { onPingBack = function(ping) print(g_game.getWorldName() .. ' => ' .. ping .. ' ms') end })
end
