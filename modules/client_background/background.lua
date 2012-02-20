Background = { }

-- private variables
local background

-- public functions
function Background.init()
  background = displayUI('background.otui')
  background:lower()

  connect(g_game, { onGameStart = Background.hide })
  connect(g_game, { onGameEnd = Background.show })
end

function Background.terminate()
  disconnect(g_game, { onGameStart = Background.hide })
  disconnect(g_game, { onGameEnd = Background.show })

  background:destroy()
  background = nil

  Background = nil
end

function Background.hide()
  background:hide()
end

function Background.show()
  background:show()
end
