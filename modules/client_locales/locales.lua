Locales = { }

-- private variables
local defaultLocaleName = 'en-us'
local installedLocales
local currentLocale

-- public functions
function Locales.init()
  installedLocales = {}

  Locales.installLocales('locales')

  local userLocaleName = Settings.get('locale')
  if not userLocaleName or not Locales.setLocale(userLocaleName) then
    print('Locale ' .. userLocaleName .. ' is not loaded. Using default. ' .. defaultLocaleName)
    if not Locales.setLocale(defaultLocaleName) then
      fatal('Default locale could not be loaded. Re-install the program.')
      return
    end
    Settings.set('locale', defaultLocaleName)
  end

  -- add event for creating combobox
  --for key,value in pairs(installedLocales) do
    -- add elements
  --end
end

function Locales.terminate()
  installedLocales = nil
  currentLocale = nil
end

function Locales.installLocale(locale)
  if not locale then
    print('Coult not install locale.')
    return false
  end

  if not locale.name then
    printf('Coult not install locale.')
    return false
  end

  local installedLocale = installedLocales[locale.name]
  if installedLocale then
    -- combine translations replacing with new if already exists
    for word,translation in pairs(locale.translation) do
      installedLocale.translation[word] = translation
    end
  else
    installedLocales[locale.name] = locale

    -- update combobox
  end
end

function Locales.installLocales(directory)
  dofiles(directory)
end

function Locales.setLocale(name)
  local locale = installedLocales[name]
  if locale then
    currentLocale = locale
    return true
  end

  print("Locale " .. name .. ' does not exist.')
  return false
end

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
      if translation then
        return string.format(translation, ...)
      elseif currentLocale.name ~= defaultLocaleName then
        print('WARNING: \"' .. text .. '\" could not be translated.')
      end
    end
  end
  return text
end