Client = { }

-- TODO: load and save configurations
function Client.init()
  -- initialize in fullscreen mode on mobile devices
  if g_window.getPlatformType() == "X11-EGL" then
    g_window:setFullscreen()
  else
    g_window.move({ x=220, y=220 })
    g_window.resize({ width=800, height=480 })
  end
  g_window.setTitle('OTClient')
  g_window.setIcon('clienticon.png')
  return true
end

function Client.terminate()
end
