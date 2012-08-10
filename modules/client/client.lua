Client = {}

function Client.reloadScripts()
  g_modules.reloadModules()
  dofile '/otclientrc'
  local message = tr('All modules and scripts were reloaded.')

  modules.game_textmessage.displayGameMessage(message)
  print(message)
end

function Client.init()
  g_window.setMinimumSize({ width = 600, height = 480 })

  -- initialize in fullscreen mode on mobile devices
  if g_window.getPlatformType() == "X11-EGL" then
    g_window.setFullscreen(true)
  else
    -- window size
    local size = { width = 800, height = 600 }
    size = g_settings.getSize('window-size', size)
    g_window.resize(size)

    -- window position, default is the screen center
    local displaySize = g_window.getDisplaySize()
    local defaultPos = { x = (displaySize.width - size.width)/2,
                         y = (displaySize.height - size.height)/2 }
    local pos = g_settings.getPoint('window-pos', defaultPos)
    g_window.move(pos)

    -- window maximized?
    local maximized = g_settings.getBoolean('window-maximized', false)
    if maximized then g_window.maximize() end
  end

  g_window.setTitle('OTClient')
  g_window.setIcon(resolvepath('clienticon.png'))
  g_keyboard.bindKeyDown('Ctrl+Shift+R', Client.reloadScripts)

  local clientVersion = g_settings.getInteger('client-version')
  if clientVersion ~= 0 then
    g_game.setClientVersion(clientVersion)
  end

  connect(g_app, { onRun =
    function()
      -- Play startup music (The Silver Tree, by Mattias Westlund)
      g_sounds.playMusic("startup.ogg", 3)
      connect(g_game, { onGameStart = function() g_sounds.stopMusic(3) end })
      connect(g_game, { onGameEnd = function() g_sounds.playMusic("startup.ogg", 3) end })
    end
  })

end

function Client.terminate()
  -- save window configs
  g_settings.set('window-size', g_window.getUnmaximizedSize())
  g_settings.set('window-pos', g_window.getUnmaximizedPos())
  g_settings.set('window-maximized', g_window.isMaximized())
  g_settings.set('client-version', g_game.getClientVersion())
  Client = nil
end
