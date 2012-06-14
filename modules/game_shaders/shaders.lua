Shaders = {}

local HOTKEY = 'Ctrl+X'
local SHADERS = {
  ['Default'] = 'shaders/default.frag',
  ['Bloom'] = 'shaders/bloom.frag',
  ['Sepia'] = 'shaders/sepia.frag',
  ['Grayscale'] = 'shaders/grayscale.frag',
  ['Pulse'] = 'shaders/pulse.frag',
  ['Old Tv'] = 'shaders/oldtv.frag',
  ['Fog'] = 'shaders/fog.frag',
  ['Party'] = 'shaders/party.frag',
  ['Radial Blur'] = 'shaders/radialblur.frag',
  ['Zomg'] = 'shaders/zomg.frag',
}

local shadersPanel

function Shaders.init()
  importStyle 'shaders.otui'


  Keyboard.bindKeyDown(HOTKEY, Shaders.toggle)

  shadersPanel = createWidget('ShadersPanel', GameInterface.getMapPanel())
  shadersPanel:hide()

  local mapComboBox = shadersPanel:getChildById('mapComboBox')
  mapComboBox.onOptionChange = function(combobox, option)
    local map = GameInterface.getMapPanel()
    map:setMapShader(g_shaders.getShader(option))
    print(option)
  end

  for name,file in pairs(SHADERS) do
    local shader = g_shaders.createFragmentShader(name, file)
    mapComboBox:addOption(name)

    if name == 'Fog' then
      shader:addMultiTexture('images/clouds.png')
    end
  end

  local map = GameInterface.getMapPanel()
  map:setMapShader(g_shaders.getShader('Default'))
end

function Shaders.terminate()
  Keyboard.unbindKeyDown(HOTKEY)
  shadersPanel:destroy()
  shadersPanel = nil
end

function Shaders.toggle()
  shadersPanel:setVisible(not shadersPanel:isVisible())
end
