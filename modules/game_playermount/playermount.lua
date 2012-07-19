PlayerMount = {}

function PlayerMount.init()
  if g_game.getFeature(GamePlayerMount) then
    g_keyboard.bindKeyDown('Ctrl+R', PlayerMount.toggleMount, gameRootPanel)
  end
end

function PlayerMount.terminate()
  if g_game.getFeature(GamePlayerMount) then
    g_keyboard.unbindKeyDown('Ctrl+R', gameRootPanel)
  end

  PlayerMount = nil
end

function PlayerMount.toggleMount()
  g_game.mount(not g_game.isMounted())
end

function PlayerMount.dismount()
  g_game.mount(false)
end
