Background = { }

-- private variables
local background

-- public functions
function Background.init()
  background = displayUI('background.otui')
  background:lower()

  local clientVersionLabel = background:getChildById('clientVersionLabel')
  clientVersionLabel:setText('OTClient ' .. g_app.getVersion() .. '\n' ..
                             'Rev  ' .. g_app.getBuildRevision() .. '\n' ..
                             'Protocol  ' .. g_game.getProtocolVersion() .. '\n' ..
                             'Built on ' .. g_app.getBuildDate())

  if not g_game.isOnline() then
    Effects.fadeIn(clientVersionLabel, 1500)
  end

  connect(g_game, { onGameStart = Background.hide })
  connect(g_game, { onGameEnd = Background.show })
end

function Background.terminate()
  disconnect(g_game, { onGameStart = Background.hide })
  disconnect(g_game, { onGameEnd = Background.show })

  Effects.cancelFade(background:getChildById('clientVersionLabel'))
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
