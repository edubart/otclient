local defaultOptions = {
  vsync = false,
  showFps = false,
  showPing = false,
  fullscreen = false,
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
  showLeftPanel = false,
  foregroundFrameRate = 61,
  backgroundFrameRate = 201,
  painterEngine = 0,
  enableAudio = true,
  enableMusicSound = true,
  enableShaders = true,
  musicSoundVolume = 100,
  enableLights = true,
  enableShaders = true,
  ambientLight = 25,
}

local optionsWindow
local optionsButton
local optionsTabBar
local options = {}
local generalPanel
local consolePanel
local graphicsPanel
local soundPanel
local audioButton

local function setupGraphicsEngines()
  local enginesRadioGroup = UIRadioGroup.create()
  local ogl1 = graphicsPanel:getChildById('opengl1')
  local ogl2 = graphicsPanel:getChildById('opengl2')
  local dx9 = graphicsPanel:getChildById('directx9')
  enginesRadioGroup:addWidget(ogl1)
  enginesRadioGroup:addWidget(ogl2)
  enginesRadioGroup:addWidget(dx9)

  if g_window.getPlatformType() == 'WIN32-EGL' then
    enginesRadioGroup:selectWidget(dx9)
    ogl1:setEnabled(false)
    ogl2:setEnabled(false)
    dx9:setEnabled(true)
  else
    ogl1:setEnabled(g_graphics.isPainterEngineAvailable(1))
    ogl2:setEnabled(g_graphics.isPainterEngineAvailable(2))
    dx9:setEnabled(false)
    if g_graphics.getPainterEngine() == 2 then
      enginesRadioGroup:selectWidget(ogl2)
    else
      enginesRadioGroup:selectWidget(ogl1)
    end

    if g_app.getOs() ~= 'windows' then
      dx9:hide()
    end
  end

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

  local shadersBox = graphicsPanel:getChildById('enableShaders')
  shadersBox:setEnabled(g_graphics.getPainterEngine() == 2)
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

  g_keyboard.bindKeyDown('Ctrl+Shift+F', function() toggleOption('fullscreen') end)
  --g_keyboard.bindKeyDown('Ctrl+D', function() toggleOption('walkBooster') end)

  optionsWindow = g_ui.displayUI('options')
  optionsWindow:hide()
  optionsButton = modules.client_topmenu.addLeftButton('optionsButton', tr('Options'), '/images/topbuttons/options', toggle)

  optionsTabBar = optionsWindow:getChildById('optionsTabBar')
  optionsTabBar:setContentWidget(optionsWindow:getChildById('optionsTabContent'))

  generalPanel = g_ui.loadUI('game')
  optionsTabBar:addTab(tr('Game'), generalPanel, '/images/optionstab/game')

  consolePanel = g_ui.loadUI('console')
  optionsTabBar:addTab(tr('Console'), consolePanel, '/images/optionstab/console')

  graphicsPanel = g_ui.loadUI('graphics')
  optionsTabBar:addTab(tr('Graphics'), graphicsPanel, '/images/optionstab/graphics')

  audioPanel = g_ui.loadUI('audio')
  optionsTabBar:addTab(tr('Audio'), audioPanel, '/images/optionstab/audio')

  audioButton = modules.client_topmenu.addLeftButton('audioButton', tr('Audio'), '/images/topbuttons/audio', function() toggleOption('enableAudio') end)

  setupGraphicsEngines()
end

function terminate()
  --g_keyboard.unbindKeyDown('Ctrl+D')
  g_keyboard.unbindKeyDown('Ctrl+Shift+F')
  optionsWindow:destroy()
  optionsWindow = nil
  optionsButton:destroy()
  optionsButton = nil
  optionsTabBar = nil
  generalPanel = nil
  consolePanel = nil
  graphicsPanel = nil
  audioPanel = nil
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
  setOption(key, not getOption(key))
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
  elseif key == 'enableAudio' then
    g_sounds.setAudioEnabled(value)
    addEvent(function()
      if value then
        audioButton:setIcon('/images/topbuttons/audio')
      else
        audioButton:setIcon('/images/topbuttons/audio_mute')
      end
    end)
  elseif key == 'enableMusicSound' then
    g_sounds.getChannel(SoundChannels.Music):setEnabled(value)
  elseif key == 'musicSoundVolume' then
    g_sounds.getChannel(SoundChannels.Music):setGain(value/100)
    if audioPanel then
      audioPanel:getChildById('musicSoundVolumeLabel'):setText(tr('Music volume: %d', value))
    end
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
  elseif key == 'enableLights' then
    addEvent(function()
      local map = modules.game_interface.getMapPanel()
      map:setDrawLights(value and options['ambientLight'] < 100)

      if graphicsPanel then
        graphicsPanel:getChildById('ambientLight'):setEnabled(value)
        graphicsPanel:getChildById('ambientLightLabel'):setEnabled(value)
      end
    end)
  elseif key == 'enableShaders' then
    g_graphics.setShouldUseShaders(value)
  elseif key == 'ambientLight' then
    addEvent(function()
      local map = modules.game_interface.getMapPanel()
      if graphicsPanel then
        graphicsPanel:getChildById('ambientLightLabel'):setText(tr('Ambient light: %s%%', value))
      end
      map:setMinimumAmbientLight(value/100)
      map:setDrawLights(options['enableLights'] and value < 100)
    end)
  elseif key == 'painterEngine' then
    g_graphics.selectPainterEngine(value)
    addEvent(function()
      if graphicsPanel then
        local shadersBox = graphicsPanel:getChildById('enableShaders')
        shadersBox:setEnabled(value == 2)
      end
    end)
  end
  g_settings.set(key, value)
  options[key] = value
end

function getOption(key)
  return options[key]
end

function addTab(name, panel, icon)
  optionsTabBar:addTab(name, panel, icon)
end
