Locales = { }

-- private variables
local defaultLocaleName = 'en-us'
local installedLocales
local currentLocale

-- hooked functions
function UIWidget:onTextChange(text, oldText)
  local translation = tr(text)
  if translation ~= text then
    self:setText(translation)
  end
end

-- public functions
function Locales.init()
  installedLocales = {}

  dofile('en-us')
  dofile('pt-br')

  local userLocaleName = Settings.get('locale')
  if not userLocaleName or not Locales.setLocale(userLocaleName) then
    print('Locale ' .. userLocaleName .. ' is not loaded. Using default. ' .. defaultLocaleName)
    if not Locales.setLocale(defaultLocaleName) then
      fatal('Default locale could not be loaded. Re-install the program.')
      return
    end
    Settings.set('locale', defaultLocaleName)
  end

  -- create combobox
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

  installedLocales[locale.name] = locale
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
      -- todo: add some dots etc
    elseif tostring(text) then
      local translation = currentLocale.translation[text]
      if translation then
        return string.format(translation, ...)
      end
    end
  end
  return text
end