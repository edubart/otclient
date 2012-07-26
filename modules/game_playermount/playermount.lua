function init()
  g_keyboard.bindKeyDown('Ctrl+R', toggleMount, gameRootPanel)
end

function terminate()
  g_keyboard.unbindKeyDown('Ctrl+R', gameRootPanel)
end

function toggleMount()
  if g_game.getFeature(GamePlayerMounts) then
    g_game.mount(not g_game.isMounted())
  end
end

function dismount()
  if g_game.getFeature(GamePlayerMounts) then
    g_game.mount(false)
  end
end
