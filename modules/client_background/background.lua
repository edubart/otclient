Background = { }

-- private variables
local background

-- public functions
function Background.init()
  background = g_ui.displayUI('background.otui')
  background:lower()

  local clientVersionLabel = background:getChildById('clientVersionLabel')
  clientVersionLabel:setText('OTClient ' .. g_app.getVersion() .. '\n' ..
                             'Rev  ' .. g_app.getBuildRevision() .. ' ('.. g_app.getBuildCommit() .. ')\n' ..
                             'Built on ' .. g_app.getBuildDate())

  if not g_game.isOnline() then
    g_effects.fadeIn(clientVersionLabel, 1500)
  end

  connect(g_game, { onGameStart = Background.hide })
  connect(g_game, { onGameEnd = Background.show })
end

function Background.terminate()
  disconnect(g_game, { onGameStart = Background.hide })
  disconnect(g_game, { onGameEnd = Background.show })

  g_effects.cancelFade(background:getChildById('clientVersionLabel'))
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

function Background.hideVersionLabel()
  background:getChildById('clientVersionLabel'):hide()
end
