PlayerMount = {}

-- private variables

-- private functions

-- public functions
function PlayerMount.init()
  g_ui.importStyle('playermount.otui')
  
  connect(g_game, { onDeath = PlayerMount.dismount,
                    onGameEnd = PlayerMount.dismount })
  
  g_keyboard.bindKeyDown('Ctrl+R', PlayerMount.toggleMount, gameRootPanel)
end

function PlayerMount.terminate()
  disconnect(g_game, { onDeath = PlayerMount.dismount,
                       onGameEnd = PlayerMount.dismount })

  g_keyboard.unbindKeyDown('Ctrl+R', PlayerMount.toggleMount, gameRootPanel)
  PlayerMount.reset()
  
  PlayerMount = nil
end

-- hooked events
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