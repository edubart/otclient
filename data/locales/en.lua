locale = {
  name = "en",
  charset = "cp1252",
  languageName = "English",

  formatNumbers = true,
  decimalSeperator = '.',
  thousandsSeperator = ',',

  -- translations are not needed because everything is already in english
  translation = {}
}

modules.client_locales.installLocale(locale)
