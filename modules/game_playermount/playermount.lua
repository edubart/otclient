function init()
  connect(g_game, {
    onGameStart = online,
    onGameEnd = offline
  })
  if g_game.isOnline() then online() end
end

function terminate()
  disconnect(g_game, {
    onGameStart = online,
    onGameEnd = offline
  })
  offline()
end

function online()
  if g_game.getFeature(GamePlayerMounts) then
    g_keyboard.bindKeyDown('Ctrl+R', toggleMount)
  end
end

function offline()
  if g_game.getFeature(GamePlayerMounts) then
    g_keyboard.unbindKeyDown('Ctrl+R')
  end
end

function toggleMount()
  local player = g_game.getLocalPlayer()
  if player then
    player:toggleMount()
  end
end

function mount()
  local player = g_game.getLocalPlayer()
  if player then
    player:mount()
  end
end

function dismount()
  local player = g_game.getLocalPlayer()
  if player then
    player:dismount()
  end
end
