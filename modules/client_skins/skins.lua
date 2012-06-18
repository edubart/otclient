Skins = { }

-- private variables
local defaultSkinName = 'Default'
local installedSkins
local currentSkin
local skinComboBox

-- private functions
local function onSkinComboBoxOptionChange(self, optionText, optionData)
  if Skins.setSkin(optionText) then
    Settings.set('skin', optionText)
    reloadModules()
  end
end

-- public functions
function Skins.init()
  installedSkins = {}

  Skins.installSkins('skins')

  local userSkinName = Settings.get('skin')
  if userSkinName and Skins.setSkin(userSkinName) then
    info('Using configured skin: ' .. userSkinName)
  else
    info('Using default skin: ' .. defaultSkinName)
    Skins.setSkin(defaultSkinName)
    Settings.set('skin', defaultSkinName)
  end

  addEvent( function()
              skinComboBox = createWidget('ComboBox', rootWidget:recursiveGetChildById('rightButtonsPanel'))
              for key,value in pairs(installedSkins) do
                skinComboBox:addOption(value.name)
              end
              skinComboBox:setCurrentOption(currentSkin.name)
              skinComboBox.onOptionChange = onSkinComboBoxOptionChange
            end, false)
end

function Skins.terminate()
  installedSkins = nil
  currentSkin = nil
  skinComboBox = nil
end

function Skins.installSkin(skin)
  if not skin or not skin.name or not skin.styles then
    error('Unable to install skin.')
    return false
  end

  installedSkins[skin.name] = skin
  -- todo: maybe combine styles if skin already exists
  return true
end

function Skins.installSkins(directory)
  dofiles(directory)
end

function Skins.setSkin(name)
  local skin = installedSkins[name]
  if not skin then
    warning("Skin " .. name .. ' does not exist.')
    return false
  end

  --g_ui.clearStyles() -- this is crashing
  for i=1,#skin.styles do
    g_ui.importStyle('skins/' .. string.lower(name) .. '/' .. skin.styles[i])
  end

  currentSkin = skin
  return true
end
