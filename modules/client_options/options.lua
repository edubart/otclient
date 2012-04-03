Options = {}

local optionsWindow
local optionsButton
local optionsTabBar
local options = { vsync = true,
                  showfps = true,
                  fullscreen = false,
                  classicControl = false,
                  showStatusMessagesInConsole = true,
                  showEventMessagesInConsole = true,
                  showInfoMessagesInConsole = true,
                  showTimestampsInConsole = true,
                  showLevelsInConsole = true,
                  showPrivateMessagesInConsole = false }

function Options.init()
  -- load options
  for k,v in pairs(options) do
    if type(v) == 'boolean' then
      Settings.setDefault(k, v)
      Options.setOption(k, Settings.getBoolean(k))
    end
  end

  Keyboard.bindKeyDown('Ctrl+P', Options.toggle)

  optionsWindow = displayUI('options.otui')
  optionsWindow:hide()
  optionsButton = TopMenu.addLeftButton('optionsButton', 'Options (Ctrl+O)', 'options.png', Options.toggle)
  optionsTabBar = optionsWindow:getChildById('optionsTabBar')
  optionsTabBar:setContentWidget(optionsWindow:getChildById('optionsTabContent'))
  optionsTabBar:addTab('General', loadUI('general.otui'))
  optionsTabBar:addTab('Graphics', loadUI('graphics.otui'))
end

function Options.terminate()
  Keyboard.unbindKeyDown('Ctrl+P')
  optionsWindow:destroy()
  optionsWindow = nil
  optionsButton:destroy()
  optionsButton = nil
  optionsTabBar = nil
  Options = nil
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
  optionsWindow:raise()
  optionsWindow:focus()
end

function Options.hide()
  optionsWindow:hide()
end

function Options.openWebpage()
  displayErrorBox("Error", "Not implemented yet")
end

function Options.setOption(key, value)
  if key == 'vsync' then
    g_window.setVerticalSync(value)
  elseif key == 'showfps' then
    addEvent(function()
      local frameCounter = rootWidget:recursiveGetChildById('frameCounter')
      if frameCounter then frameCounter:setVisible(value) end
    end)
  elseif key == 'fullscreen' then
    addEvent(function()
      g_window.setFullscreen(value)
    end)
  end
  Settings.set(key, value)
  options[key] = value
end

function Options.getOption(key)
  return options[key]
end

