local defaultOptions = {
  vsync = false,
  showFps = false,
  showPing = false,
  fullscreen = false,
  classicControl = false,
  smartWalk = false,
  dashWalk = false,
  autoChaseOverride = true,
  showStatusMessagesInConsole = true,
  showEventMessagesInConsole = true,
  showInfoMessagesInConsole = true,
  showTimestampsInConsole = true,
  showLevelsInConsole = true,
  showPrivateMessagesInConsole = true,
  showPrivateMessagesOnScreen = true,
  showLeftPanel = false,
  foregroundFrameRate = 61,
  backgroundFrameRate = 201,
  painterEngine = 0,
  enableAudio = true,
  enableMusicSound = true,
  musicSoundVolume = 100,
  enableLights = true,
  ambientLight = 25,
  displayNames = true,
  displayHealth = true,
  displayText = true
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
end

function init()
  optionsWindow = g_ui.displayUI('options')
  optionsWindow:hide()
  optionsButton = modules.client_topmenu.addLeftButton('optionsButton', tr('Options'), '/images/topbuttons/options', toggle)

  optionsTabBar = optionsWindow:getChildById('optionsTabBar')
  optionsTabBar:setContentWidget(optionsWindow:getChildById('optionsTabContent'))

  addEvent(function() setup() end)

  g_keyboard.bindKeyDown('Ctrl+Shift+F', function() toggleOption('fullscreen') end)
  g_keyboard.bindKeyDown('Ctrl+N', toggleDisplays)

  audioButton = modules.client_topmenu.addLeftButton('audioButton', tr('Audio'), '/images/topbuttons/audio', function() toggleOption('enableAudio') end)
end

function terminate()
  g_keyboard.unbindKeyDown('Ctrl+Shift+F')
  g_keyboard.unbindKeyDown('Ctrl+N')
  optionsWindow:destroy()
  optionsButton:destroy()
  audioButton:destroy()
  optionsTabBar = nil
  generalPanel = nil
  consolePanel = nil
  graphicsPanel = nil
  audioPanel = nil
end

function setup()
  -- load options
  for k,v in pairs(defaultOptions) do
    g_settings.setDefault(k, v)
    if type(v) == 'boolean' then
      setOption(k, g_settings.getBoolean(k))
    elseif type(v) == 'number' then
      setOption(k, g_settings.getNumber(k))
    end
  end

  generalPanel = g_ui.loadUI('game')
  optionsTabBar:addTab(tr('Game'), generalPanel, '/images/optionstab/game')

  consolePanel = g_ui.loadUI('console')
  optionsTabBar:addTab(tr('Console'), consolePanel, '/images/optionstab/console')

  graphicsPanel = g_ui.loadUI('graphics')
  optionsTabBar:addTab(tr('Graphics'), graphicsPanel, '/images/optionstab/graphics')

  audioPanel = g_ui.loadUI('audio')
  optionsTabBar:addTab(tr('Audio'), audioPanel, '/images/optionstab/audio')

  setupGraphicsEngines()
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

function toggleDisplays()
  if options['displayNames'] and options['displayHealth'] then
    setOption('displayNames', false)
  elseif options['displayHealth'] then
    setOption('displayHealth', false)
  else
    if not options['displayNames'] and not options['displayHealth'] then
      setOption('displayNames', true)
    else
      setOption('displayHealth', true)
    end
  end
end

function toggleOption(key)
  setOption(key, not getOption(key))
end

function setOption(key, value)
  if options[key] == value then return end
  local gameMapPanel = modules.game_interface.getMapPanel()

  local panel = nil
  if key == 'vsync' then
    g_window.setVerticalSync(value)
  elseif key == 'showFps' then
    modules.client_topmenu.setFpsVisible(value)
  elseif key == 'showPing' then
    modules.client_topmenu.setPingVisible(value)
  elseif key == 'fullscreen' then
    g_window.setFullscreen(value)
    panel = graphicsPanel
  elseif key == 'enableAudio' then
    g_sounds.setAudioEnabled(value)
    addEvent(function()
      if value then
        audioButton:setIcon('/images/topbuttons/audio')
      else
        audioButton:setIcon('/images/topbuttons/audio_mute')
      end
    end)
    panel = audioPanel
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
  elseif key == 'displayNames' then
    gameMapPanel:setDrawNames(value)
    panel = generalPanel
  elseif key == 'displayHealth' then
    gameMapPanel:setDrawHealthBars(value)
    panel = generalPanel
  elseif key == 'displayText' then
    gameMapPanel:setDrawTexts(value)
    panel = generalPanel
  end

  -- change value for keybind updates
  if panel then
    local widget = panel:recursiveGetChildById(key)
    if widget and widget:getStyle().__class == 'UICheckBox' then
      widget:setChecked(value)
    end
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

function addButton(name, func, icon)
  optionsTabBar:addButton(name, func, icon)
end
