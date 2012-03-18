-- this file is loaded after all modules are loaded and initialized
-- you can place any custom user code here
-- you

Keyboard.bindKeyDown('F1', function() g_game.talk('exura gran') end)
Keyboard.bindKeyDown('F2', function() g_game.talk('exori mort') end)
Keyboard.bindKeyDown('F3', function() g_game.talk('exori frigo') end)
Keyboard.bindKeyDown('F4', function() g_game.talk('exevo vis hur') end)
Keyboard.bindKeyDown('F5', function() g_game.talk('utani gran hur') end)
Keyboard.bindKeyDown('F6', function() g_game.talk('exani tera') end)
Keyboard.bindKeyDown('Ctrl+R', Client.reloadScripts)
