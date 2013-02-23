ServerList = {}

-- private variables
local serverListWindow = nil
local serverTextList = nil
local servers = {}

-- private functions
function getServer(host)
  for k,server in pairs(servers) do
    if server.host == host then
      return server
    end
  end
end

function getServerKey(host)
  for k,server in pairs(servers) do
    if server.host == host then
      return k
    end
  end
end

-- public functions
function ServerList.init()
  serverListWindow = g_ui.displayUI('serverlist')
  serverTextList = serverListWindow:getChildById('serverList')

  servers = g_settings.getNode('ServerList') or {}
  ServerList.load()
end

function ServerList.terminate()
  ServerList.destroy()

  g_settings.setNode('ServerList', servers)

  ServerList = nil
end

function ServerList.load()
  for k,server in pairs(servers) do
    ServerList.add(server.host, server.port, server.protocol, true)
  end
end

function ServerList.select()
  local selected = serverTextList:getFocusedChild()
  if selected then
    local server = servers[getServerKey(selected:getId())]
    if server then
      EnterGame.setDefaultServer(server.host, server.port, server.protocol)
      EnterGame.setAccountName(server.account)
      EnterGame.setPassword(server.password)
      ServerList.hide()
    end
  end
end

function ServerList.add(host, port, protocol, load)
  if not load and getServerKey(host) then
    return false, 'Server already exists'
  elseif host == '' or port == '' then
    return false, 'Required fields are missing'
  end
  local widget = g_ui.createWidget('ServerWidget', serverTextList)
  widget:setId(host)
  
  if not load then
    servers[table.size(servers)+1] = {
      host = host,
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

function ServerList.remove(host)
  servers[getServerKey(host)] = nil
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
  local key = getServerKey(host)
  if servers[key] then
    servers[key].account = account
  end
end

function ServerList.setServerPassword(host, password)
  local key = getServerKey(host)
  if servers[key] then
    servers[key].password = password
  end
end