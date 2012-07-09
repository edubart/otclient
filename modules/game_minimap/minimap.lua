Minimap = {}

-- private variables
local minimapWidget
local minimapButton
local minimapWindow
local DEFAULT_ZOOM = 45
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
                    onForceWalk = Minimap.center } )
                    
  g_keyboard.bindKeyDown('Ctrl+M', Minimap.toggle)

  minimapButton = TopMenu.addGameToggleButton('minimapButton', tr('Minimap') .. ' (Ctrl+M)', 'minimap.png', Minimap.toggle)
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
                       onForceWalk = Minimap.center } )
                       
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

function Minimap.isClickInRange(position, fromPosition, toPosition)
	return (position.x >= fromPosition.x and position.y >= fromPosition.y and position.x <= toPosition.x and position.y <= toPosition.y)
end
-- hooked functions
local compassZones = {}
compassZones.west = {x = 0, y = 30, posx = -1, posy = 0}
compassZones.north = {x = 30, y = 0, posx = 0, posy = -1}
compassZones.south = {x = 30, y = 57, posx = 0, posy = 1}
compassZones.east = {x = 57, y = 30, posx = 1, posy = 0}
function Minimap.compassClick(self, mousePos)
  local compassPos = self:getRect()
  local pos = {x = mousePos.x-compassPos.x, y = mousePos.y-compassPos.y}
  local move = {x = 0, y = 0}
  for i,v in pairs(compassZones) do
    local lowPos = {x = v.x-15, y = v.y-15}
    local highPos = {x = v.x+15, y = v.y+15}
    if Minimap.isClickInRange(pos, lowPos, highPos) then
      move.x = move.x + v.posx * minimapWidget:getZoom()/10
      move.y = move.y + v.posy * minimapWidget:getZoom()/10
      break
    end
  end

  local cameraPos = minimapWidget:getCameraPosition()
  local pos = {x = cameraPos.x + move.x, y = cameraPos.y + move.y, z = cameraPos.z}
  minimapWidget:setCameraPosition(pos)
end

function Minimap.onButtonClick(id)
  if id == "zoomIn" then
    minimapWidget:setZoom(math.max(minimapWidget:getMaxZoomIn(), minimapWidget:getZoom()-15))
  elseif id == "zoomOut" then
    minimapWidget:setZoom(math.min(minimapWidget:getMaxZoomOut(), minimapWidget:getZoom()+15))
  elseif id == "levelUp" then
    local pos = minimapWidget:getCameraPosition()
    pos.z = pos.z - 1
    minimapWidget:setCameraPosition(pos)
  elseif id == "levelDown" then
    local pos = minimapWidget:getCameraPosition()
    pos.z = pos.z + 1
    minimapWidget:setCameraPosition(pos)
  end
end