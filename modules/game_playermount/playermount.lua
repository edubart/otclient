PlayerMount = {}

function PlayerMount.init()
  g_keyboard.bindKeyDown('Ctrl+R', PlayerMount.toggleMount, gameRootPanel)
end

function PlayerMount.terminate()
  g_keyboard.unbindKeyDown('Ctrl+R', gameRootPanel)

  PlayerMount = nil
end

function PlayerMount.toggleMount()
  g_game.mount(not g_game.isMounted())
end

function PlayerMount.dismount()
  g_game.mount(false)
end
