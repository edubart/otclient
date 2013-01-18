local defaultOptions = {
  vsync = false,
  showFps = true,
  showPing = true,
  fullscreen = false,
  dontStretchShrink = false,
  classicControl = false,
  walkBooster = false,
  smartWalk = false,
  autoChaseOverride = true,
  showStatusMessagesInConsole = true,
  showEventMessagesInConsole = true,
  showInfoMessagesInConsole = true,
  showTimestampsInConsole = true,
  showLevelsInConsole = true,
  showPrivateMessagesInConsole = false,
  showPrivateMessagesOnScreen = true,
  enableMusic = true,
  showLeftPanel = false,
  foregroundFrameRate = 61,
  backgroundFrameRate = 201,
  ambientLight = 15,
  painterEngine = 0
}

local warningWindow
local optionsWindow
local optionsButton
local optionsTabBar
local options = {}
local gamePanel
local consolePanel
local graphicsPanel

local function setupGraphicsEngines()
  local enginesRadioGroup = UIRadioGroup.create()
  local ogl1 = graphicsPanel:getChildById('opengl1')
  local ogl2 = graphicsPanel:getChildById('opengl2')
  enginesRadioGroup:addWidget(ogl1)
  enginesRadioGroup:addWidget(ogl2)

  if g_graphics.getPainterEngine() == 2 then
    enginesRadioGroup:selectWidget(ogl2)
  else
    enginesRadioGroup:selectWidget(ogl1)
  end

  ogl1:setEnabled(g_graphics.isPainterEngineAvailable(1))
  ogl2:setEnabled(g_graphics.isPainterEngineAvailable(2))

  enginesRadioGroup.onSelectionChange = function(self, selected)
    if selected == ogl1 then
      setOption('painterEngine', 1)
    elseif selected == ogl2 then
      setOption('painterEngine', 2)
    end
  end

  if not g_graphics.canCacheBackbuffer() then
    graphicsPanel:getChildById('foregroundFrameRate'):disable()
    graphicsPanel:getChildById('foregroundFrameRateLabel'):disable()
  end
end

function displayWarning(widget, warning)
  if warningWindow and warningWindow:isVisible() then
    return
  end
  if widget:isChecked() then
    local yesCallback = function() warningWindow:destroy() warningWindow=nil end
    local noCallback = function() widget:setChecked(false) warningWindow:destroy() warningWindow=nil end

    warningWindow = displayGeneralBox('Warning', tr(warning), {
      { text='Yes', callback=yesCallback },
      { text='No', callback=noCallback },
      anchor=AnchorHorizontalCenter}, yesCallback, noCallback)
  end
end

function init()
  -- load options
  for k,v in pairs(defaultOptions) do
    g_settings.setDefault(k, v)
    if type(v) == 'boolean' then
      setOption(k, g_settings.getBoolean(k))
    elseif type(v) == 'number' then
      setOption(k, g_settings.getNumber(k))
    end
  end

  g_keyboard.bindKeyDown('Ctrl+D', function() toggle() end)
  g_keyboard.bindKeyDown('Ctrl+F', function() toggleOption('fullscreen') end)
  g_keyboard.bindKeyDown('Ctrl+Shift+D', function() toggleOption('walkBooster') end)

  optionsWindow = g_ui.displayUI('options')
  optionsWindow:hide()
  optionsButton = modules.client_topmenu.addLeftButton('optionsButton', tr('Options') .. ' (Ctrl+D)', '/images/topbuttons/options', toggle)

  optionsTabBar = optionsWindow:getChildById('optionsTabBar')
  optionsTabBar:setContentWidget(optionsWindow:getChildById('optionsTabContent'))

  gamePanel = g_ui.loadUI('game')
  optionsTabBar:addTab(tr('Game'), gamePanel)

  consolePanel = g_ui.loadUI('console')
  optionsTabBar:addTab(tr('Console'), consolePanel)

  graphicsPanel = g_ui.loadUI('graphics')
  optionsTabBar:addTab(tr('Graphics'), graphicsPanel)

  if g_game.isOfficialTibia() then
    local optionWalkBooster = gamePanel:getChildById('walkBooster')
    optionWalkBooster.onCheckChange = function(widget)
      displayWarning(widget, "This feature could be detectable by official Tibia servers. Would like to continue?")
      setOption(widget:getId(), widget:isChecked())
    end
  end

  setupGraphicsEngines()
end

function terminate()
  g_keyboard.unbindKeyDown('Ctrl+D')
  g_keyboard.unbindKeyDown('Ctrl+F')
  g_keyboard.unbindKeyDown('Ctrl+Shift+D')
  optionsWindow:destroy()
  optionsWindow = nil
  optionsButton:destroy()
  optionsButton = nil
  optionsTabBar = nil
  gamePanel = nil
  consolePanel = nil
  graphicsPanel = nil
end

function toggle()
  if optionsWindow:isVisible() then
    hide()
  else
    show()
  end
end

function show()
  optionsWindow:show()
  optionsWindow:raise()
  optionsWindow:focus()
end

function hide()
  optionsWindow:hide()
end

function toggleOption(key)
  local optionWidget = optionsWindow:recursiveGetChildById(key)
  if optionWidget then
    optionWidget:setChecked(not getOption(key))
  else
    setOption(key, not getOption(key))
  end
end

function setOption(key, value)
  if options[key] == value then return end
  if key == 'vsync' then
    g_window.setVerticalSync(value)
  elseif key == 'showFps' then
    addEvent(function()
      local frameCounter = rootWidget:recursiveGetChildById('frameCounter')
      if frameCounter then frameCounter:setVisible(value) end
    end)
  elseif key == 'showPing' then
    addEvent(function()
      local ping = rootWidget:recursiveGetChildById('pingLabel')
      if ping then ping:setVisible(value) end
    end)
  elseif key == 'fullscreen' then
    g_window.setFullscreen(value)
  elseif key == 'dontStretchShrink' then
    addEvent(function()
      modules.game_interface.updateStretchShrink()
    end)
  elseif key == 'enableMusic' then
    g_sounds.setAudioEnabled(value)
  elseif key == 'showLeftPanel' then
    addEvent(function()
      modules.game_interface.getLeftPanel():setOn(value)
    end)
  elseif key == 'backgroundFrameRate' then
    local text = value
    if value <= 0 or value >= 201 then
      text = 'max'
      value = 0
    end

    if graphicsPanel then
      graphicsPanel:getChildById('backgroundFrameRateLabel'):setText(tr('Game framerate limit: %s', text))
    end
    g_app.setBackgroundPaneMaxFps(value)
  elseif key == 'foregroundFrameRate' then
    local text = value
    if value <= 0 or value >= 61 then
      text = 'max'
      value = 0
    end

    if graphicsPanel then
      graphicsPanel:getChildById('foregroundFrameRateLabel'):setText(tr('Interface framerate limit: %s', text))
    end
    g_app.setForegroundPaneMaxFps(value)
  elseif key == 'ambientLight' then
    addEvent(function()
      local map = rootWidget:recursiveGetChildById('gameMapPanel')
      if graphicsPanel then
        graphicsPanel:getChildById('ambientLightLabel'):setText(tr('Ambient light: %s%%', value))
      end
      if map then
        map:setMinimumAmbientLight(value/100)
        map:setDrawLights(value < 100)
      end
    end)
  elseif key == 'painterEngine' then
    g_graphics.selectPainterEngine(value)
  end
  g_settings.set(key, value)
  options[key] = value
end

function getOption(key)
  return options[key]
end


