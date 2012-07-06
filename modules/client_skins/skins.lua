Skins = { }

-- private variables
local defaultSkinName = 'Default'
local installedSkins
local currentSkin
local skinComboBox

-- private functions
local function onSkinComboBoxOptionChange(self, optionText, optionData)
  if Skins.setSkin(optionText) then
    g_settings.set('skin', optionText)
    g_modules.reloadModules()
  end
end

local function getSkinPath(name)
  local current = getfsrcpath()
  return g_resources.getRealDir(current) .. current .. '/skins/' .. string.lower(name)
end

-- public functions
function Skins.init()
  installedSkins = {}

  Skins.installSkins('skins')

  if installedSkins[defaultSkinName] then
    g_resources.addToSearchPath(getSkinPath(defaultSkinName), 0)
  end

  local userSkinName = g_settings.get('skin', 'false')
  if userSkinName ~= 'false' and Skins.setSkin(userSkinName) then
    pdebug('Using configured skin: ' .. userSkinName)
  else
    pdebug('Using default skin: ' .. defaultSkinName)
    Skins.setSkin(defaultSkinName)
    g_settings.set('skin', defaultSkinName)
  end

  addEvent( function()
              skinComboBox = g_ui.createWidget('ComboBox', rootWidget:recursiveGetChildById('rightButtonsPanel'))
              for key,value in pairs(installedSkins) do
                skinComboBox:addOption(value.name)
              end
              skinComboBox:setCurrentOption(currentSkin.name)
              skinComboBox.onOptionChange = onSkinComboBoxOptionChange
            end, false)
end

function Skins.terminate()
  g_resources.removeFromSearchPath(getSkinPath(defaultSkinName))
  if currentSkin then
    g_resources.removeFromSearchPath(getSkinPath(currentSkin.name))
  end

  installedSkins = nil
  currentSkin = nil
  skinComboBox = nil
end

function Skins.installSkin(skin)
  if not skin or not skin.name or not skin.styles then
    error('Unable to install skin.')
    return false
  end

  if installedSkins[skin.name] then
    pwarning(skin.name .. ' has been replaced.')
  end

  installedSkins[skin.name] = skin
  return true
end

function Skins.installSkins(directory)
  dofiles(directory)
end

function Skins.setSkin(name)
  local skin = installedSkins[name]
  if not skin then
    pwarning("Skin " .. name .. ' does not exist.')
    return false
  end

  g_fonts.clearFonts()
  g_ui.clearStyles()

  if name ~= defaultSkinName then
    local defaultSkin = installedSkins[defaultSkinName]
    if not defaultSkin then
      error("Default skin is not installed.")
      return false
    end

    Skins.loadSkin(defaultSkin)
  end

  if currentSkin then
    g_resources.removeFromSearchPath(getSkinPath(currentSkin.name))
  end
  g_resources.addToSearchPath(getSkinPath(skin.name), true)

  Skins.loadSkin(skin)
  currentSkin = skin
  return true
end

function Skins.loadSkin(skin)
  local lowerName = string.lower(skin.name)

  if skin.fonts then
    for i=1,#skin.fonts do
      g_fonts.importFont('skins/' .. lowerName .. '/fonts/' .. skin.fonts[i])

      if i == 1 then
        g_fonts.setDefaultFont(skin.fonts[i])
      end
    end
  end

  if skin.styles then
    for i=1,#skin.styles do
      g_ui.importStyle('skins/' .. lowerName .. '/styles/' .. skin.styles[i])
    end
  end

  if skin.particles then
    for i=1,#skin.particles do
      g_particles.importParticle('skins/' .. lowerName .. '/particles/' .. skin.particles[i])
    end
  end
end
