Options = {}

local optionsWindow
local optionsButton

function Options.init()
  -- load settings
  local booleanOptions = { vsync = true,
                           showfps = true,
                           fullscreen = false,
                           classicControl = false,
                           showStatusMessagesInConsole = true,
                           showEventMessagesInConsole = true,
                           showInfoMessagesInConsole = true,
                           showTimestampsInConsole = true,
                           showLevelsInConsole = true,
                           showPrivateMessagesInConsole = true,
                           }

  for k,v in pairs(booleanOptions) do
    Settings.setDefault(k, v)
    Options.changeOption(k, Settings.getBoolean(k))
  end

  optionsWindow = displayUI('options.otui')
  optionsWindow:setVisible(false)
  optionsButton = TopMenu.addButton('optionsButton', 'Options (Ctrl+O)', 'options.png', Options.toggle)
  Hotkeys.bindKeyDown('Ctrl+O', Options.toggle)
end

function Options.terminate()
  Hotkeys.unbindKeyDown('Ctrl+O')
  optionsWindow:destroy()
  optionsWindow = nil
  optionsButton:destroy()
  optionsButton = nil
end

function Options.toggle()
  if optionsWindow:isVisible() then
    Options.hide()
  else
    Options.show()
  end
end

function Options.show()
  optionsWindow:show()
  optionsWindow:lock()
end

function Options.hide()
  optionsWindow:unlock()
  optionsWindow:hide()
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
