HOST = '189.55.105.125'
PORT = 3000

function init()
  connect(g_game, { onGameStart = onGameStart })

  protocolHttp = ProtocolHttp.create()
  connect(protocolHttp, { onConnect = onConnect,
    onRecv = onRecv,
    onError = onError })
end

function terminate()
  disconnect(g_game, { onGameStart = onGameStart })

  disconnect(protocolHttp, { onConnect = onConnect,
    onRecv = onRecv,
    onError = onError })
  protocolHttp = nil
end

function sendInfo()
  protocolHttp:connect(HOST, PORT)
end

-- events
function onGameStart()
  scheduleEvent(sendInfo, 5*1000)
end

function onConnect(protocol)
  pinfo('Connected to stats server.')

  if not g_game.isOnline() then
    perror('Could not send stats. Game not online.')
    protocol:disconnect()
    return
  end

  local post = ''
  post = post .. 'os='                 .. g_app.getOs()
  post = post .. '&graphics_vendor='   .. g_graphics.getVendor()
  post = post .. '&graphics_renderer=' .. g_graphics.getRenderer()
  post = post .. '&graphics_version='  .. g_graphics.getVersion()
  post = post .. '&painter_engine='    .. g_graphics.getPainterEngine()
  post = post .. '&fps='               .. g_app.getBackgroundPaneFps()
  post = post .. '&max_fps='           .. g_app.getBackgroundPaneMaxFps()
  post = post .. '&fullscreen='        .. fromboolean(g_window.isFullscreen())
  post = post .. '&window_width='      .. g_window.getWidth()
  post = post .. '&window_height='     .. g_window.getHeight()
  post = post .. '&player_name='       .. g_game.getLocalPlayer():getName()
  post = post .. '&otserv_host='       .. G.host
  post = post .. '&otserv_port='       .. G.port
  post = post .. '&otserv_protocol='   .. g_game.getClientVersion()
  post = post .. '&build_version='     .. g_app.getVersion()
  post = post .. '&build_revision='    .. g_app.getBuildRevision()
  post = post .. '&build_commit='      .. g_app.getBuildCommit()
  post = post .. '&build_date='        .. g_app.getBuildDate()
  post = post .. '&display_width='     .. g_window.getDisplayWidth()
  post = post .. '&display_height='    .. g_window.getDisplayHeight()

  local message = ''
  message = message .. "POST /report HTTP/1.0\r\n"
  message = message .. "Host: " .. HOST .. "\r\n"
  message = message .. "Accept: */*\r\n"
  message = message .. "Connection: close\r\n"
  message = message .. "Content-Type: application/x-www-form-urlencoded\r\n"
  message = message .. "Content-Length: " .. post:len() .. "\r\n\r\n"
  message = message .. post

  protocol:send(message)
  protocol:recv()
end

function onRecv(protocol, message)
  if string.find(message, 'HTTP/1.1 200 OK') then
    pinfo('Stats sent to server successfully!')
  end
  protocol:disconnect()
end

function onError(protocol, message, code)
  perror('Could not send statistics. ' .. message .. 'Code: ' .. code)
end
