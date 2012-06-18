Client = {}

function Client.reloadScripts()
  dofile '/otclientrc'
  reloadModules()
  local message = tr('All modules and scripts were reloaded.')
  TextMessage.displayEventAdvance(message)
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
    size = Settings.getSize('window-size', size)
    g_window.resize(size)

    -- window position, default is the screen center
    local displaySize = g_window.getDisplaySize()
    local defaultPos = { x = (displaySize.width - size.width)/2,
                         y = (displaySize.height - size.height)/2 }
    local pos = Settings.getPoint('window-pos', defaultPos)
    g_window.move(pos)

    -- window maximized?
    local maximized = Settings.getBoolean('window-maximized', false)
    if maximized then g_window.maximize() end
  end

  g_window.setTitle('OTClient')
  g_window.setIcon(resolvepath('clienticon.png'))

  -- show the only window after the first frame is rendered
  scheduleEvent(function()
    scheduleEvent(function()
        g_window.show()

        -- Play startup music (The Silver Tree, by Mattias Westlund)
        g_sounds.playMusic("startup.ogg", 3)
        connect(g_game, { onGameStart = function() g_sounds.stopMusic(3) end })
        connect(g_game, { onGameEnd= function() g_sounds.playMusic("startup.ogg", 3) end })
    end, 0)
  end, 0)
end

function Client.terminate()
  -- save window configs
  Settings.set('window-size', g_window.getUnmaximizedSize())
  Settings.set('window-pos', g_window.getUnmaximizedPos())
  Settings.set('window-maximized', g_window.isMaximized())
  Client = nil
end
