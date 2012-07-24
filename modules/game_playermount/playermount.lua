function init()
  if g_game.getFeature(GamePlayerMount) then
    g_keyboard.bindKeyDown('Ctrl+R', toggleMount, gameRootPanel)
  end
end

function terminate()
  if g_game.getFeature(GamePlayerMount) then
    g_keyboard.unbindKeyDown('Ctrl+R', gameRootPanel)
  end
end

function toggleMount()
  g_game.mount(not g_game.isMounted())
end

function dismount()
  g_game.mount(false)
end
