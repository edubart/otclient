-- this file use loaded after everything is loaded and initialized
-- you can place any custom user code here

Keyboard.bindKeyDown('F1', function() Game.talk('exura gran') end)
Keyboard.bindKeyDown('F2', function() Game.talk('exori mort') end)
Keyboard.bindKeyDown('F3', function() Game.talk('exori frigo') end)
Keyboard.bindKeyDown('F4', function() Game.talk('exevo vis hur') end)
Keyboard.bindKeyDown('F5', function() Game.talk('utani gran hur') end)
Keyboard.bindKeyDown('F6', function() Game.talk('exani tera') end)

local function reload()
  dofile('otclientrc.lua')
  TextMessage.displayEventAdvance('Script otclientrc.lua reloaded.')
  print('Script otclient.rc lua reloaded')
end
Keyboard.bindKeyDown('Ctrl+R', reload)

rcloaded = true
