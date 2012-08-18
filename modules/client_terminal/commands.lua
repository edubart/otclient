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
        local text = [[
          id:]] ..item:getId() .. [[,
          stackable:]] ..tostring(item:isStackable()) .. [[,
          marketable:]] ..tostring(item:isMarketable()) .. [[,
          vocation:]]..(item:getMarketData() and item:getMarketData().restrictVocation or 'none') ..[[,
          cloth slot:]] ..item:getClothSlot() .. [[
        ]]
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

