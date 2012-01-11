TextMessage = {}

-- require styles
importStyle 'textmessage.otui'

-- private variables
local MessageTypes = {
  warning = { color = '#F55E5E', showOnConsole = true, windowLocation = 'center', consoleOption = 'showInfoMessagesInConsole' },
  eventAdvance = { color = '#FFFFFF', showOnConsole = true, windowLocation = 'center', consoleOption = 'showEventMessagesInConsole' },
  eventDefault = { color = '#FFFFFF', showOnConsole = true, windowLocation = 'bottom', consoleOption = 'showEventMessagesInConsole' },
  eventOrange = { color = '#FE6500', showOnConsole = true, windowLocation = 'bottom', consoleOption = 'showEventMessagesInConsole' },
  statusDefault = { color = '#FFFFFF', showOnConsole = true, windowLocation = 'bottom', consoleOption = 'showStatusMessagesInConsole' },
  infoDescription = { color = '#00EB00', showOnConsole = true, windowLocation = 'center', consoleOption = 'showInfoMessagesInConsole' },
  statusSmall = { color = '#FFFFFF', showOnConsole = false, windowLocation = 'bottom' },
  consoleOrange = { color = '#FE6500', showOnConsole = true },
  consoleBlue =  { color = '#9F9DFD', showOnConsole = true },
  consoleRed = { color = '#F55E5E', showOnConsole = true }
}

local bottomLabelWidget
local centerLabelWidget
local bottomLabelHideEvent
local centerLabelHideEvent

-- private functions
local function displayMessage(msgtype, msg, time)
  if not Game.isOnline() then return end

  if msgtype.showOnConsole then
    if msgtype.consoleOption == nil or Options[msgtype.consoleOption] then
      Console.addText(msg, msgtype.color)
    end
  end

  if msgtype.windowLocation  then
    local label
    local style
    if msgtype.windowLocation == 'bottom' then
      label = bottomLabelWidget
      style = 'BottomLabel'
    elseif msgtype.windowLocation == 'center' then
      label = centerLabelWidget
      style = 'CenterLabel'
    end

    label:setVisible(true)
    label:setText(msg)
    label:setStyle(style)
    label:setColor(msgtype.color)

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
  if msgtype then
    displayMessage(msgtype, msg)
  end
end

-- hooked events
local function onGameDeath()
  TextMessage.displayEventAdvance('You are dead.', 10)
  scheduleEvent(function() Game.logout(true) end, 10*1000)
end

local function onGameTextMessage(msgtype, msg)
  TextMessage.display(msgtype, msg)
end

connect(Game, { onLogin = TextMessage.create,
                onLogout = TextMessage.destroy,
                onDeath = onGameDeath,
                onTextMessage = onGameTextMessage })
