ServerList = {}

-- private variables
local serverListWindow = nil
local serverTextList = nil
local removeWindow = nil
local servers = {}

-- public functions
function ServerList.init()
  serverListWindow = g_ui.displayUI('serverlist')
  serverTextList = serverListWindow:getChildById('serverList')

  servers = g_settings.getNode('ServerList') or {}
  if servers then
    ServerList.load()
  end
end

function ServerList.terminate()
  ServerList.destroy()

  g_settings.setNode('ServerList', servers)

  ServerList = nil
end

function ServerList.load()
  for host, server in pairs(servers) do
    ServerList.add(host, server.port, server.protocol, true)
  end
end

function ServerList.select()
  local selected = serverTextList:getFocusedChild()
  if selected then
    local server = servers[selected:getId()]
    if server then
      EnterGame.setDefaultServer(selected:getId(), server.port, server.protocol)
      EnterGame.setAccountName(server.account)
      EnterGame.setPassword(server.password)
      ServerList.hide()
    end
  end
end

function ServerList.add(host, port, protocol, load)
  if not host or not port or not protocol then
    return false, 'Failed to load settings'
  elseif not load and servers[host] then
    return false, 'Server already exists'
  elseif host == '' or port == '' then
    return false, 'Required fields are missing'
  end
  local widget = g_ui.createWidget('ServerWidget', serverTextList)
  widget:setId(host)

  if not load then
    servers[host] = {
      port = port,
      protocol = protocol,
      account = '',
      password = ''
    }
  end

  local details = widget:getChildById('details')
  details:setText(host..':'..port)

  local proto = widget:getChildById('protocol')
  proto:setText(protocol)

  connect(widget, { onDoubleClick = function () ServerList.select() return true end } )
  return true
end

function ServerList.remove(widget)
  local host = widget:getId()

  if removeWindow then
    return
  end

  local yesCallback = function()
    widget:destroy()
    servers[host] = nil
    removeWindow:destroy()
    removeWindow=nil
  end
  local noCallback = function()
    removeWindow:destroy()
    removeWindow=nil
  end

  removeWindow = displayGeneralBox(tr('Remove'), tr('Remove '..host..'?'), {
      { text=tr('Yes'), callback=yesCallback },
      { text=tr('No'), callback=noCallback },
      anchor=AnchorHorizontalCenter}, yesCallback, noCallback)
end

function ServerList.destroy()
  if serverListWindow then
    serverTextList = nil
    serverListWindow:destroy()
    serverListWindow = nil
  end
end

function ServerList.show()
  if g_game.isOnline() then
    return
  end
  serverListWindow:show()
  serverListWindow:raise()
  serverListWindow:focus()
end

function ServerList.hide()
  serverListWindow:hide()
end

function ServerList.setServerAccount(host, account)
  if servers[host] then
    servers[host].account = account
  end
end

function ServerList.setServerPassword(host, password)
  if servers[host] then
    servers[host].password = password
  end
end