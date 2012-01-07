-- this file use loaded after everything is loaded and initialized
-- you can place any custom user code here

Hotkeys.bind('F1', function() Game.talk('exura gran') end)
Hotkeys.bind('F2', function() Game.talk('exori frigo') end)
Hotkeys.bind('F3', function() Game.talk('exevo flam hur') end)
Hotkeys.bind('F4', function() Game.talk('exevo pan') end)
Hotkeys.bind('F5', function() Game.talk('exani tera') end)
Hotkeys.bind('Ctrl+R', function() runscript('otclientrc.lua') end)

if rcloaded then
  print('otclient.rc lua reloaded')
end
rcloaded = true