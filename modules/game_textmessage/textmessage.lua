TextMessage = {}

-- require styles
importStyle 'textmessage.otui'

-- private variables
local MessageTypes = {
  warning = { color = '#F55E5E', showOnConsole = true, showOnWindow = true, windowLocation = 'CenterLabel' },
  eventAdvance = { color = '#FFFFFF', showOnConsole = true, showOnWindow = true, windowLocation = 'CenterLabel' },
  eventDefault = { color = '#FFFFFF', showOnConsole = true, showOnWindow = true, windowLocation = 'BottomLabel' },
  statusDefault = { color = '#FFFFFF', showOnConsole = true, showOnWindow = true, windowLocation = 'BottomLabel' },
  infoDesc = { color = '#00EB00', showOnConsole = true, showOnWindow = true, windowLocation = 'CenterLabel' },
  statusSmall = { color = '#FFFFFF', showOnConsole = false, showOnWindow = true, windowLocation = 'BottomLabel' },
  consoleOrange = { color = '#FE6500', showOnConsole = true, showOnWindow = false },
  consoleBlue =  { color = '#9F9DFD', showOnConsole = true, showOnWindow = false },
  consoleRed = { color = '#F55E5E', showOnConsole = true, showOnWindow = false }
}

local bottomLabelWidget
local centerLabelWidget
local bottomLabelHideEvent
local centerLabelHideEvent

-- private functions
local function displayMessage(msgtype, msg, time)
  if msgtype.showOnConsole then
    -- TODO
  end

  if msgtype.showOnWindow then
    local label
    if msgtype.windowLocation == 'BottomLabel' then
      label = bottomLabelWidget
    elseif msgtype.windowLocation == 'CenterLabel' then
      label = centerLabelWidget
    end

    label:setVisible(true)
    label:setText(msg)
    label:setStyle(msgtype.windowLocation)
    label:setForegroundColor(msgtype.color)

    if not time then
      time = math.max(#msg * 75, 3000)
    else
      time = time * 1000
    end
    removeEvent(label.hideEvent)
    label.hideEvent = scheduleEvent(function() label:setVisible(false) end, time)
  end
end

-- public functions
function TextMessage.create()
  bottomLabelWidget = createWidget('UILabel', Game.gameMapPanel)
  centerLabelWidget = createWidget('UILabel', Game.gameMapPanel)
end

function TextMessage.displayStatus(msg, time)
  displayMessage(MessageTypes.warning, msg)
end

function TextMessage.displayEventAdvance(msg, time)
  displayMessage(MessageTypes.eventAdvance, msg, time)
end

function TextMessage.display(msgtypedesc, msg)
  local msgtype = MessageTypes[msgtypedesc]
  if msgtype == nil then
    error('unknown text msg type ' .. msgtype)
    return
  end
  displayMessage(msgtype, msg)
end

-- hooked events
local function onGameDeath()
  TextMessage.displayEventAdvance('You are dead.', 10)
end

local function onGameTextMessage(msgtype, msg)
  TextMessage.display(msgtype, msg)
end

connect(Game, { onLogin = TextMessage.create,
                onLogout = TextMessage.destroy,
                onDeath = onGameDeath,
                onTextMessage = onGameTextMessage })
