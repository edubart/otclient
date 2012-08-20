function init()
  g_keyboard.bindKeyDown('Ctrl+R', toggleMount, gameRootPanel)
end

function terminate()
  g_keyboard.unbindKeyDown('Ctrl+R', gameRootPanel)
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
