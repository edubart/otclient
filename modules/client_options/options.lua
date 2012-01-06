Options = {}

-- public functions
function Options.load()
  -- set default settings
  Settings.setDefault('vsync', true)
  Settings.setDefault('showfps', true)

  -- load the options
  Options.enableVsync(Settings.getBoolean('vsync'))
  Options.enableFps(Settings.getBoolean('showfps'))
end

function Options.show()
  displayUI('options.otui', { locked = true })
end

function Options.openWebpage()
  displayErrorBox("Error", "Not implemented yet")
end

-- private functions
function Options.enableVsync(on)
  g_window.setVerticalSync(on)
  Settings.set('vsync', on)
  Options.vsync = on
end

function Options.enableFps(on)
  local frameCounter = rootWidget:recursiveGetChildById('frameCounter')
  frameCounter:setVisible(on)
  Settings.set('showfps', on)
  Options.fps = on
end
