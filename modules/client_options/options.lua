Options = {}

function Options.load()
  local booleanOptions = { vsync = true,
                           showfps = true,
                           fullscreen = false,
                           classicControl = false }

  for k,v in pairs(booleanOptions) do
    Settings.setDefault(k, v)
    Options.changeOption(k, Settings.getBoolean(k))
  end
end

function Options.show()
  displayUI('options.otui', { locked = true })
end

function Options.openWebpage()
  displayErrorBox("Error", "Not implemented yet")
end

-- private functions
function Options.changeOption(key, status)
  if key == 'vsync' then
    g_window.setVerticalSync(status)
  elseif key == 'showfps' then
    addEvent(function()
      local frameCounter = rootWidget:recursiveGetChildById('frameCounter')
      if frameCounter then frameCounter:setVisible(status) end
    end)
  elseif key == 'fullscreen' then
    addEvent(function()
      g_window.setFullscreen(status)
    end)
  end
  Settings.set(key, status)
  Options[key] = status
end
