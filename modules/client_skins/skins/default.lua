local skin = {
  name = 'Default',

  -- first font is default
  fonts = {
    'verdana-11px-antialised',
    'verdana-11px-monochrome',
    'verdana-11px-rounded',
    'terminus-14px-bold'
  },
  defaultFont = 'verdana-11px-antialised',

  styles = {
    'buttons.otui',
    'creaturebuttons.otui',
    'labels.otui',
    'panels.otui',
    'separators.otui',
    'textedits.otui',
    'checkboxes.otui',
    'progressbars.otui',
    'tabbars.otui',
    'windows.otui',
    'listboxes.otui',
    'popupmenus.otui',
    'comboboxes.otui',
    'spinboxes.otui',
    'messageboxes.otui',
    'scrollbars.otui',
    'splitters.otui',
    'miniwindow.otui',
    'items.otui',
    'creatures.otui',
    'tables.otui'
  },

  particles = {
    'shiny.otps'
  }
}

Skins.installSkin(skin)
