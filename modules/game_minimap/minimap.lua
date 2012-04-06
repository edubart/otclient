Minimap = {}

-- private variables
local minimapWidget
local minimapButton

-- private functions
function onMinimapMouseRelease(self, mousePosition, mouseButton)
  local tile = self:getTile(mousePosition)
  if tile and mouseButton == MouseLeftButton and self:isPressed() then
    local dirs = g_map.findPath(g_game.getLocalPlayer():getPosition(), tile:getPosition(), 255)
    if #dirs == 0 then
      TextMessage.displayStatus('There is no way.')
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
  connect(g_game, { onLogin = Minimap.reset })
  Keyboard.bindKeyDown('Ctrl+M', Minimap.toggle)

  minimapButton = TopMenu.addGameToggleButton('minimapButton', 'Minimap (Ctrl+M)', 'minimap.png', Minimap.toggle)
  minimapButton:setOn(true)

  minimapWidget = loadUI('minimap.otui', GameInterface.getMapPanel())
  minimapWidget.onMouseRelease = onMinimapMouseRelease
  minimapWidget.onMouseWheel = onMinimapMouseWheel
end

function Minimap.terminate()
  Keyboard.unbindKeyDown('Ctrl+M')
  disconnect(g_game, { onLogin = Minimap.reset })

  minimapWidget:destroy()
  minimapWidget = nil
  minimapButton:destroy()
  minimapButton = nil
end

function Minimap.toggle()
  local visible = not minimapWidget:isExplicitlyVisible()
  minimapWidget:setVisible(visible)
  minimapButton:setOn(visible)
end

function Minimap.reset()
  minimapWidget:followCreature(g_game.getLocalPlayer())
  for i=1,10 do minimapWidget:zoomOut() end
end

