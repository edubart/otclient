PlayerMount = {}

function PlayerMount.init()
  g_keyboard.bindKeyDown('Ctrl+R', PlayerMount.toggleMount, gameRootPanel)
end

function PlayerMount.terminate()
  g_keyboard.unbindKeyDown('Ctrl+R', gameRootPanel)

  PlayerMount = nil
end

function PlayerMount.toggleMount()
  if g_game.isMounted() then
    g_game.mount(false)
  else
    g_game.mount(true)
  end
end

function PlayerMount.dismount()
  g_game.mount(false)
end
