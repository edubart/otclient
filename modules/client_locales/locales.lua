Locales = { }

dofile 'neededtranslations.lua'

-- private variables
local defaultLocaleName = 'en'
local installedLocales
local currentLocale
local localeComboBox

-- private functions
local function sendLocale(localeName)
  local protocolGame = g_game.getProtocolGame()
  if protocolGame then
    protocolGame:sendExtendedOpcode(ExtendedLocales, localeName)
    return true
  end
  return false
end

local function onLocaleComboBoxOptionChange(self, optionText, optionData)
  if Locales.setLocale(optionData) then
    g_settings.set('locale', optionData)
    sendLocale(currentLocale.name)
    g_modules.reloadModules()
  end
end

-- hooked functions
local function onGameStart()
  sendLocale(currentLocale.name)
end

local function onExtendedLocales(protocol, opcode, buffer)
  local locale = installedLocales[buffer]
  if locale then
    localeComboBox:setCurrentOption(locale.languageName)
  end
end

-- public functions
function Locales.init()
  installedLocales = {}

  Locales.installLocales('locales')

  local userLocaleName = g_settings.get('locale', 'false')
  if userLocaleName ~= 'false' and Locales.setLocale(userLocaleName) then
    pdebug('Using configured locale: ' .. userLocaleName)
  else
    pdebug('Using default locale: ' .. defaultLocaleName)
    Locales.setLocale(defaultLocaleName)
    g_settings.set('locale', defaultLocaleName)
  end

  addEvent( function()
              localeComboBox = g_ui.createWidget('ComboBox', rootWidget:recursiveGetChildById('rightButtonsPanel'))
              localeComboBox:setFixedSize(true)
              for key,value in pairs(installedLocales) do
                localeComboBox:addOption(value.languageName, value.name)
              end
              localeComboBox:setCurrentOption(currentLocale.languageName)
              localeComboBox.onOptionChange = onLocaleComboBoxOptionChange
            end, false)

  Extended.register(ExtendedLocales, onExtendedLocales)
  connect(g_game, { onGameStart = onGameStart })
end

function Locales.terminate()
  installedLocales = nil
  currentLocale = nil
  localeComboBox = nil
  Extended.unregister(ExtendedLocales)
  disconnect(g_game, { onGameStart = onGameStart })
end

function generateNewTranslationTable(localename)
  local locale = installedLocales[localename]
  for _i,k in pairs(Locales.neededTranslations) do
    local trans = locale.translation[k]
    k = k:gsub('\n','\\n')
    k = k:gsub('\t','\\t')
    k = k:gsub('\"','\\\"')
    if trans then
      trans = trans:gsub('\n','\\n')
      trans = trans:gsub('\t','\\t')
      trans = trans:gsub('\"','\\\"')
    end
    if not trans then
      print('    ["' .. k .. '"]' .. ' = false,')
    else
      print('    ["' .. k .. '"]' .. ' = "' .. trans .. '",')
    end
  end
end

function Locales.installLocale(locale)
  if not locale or not locale.name then
    error('Unable to install locale.')
  end

  if locale.name ~= defaultLocaleName then
    for _i,k in pairs(Locales.neededTranslations) do
      if locale.translation[k] == nil then
        pwarning('Translation for locale \'' .. locale.name .. '\' not found: \"' .. k.. '\"')
      end
    end
  end

  local installedLocale = installedLocales[locale.name]
  if installedLocale then
    for word,translation in pairs(locale.translation) do
      installedLocale.translation[word] = translation
    end
  else
    installedLocales[locale.name] = locale
    if localeComboBox then
      localeComboBox.onOptionChange = nil
      localeComboBox:addOption(locale.languageName, locale.name)
      localeComboBox.onOptionChange = onLocaleComboBoxOptionChange
    end
  end
end

function Locales.installLocales(directory)
  dofiles(directory)
end

function Locales.setLocale(name)
  local locale = installedLocales[name]
  if not locale then
    pwarning("Locale " .. name .. ' does not exist.')
    return false
  end
  currentLocale = locale
  return true
end

-- global function used to translate texts
function tr(text, ...)
  if currentLocale then
    if tonumber(text) then
      -- todo: use locale information to calculate this. also detect floating numbers
      local out = ''
      local number = tostring(text):reverse()
      for i=1,#number do
        out = out .. number:sub(i, i)
        if i % 3 == 0 and i ~= #number then
          out = out .. ','
        end
      end
      return out:reverse()
    elseif tostring(text) then
      local translation = currentLocale.translation[text]
      if not translation then
        if currentLocale.name ~= defaultLocaleName then
          pwarning('Unable to translate: \"' .. text .. '\"')
        end
        translation = text
      end
      return string.format(translation, ...)
    end
  end
  return text
end
