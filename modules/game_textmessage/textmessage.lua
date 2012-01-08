TextMessage = {}

-- require styles
importStyle 'textmessage.otui'

-- private variables
local bottomLabelWidget, centerLabelWidget

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

local MessageTypesMap = {
  [12] = MessageTypes.consoleOrange,
  [13] = MessageTypes.consoleOrange,
  [14] = MessageTypes.warning,
  [15] = MessageTypes.eventAdvance,
  [15] = MessageTypes.eventDefault,
  [16] = MessageTypes.statusDefault,
  [17] = MessageTypes.infoDesc,
  [18] = MessageTypes.statusSmall,
  [19] = MessageTypes.consoleBlue,
  [20] = MessageTypes.consoleRed,
--[[
  [18] = MessageTypes.consoleRed,
  [19] = MessageTypes.consoleOrange,
  [20] = MessageTypes.consoleOrange,
  [21] = MessageTypes.warning,
  [22] = MessageTypes.eventAdvance,
  [23] = MessageTypes.eventDefault,
  [24] = MessageTypes.statusDefault,
  [25] = MessageTypes.infoDesc,
  [26] = MessageTypes.statusSmall,
  [27] = MessageTypes.consoleBlue
]]--
}

-- private variables
local bottomLabelHideEvent
local centerLabelHideEvent

-- private functions
local function displayMessage(msgtype, msg)
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

    time = #msg * 75
    removeEvent(label.hideEvent)
    label.hideEvent = scheduleEvent(function() label:setVisible(false) end, time)
  end
end

-- public functions
function TextMessage.create()
  bottomLabelWidget = createWidget('UILabel', Game.gameMapPanel)
  centerLabelWidget = createWidget('UILabel', Game.gameMapPanel)
end

function TextMessage.displayWarning(msg)
  TextMessage.display(MessageTypes.warning, msg)
end

function TextMessage.display(msgtypeid, msg)
  local msgtype = MessageTypesMap[msgtypeid]
  if msgtype == nil then
    error('unknown text msg type ' .. msgtypeid)
    return
  end
  displayMessage(msgtype, msg)
end

-- hooked events
function TextMessage.onTextMessage(msgtypeid, msg)
  TextMessage.display(msgtypeid, msg)
end


connect(Game, { onLogin = TextMessage.create,
                onLogout = TextMessage.destroy,
                onTextMessage = TextMessage.onTextMessage })
