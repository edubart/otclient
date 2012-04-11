-- this file is loaded after all modules are loaded and initialized
-- you can place any custom user code here

Keyboard.bindKeyDown('Ctrl+R', Client.reloadScripts)

-- client usage report
local reportEvent
local reportMessage = g_app.getVersion() .. ";" .. g_app.getBuildRevision() .. ";" .. g_game.getProtocolVersion() .. ";" .. g_app.getBuildDate()
function reportClientUsage()
  if g_game.isOnline() and EnterGame.host and (EnterGame.host:lower():find('trpgb') or EnterGame.host:lower():find('tibiarpgbrasil')) then
    g_game.talk('!otc-report ' .. reportMessage)
  end
  removeEvent(reportEvent)
  reportEvent = scheduleEvent(reportClientUsage, 5*60000)
end
connect(g_game, { onLogin = reportClientUsage } )
