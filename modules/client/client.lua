Client = { }

-- TODO: load and save configurations
function Client.init()
  g_window.move({ x=220, y=220 })
  g_window.resize({ width=800, height=600 })
  g_window.setTitle('OTClient')
  g_window.setIcon('clienticon.png')
  return true
end

function Client.terminate()
end
