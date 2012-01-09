PingBar = {}

local pingLabel

-- public functions
function PingBar.init()
  pingLabel = createWidget('UILabel', rootWidget:recursiveGetChildById('leftButtonsPanel'))
  pingLabel:applyStyle({ ['anchors.left'] =  'prev.right',
                         ['anchors.top'] = 'parent.top',
                         ['margin-top'] = 12,
                         ['margin-left'] = 10,
                         font = 'verdana-11px-rounded',
                         color = '#FE6500',
                         width = 120,
                         height = 16})
end

function PingBar.terminate()
  pingLabel:destroy()
end

-- hooked events
local function onGamePingUpdate(ping)
  pingLabel:setText('Walk Ping: ' .. ping .. ' ms')
end

connect(Game, { onWalkPingUpdate = onGamePingUpdate })