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
    local dirs = g_map.findPath(g_game.getLocalPlayer():getPosition(), tile:getPosition(), 255)
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

-- public functions
function Minimap.init()
  connect(g_game, { onGameStart = Minimap.reset })
  Keyboard.bindKeyDown('Ctrl+M', Minimap.toggle)

  minimapButton = TopMenu.addGameToggleButton('minimapButton', tr('Minimap') .. ' (Ctrl+M)', 'minimap.png', Minimap.toggle)
  minimapButton:setOn(true)

  minimapWindow = loadUI('minimap.otui', GameInterface.getRightPanel())

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
  disconnect(g_game, { onGameStart = Minimap.reset })
  Keyboard.unbindKeyDown('Ctrl+M')

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

