Client = {}

-- TODO: load and save configurations
function Client.init()
  g_window.show()
  g_window.setMinimumSize({ width = 550, height = 450 })

  -- initialize in fullscreen mode on mobile devices
  if g_window.getPlatformType() == "X11-EGL" then
    g_window.setFullscreen(true)
  else
    local size = { width = 1024,
                   height = 768 }
    g_window.resize(size)

    local displaySize = g_window.getDisplaySize()
    local pos = { x = (displaySize.width - size.width)/2,
                  y = (displaySize.height - size.height)/2 }
    g_window.move(pos)
  end

  g_window.setTitle('OTClient')
  g_window.setIcon(resolvepath('clienticon.png'))

  if not g_sprites.isLoaded() or not g_thingsType.isLoaded() then
    fatal("spr and dat files are not loaded, did you loaded them?")
  end
end

function Client.terminate()
end
