local function pcolored(text, color)
  color = color or 'white'
  modules.client_terminal.addLine(text, color)
end

function draw_debug_boxes(enable)
  if enable == nil then enable = true end
  g_ui.setDebugBoxesDrawing(enable)
end

function hide_map()
  modules.game_interface.getMapPanel():hide()
end

function show_map()
  modules.game_interface.getMapPanel():show()
end

function auto_reload_module(name)
  local function reloadEvent()
    reloadModule(name)
    scheduleEvent(reloadEvent, 1000)
  end
  reloadEvent()
end

local function pingBack(ping) print(g_game.getWorldName() .. ' => ' .. ping .. ' ms') end
local pinging = false
function ping()
  if pinging then
    pdebug('Ping stopped.')
    g_game.setPingDelay(1000)
    disconnect(g_game, 'onPingBack', pingBack)
  else
    if not (g_game.getFeature(GameClientPing) or g_game.getFeature(GameExtendedClientPing)) then
      perror('this server does not support ping')
      return
    elseif not g_game.isOnline() then
      perror('ping command is only allowed when online')
      return
    end

    pdebug('Starting ping...')
    g_game.setPingDelay(0)
    connect(g_game, 'onPingBack', pingBack)
  end
  pinging = not pinging
end

function clear()
  modules.client_terminal.clear()
end

function ls(path)
  path = path or '/'
  local files = g_resources.listDirectoryFiles(path)
  for k,v in pairs(files) do
    if g_resources.directoryExists(path .. v) then
      pcolored(path .. v, 'blue')
    else
      pcolored(path .. v)
    end
  end
end

function about_version()
  pcolored(g_app.getName() .. ' ' .. g_app.getVersion() .. '\n' ..
        'Rev  ' .. g_app.getBuildRevision() .. ' ('.. g_app.getBuildCommit() .. ')\n' ..
        'Built on ' .. g_app.getBuildDate())
end

function about_graphics()
  pcolored('Vendor ' .. g_graphics.getVendor() )
  pcolored('Renderer' .. g_graphics.getRenderer())
  pcolored('Version' .. g_graphics.getVersion())
end

function about_modules()
  for k,m in pairs(g_modules.getModules()) do
    local loadedtext
    if m:isLoaded() then
      pcolored(m:getName() .. ' => loaded', 'green')
    else
      pcolored(m:getName() .. ' => not loaded', 'red')
    end
  end
end
