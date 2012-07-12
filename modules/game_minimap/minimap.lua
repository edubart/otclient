Minimap = {}

-- private variables
local minimapWidget
local minimapButton
local minimapWindow
local DEFAULT_ZOOM = 60
minimapFirstLoad = true

-- private functions
function onMinimapMouseRelease(self, mousePosition, mouseButton)
  local tile = self:getTile(mousePosition)
  if tile and mouseButton == MouseLeftButton and self:isPressed() then
    local dirs = g_map.findPath(g_game.getLocalPlayer():getPosition(), tile:getPosition(), 127)
    if #dirs == 0 then
      TextMessage.displayStatus(tr('There is no way.'))
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

--[[
  Known Issue (TODO):
  If you move the minimap compass directions and
  you change floor it will not update the minimap.
]]
-- public functions
function Minimap.init()
  connect(g_game, { onGameStart = Minimap.reset,
                    onForceWalk = Minimap.center })

  g_keyboard.bindKeyDown('Ctrl+M', Minimap.toggle)

  minimapButton = TopMenu.addRightGameToggleButton('minimapButton', tr('Minimap') .. ' (Ctrl+M)', 'minimap.png', Minimap.toggle)
  minimapButton:setOn(true)

  minimapWindow = g_ui.loadUI('minimap.otui', GameInterface.getRightPanel())

  minimapWidget = minimapWindow:recursiveGetChildById('minimap')
  minimapWidget:setAutoViewMode(false)
  minimapWidget:setViewMode(1) -- mid view
  minimapWidget:setDrawMinimapColors(true)
  minimapWidget:setMultifloor(false)
  minimapWidget:setKeepAspectRatio(false)
  minimapWidget.onMouseRelease = onMinimapMouseRelease
  minimapWidget.onMouseWheel = onMinimapMouseWheel

  Minimap.reset()

  -- load only the first time (avoid load/save between reloads)
  if minimapFirstLoad then
    minimapFirstLoad = false
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

function Minimap.terminate()
  disconnect(g_game, { onGameStart = Minimap.reset,
                       onForceWalk = Minimap.center })

  g_keyboard.unbindKeyDown('Ctrl+M')

  minimapButton:destroy()
  minimapWindow:destroy()
  minimapWindow = nil
  minimapWidget = nil
  minimapButton = nil
  Minimap = nil
end

function Minimap.toggle()
  if minimapButton:isOn() then
    minimapWindow:close()
    minimapButton:setOn(false)
  else
    minimapWindow:open()
    minimapButton:setOn(true)
  end
end

function Minimap.onMiniWindowClose()
  minimapButton:setOn(false)
end

function Minimap.isClickInRange(position, fromPosition, toPosition)
  return (position.x >= fromPosition.x and position.y >= fromPosition.y and position.x <= toPosition.x and position.y <= toPosition.y)
end

-- hooked functions
function Minimap.compassClick(self, mousePos)
  local px = mousePos.x - self:getX()
  local py = mousePos.y - self:getY()
  local dx = px - self:getWidth()/2
  local dy = -(py - self:getHeight()/2)
  local radius = math.sqrt(dx*dx+dy*dy)
  local movex=0
  local movey=0
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

function Minimap.onButtonClick(id)
  if id == "zoomIn" then
    minimapWidget:setZoom(math.max(minimapWidget:getMaxZoomIn(), minimapWidget:getZoom()-15))
  elseif id == "zoomOut" then
    minimapWidget:setZoom(math.min(minimapWidget:getMaxZoomOut(), minimapWidget:getZoom()+15))
  elseif id == "floorUp" then
    local pos = minimapWidget:getCameraPosition()
    pos.z = pos.z - 1
    minimapWidget:setCameraPosition(pos)
  elseif id == "floorDown" then
    local pos = minimapWidget:getCameraPosition()
    pos.z = pos.z + 1
    minimapWidget:setCameraPosition(pos)
  end
end

-- hooked events
function Minimap.reset()
  local player = g_game.getLocalPlayer()
  if not player then return end
  minimapWidget:followCreature(player)
  minimapWidget:setZoom(DEFAULT_ZOOM)
end

function Minimap.center()
  local player = g_game.getLocalPlayer()
  if not player then return end
  minimapWidget:followCreature(player)
end