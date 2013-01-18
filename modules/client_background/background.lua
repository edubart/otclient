-- private variables
local background

-- public functions
function init()
  background = g_ui.displayUI('background')
  background:lower()

  local clientVersionLabel = background:getChildById('clientVersionLabel')
  clientVersionLabel:setText(g_app.getName() .. ' ' .. g_app.getVersion() .. '\n' ..
                             'Rev  ' .. g_app.getBuildRevision() .. ' ('.. g_app.getBuildCommit() .. ')\n' ..
                             'Built on ' .. g_app.getBuildDate())

  if not g_game.isOnline() then
    g_effects.fadeIn(clientVersionLabel, 1500)
  end

  connect(g_game, { onGameStart = hide })
  connect(g_game, { onGameEnd = show })
end

function terminate()
  disconnect(g_game, { onGameStart = hide })
  disconnect(g_game, { onGameEnd = show })

  g_effects.cancelFade(background:getChildById('clientVersionLabel'))
  background:destroy()
  background = nil

  Background = nil
end

function hide()
  background:hide()
end

function show()
  background:show()
end

function hideVersionLabel()
  background:getChildById('clientVersionLabel'):hide()
end
