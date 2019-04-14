UUID = nil
HOST = 'otclient.herokuapp.com'
PORT = 80
FIRST_REPORT_DELAY = 15
REPORT_DELAY = 60

sendReportEvent = nil
firstReportEvent = nil

function initUUID()
  UUID = g_settings.getString('report-uuid')
  if not UUID or #UUID ~= 36 then
    UUID = g_crypt.genUUID()
    g_settings.set('report-uuid', UUID)
  end
end

function init()
  connect(g_game, { onGameStart = onGameStart,
                    onGameEnd = onGameEnd })

  initUUID()
end

function terminate()
  disconnect(g_game, { onGameStart = onGameStart,
                       onGameEnd = onGameEnd })
  removeEvent(firstReportEvent)
  removeEvent(sendReportEvent)
end

function configure(host, port, delay)
  if not host then return end
  HOST = host
  PORT = port or PORT
  REPORT_DELAY = delay or REPORT_DELAY
end

function sendReport()
  if not HOST then return end
  local protocolHttp = ProtocolHttp.create()
  protocolHttp.onConnect = onConnect
  protocolHttp.onRecv = onRecv
  protocolHttp.onError = onError
  protocolHttp:connect(HOST, PORT)
end

function onGameStart()
  if not HOST then return end
  removeEvent(firstReportEvent)
  removeEvent(sendReportEvent)
  firstReportEvent = addEvent(sendReport, FIRST_REPORT_DELAY*1000)
  sendReportEvent = cycleEvent(sendReport, REPORT_DELAY*1000)
end

function onGameEnd()
  removeEvent(firstReportEvent)
  removeEvent(sendReportEvent)
end

function onConnect(protocol)
  if not g_game.isOnline() then
    protocol:disconnect()
    return
  end

  local post = ''
  post = post .. 'uid='                .. urlencode(UUID)
  post = post .. '&report_delay='      .. REPORT_DELAY
  post = post .. '&os='                .. urlencode(g_app.getOs())
  post = post .. '&graphics_vendor='   .. urlencode(g_graphics.getVendor())
  post = post .. '&graphics_renderer=' .. urlencode(g_graphics.getRenderer())
  post = post .. '&graphics_version='  .. urlencode(g_graphics.getVersion())
  post = post .. '&painter_engine='    .. urlencode(g_graphics.getPainterEngine())
  post = post .. '&fps='               .. urlencode(g_app.getBackgroundPaneFps())
  post = post .. '&max_fps='           .. urlencode(g_app.getBackgroundPaneMaxFps())
  post = post .. '&fullscreen='        .. urlencode(tostring(g_window.isFullscreen()))
  post = post .. '&window_width='      .. urlencode(g_window.getWidth())
  post = post .. '&window_height='     .. urlencode(g_window.getHeight())
  post = post .. '&player_name='       .. urlencode(g_game.getCharacterName())
  post = post .. '&world_name='        .. urlencode(g_game.getWorldName())
  post = post .. '&otserv_host='       .. urlencode(G.host)
  post = post .. '&otserv_port='       .. G.port
  post = post .. '&otserv_protocol='   .. urlencode(g_game.getProtocolVersion())
  post = post .. '&otserv_client='     .. urlencode(g_game.getClientVersion())
  post = post .. '&build_version='     .. urlencode(g_app.getVersion())
  post = post .. '&build_revision='    .. urlencode(g_app.getBuildRevision())
  post = post .. '&build_commit='      .. urlencode(g_app.getBuildCommit())
  post = post .. '&build_date='        .. urlencode(g_app.getBuildDate())
  post = post .. '&display_width='     .. g_window.getDisplayWidth()
  post = post .. '&display_height='    .. g_window.getDisplayHeight()
  post = post .. '&cpu='               .. urlencode(g_platform.getCPUName())
  post = post .. '&mem='               .. g_platform.getTotalSystemMemory()
  post = post .. '&os_name='           .. urlencode(g_platform.getOSName())
  post = post .. getAdditionalData()

  local message = ''
  message = message .. "POST /report HTTP/1.1\r\n"
  message = message .. "Host: " .. HOST .. "\r\n"
  message = message .. "Accept: */*\r\n"
  message = message .. "Connection: close\r\n"
  message = message .. "Content-Type: application/x-www-form-urlencoded\r\n"
  message = message .. "Content-Length: " .. post:len() .. "\r\n\r\n"
  message = message .. post

  protocol:send(message)
  protocol:recv()
end

function getAdditionalData()
  return ''
end

function onRecv(protocol, message)
  if string.find(message, 'HTTP/1.1 200 OK') then
    --pinfo('Stats sent to server successfully!')
  end
  protocol:disconnect()
end

function onError(protocol, message, code)
  pdebug('Could not send statistics: ' .. message)
end

function urlencode(str)
    local encodeChar=function(chr)
	    return string.format("%%%X",string.byte(chr))
    end
	local output, t = string.gsub(str,"[^%w]",encodeChar)
	return output
end
