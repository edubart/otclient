-- this file use loaded after everything is loaded and initialized
-- you can place any custom user code here

Hotkeys.bindKeyDown('F1', function() Game.talk('exura gran') end)
Hotkeys.bindKeyDown('F2', function() Game.talk('exori mort') end)
Hotkeys.bindKeyDown('F3', function() Game.talk('exori frigo') end)
Hotkeys.bindKeyDown('F4', function() Game.talk('exevo vis hur') end)
Hotkeys.bindKeyDown('F5', function() Game.talk('utani gran hur') end)
Hotkeys.bindKeyDown('F6', function() Game.talk('exani tera') end)

local function reload()
  runscript('otclientrc.lua')
  TextMessage.displayEventAdvance('Script otclientrc.lua reloaded.')
  print('Script otclient.rc lua reloaded')
end
Hotkeys.bindKeyDown('Ctrl+R', reload)

rcloaded = true
