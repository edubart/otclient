Client = {}

local musicFilename = "startup.ogg"

function Client.setMusic(filename)
  musicFilename = filename
  g_sounds.stopMusic(0)

  if not g_game.isOnline() then
    g_sounds.playMusic(musicFilename, 3)
  end
end

function Client.reloadScripts()
  g_modules.reloadModules()
  dofile '/otclientrc'
  local message = tr('All modules and scripts were reloaded.')

  modules.game_textmessage.displayGameMessage(message)
  print(message)
end

function Client.startup()
  -- Play startup music (The Silver Tree, by Mattias Westlund)
  g_sounds.playMusic(musicFilename, 3)
  connect(g_game, { onGameStart = function() g_sounds.stopMusic(3) end })
  connect(g_game, { onGameEnd = function() g_sounds.playMusic(musicFilename, 3) end })

  -- Check for startup errors
  local errtitle = nil
  local errmsg = nil

  if g_graphics.getRenderer():lower():match('gdi generic') then
    errtitle = tr('Graphics card driver not detected')
    errmsg = tr('No graphics card detected, everything will be drawn using the CPU,\nthus the performance will be really bad.\nPlease update your graphics driver to have a better performance.')
  end

  -- Show entergame
  if errmsg or errtitle then
    local msgbox = displayErrorBox(errtitle, errmsg)
    msgbox.onOk = function() EnterGame.firstShow() end
  else
    EnterGame.firstShow()
  end
end

function Client.init()
  g_window.setMinimumSize({ width = 600, height = 480 })
  g_sounds.preload(musicFilename)

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

  connect(g_app, { onRun = Client.startup })
end

function Client.terminate()
  -- save window configs
  g_settings.set('window-size', g_window.getUnmaximizedSize())
  g_settings.set('window-pos', g_window.getUnmaximizedPos())
  g_settings.set('window-maximized', g_window.isMaximized())

  local clientVersion = g_game.getClientVersion()
  if clientVersion ~= 0 then
    g_settings.set('client-version', clientVersion)
  end

  Client = nil
end
