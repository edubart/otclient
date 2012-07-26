DEFAULT_ZOOM = 60
MAX_FLOOR_UP = 0
MAX_FLOOR_DOWN = 15

G.minimapFirstLoad = true

navigating = false
minimapWidget = nil
minimapButton = nil
minimapWindow = nil

--[[
  Known Issue (TODO):
  If you move the minimap compass directions and
  you change floor it will not update the minimap.
]]
function init()
  connect(g_game, { onGameStart = reset,
                    onForceWalk = center })

  g_keyboard.bindKeyDown('Ctrl+M', toggle)

  minimapButton = TopMenu.addRightGameToggleButton('minimapButton', tr('Minimap') .. ' (Ctrl+M)', 'minimap.png', toggle)
  minimapButton:setOn(true)

  minimapWindow = g_ui.loadUI('minimap.otui', modules.game_interface.getRightPanel())

  minimapWidget = minimapWindow:recursiveGetChildById('minimap')
  g_mouse.bindAutoPress(minimapWidget, compassClick, nil, MouseRightButton)
  g_mouse.bindAutoPress(minimapWidget, compassClick, nil, MouseLeftButton)
  minimapWidget:setAutoViewMode(false)
  minimapWidget:setViewMode(1) -- mid view
  minimapWidget:setDrawMinimapColors(true)
  minimapWidget:setMultifloor(false)
  minimapWidget:setKeepAspectRatio(false)
  minimapWidget.onMouseRelease = onMinimapMouseRelease
  minimapWidget.onMouseWheel = onMinimapMouseWheel

  reset()

  -- load only the first time (avoid load/save between reloads)
  if G.minimapFirstLoad then
    G.minimapFirstLoad = false
    if g_resources.fileExists('/minimap.otcm') then
      if g_game.isOnline() then
        perror('cannot load minimap while online')
      else
        g_map.loadOtcm('/minimap.otcm')
      end
    end

    -- save only when closing the client
    connect(g_app, { onTerminate = function()
      g_map.saveOtcm('/minimap.otcm')
    end})
  end
end

function terminate()
  disconnect(g_game, { onGameStart = reset,
                       onForceWalk = center })

  g_keyboard.unbindKeyDown('Ctrl+M')

  minimapButton:destroy()
  minimapWindow:destroy()
end

function toggle()
  if minimapButton:isOn() then
    minimapWindow:close()
    minimapButton:setOn(false)
  else
    minimapWindow:open()
    minimapButton:setOn(true)
  end
end

function isClickInRange(position, fromPosition, toPosition)
  return (position.x >= fromPosition.x and position.y >= fromPosition.y and position.x <= toPosition.x and position.y <= toPosition.y)
end

function reset()
  local player = g_game.getLocalPlayer()
  if not player then return end
  minimapWidget:followCreature(player)
  minimapWidget:setZoom(DEFAULT_ZOOM)
end

function center()
  local player = g_game.getLocalPlayer()
  if not player then return end
  minimapWidget:followCreature(player)
end

function compassClick(self, mousePos, mouseButton, elapsed)
  if elapsed < 300 then return end

  navigating = true
  local px = mousePos.x - self:getX()
  local py = mousePos.y - self:getY()
  local dx = px - self:getWidth()/2
  local dy = -(py - self:getHeight()/2)
  local radius = math.sqrt(dx*dx+dy*dy)
  local movex = 0
  local movey = 0
  dx = dx/radius
  dy = dy/radius

  if dx > 0.5 then movex = 1 end
  if dx < -0.5 then movex = -1 end
  if dy > 0.5 then movey = -1 end
  if dy < -0.5 then movey = 1 end

  local cameraPos = minimapWidget:getCameraPosition()
  local pos = {x = cameraPos.x + movex, y = cameraPos.y + movey, z = cameraPos.z}
  minimapWidget:setCameraPosition(pos)
end

function onButtonClick(id)
  if id == "zoomIn" then
    minimapWidget:setZoom(math.max(minimapWidget:getMaxZoomIn(), minimapWidget:getZoom()-15))
  elseif id == "zoomOut" then
    minimapWidget:setZoom(math.min(minimapWidget:getMaxZoomOut(), minimapWidget:getZoom()+15))
  elseif id == "floorUp" then
    local pos = minimapWidget:getCameraPosition()
    pos.z = pos.z - 1
    if pos.z > MAX_FLOOR_UP then
      minimapWidget:setCameraPosition(pos)
    end
  elseif id == "floorDown" then
    local pos = minimapWidget:getCameraPosition()
    pos.z = pos.z + 1
    if pos.z < MAX_FLOOR_DOWN then
      minimapWidget:setCameraPosition(pos)
    end
  end
end

function onMinimapMouseRelease(self, mousePosition, mouseButton)
  if navigating then
    navigating = false
    return
  end
  local tile = self:getTile(mousePosition)
  if tile and mouseButton == MouseLeftButton and self:isPressed() then
    local dirs = g_map.findPath(g_game.getLocalPlayer():getPosition(), tile:getPosition(), 127)
    if #dirs == 0 then
      modules.game_textmessage.displayStatusMessage(tr('There is no way.'))
      return true
    end
    g_game.autoWalk(dirs)
    return true
  end
  return false
end

function onMinimapMouseWheel(self, mousePos, direction)
  if direction == MouseWheelUp then
    self:zoomIn()
  else
    self:zoomOut()
  end
end

function onMiniWindowClose()
  minimapButton:setOn(false)
end
