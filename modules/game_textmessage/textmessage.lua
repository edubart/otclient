TextMessage = {}

-- require styles
importStyle 'textmessage.otui'

-- private variables
local bottomLabelWidget, centerLabelWidget
local messageTypes = {
  first = 12,
  { msgtype = 'MessageOrange', color = '#C87832', showOnConsole = true, showOnWindow = false },
  { msgtype = 'MessageOrange', color = '#C87832', showOnConsole = true, showOnWindow = false },
  { msgtype = 'MessageRed', color = '#C83200', showOnConsole = true, showOnWindow = true, windowLocation = 'CenterLabel' },
  { msgtype = 'MessageWhite', color = '#FFFFFF', showOnConsole = true, showOnWindow = true, windowLocation = 'CenterLabel' },
  { msgtype = 'MessageWhite', color = '#FFFFFF', showOnConsole = true, showOnWindow = true, windowLocation = 'BottomLabel' },
  { msgtype = 'MessageWhite', color = '#FFFFFF', showOnConsole = true, showOnWindow = true, windowLocation = 'BottomLabel' },
  { msgtype = 'MessageGreen', color = '#3FBE32', showOnConsole = true, showOnWindow = true, windowLocation = 'CenterLabel' },
  { msgtype = 'MessageWhite', color = '#FFFFFF', showOnConsole = false, showOnWindow = true, windowLocation = 'BottomLabel' },
  { msgtype = 'MessageBlue', color = '#3264C8', showOnConsole = true, showOnWindow = false },
  { msgtype = 'MessageRed', color = '#C83200', showOnConsole = true, showOnWindow = false }
}
local hideEvent

-- public functions
function TextMessage.create()
  bottomLabelWidget = createWidget('UILabel', Game.gameMapPanel)
  centerLabelWidget = createWidget('UILabel', Game.gameMapPanel)
end

-- hooked events
function TextMessage.onTextMessage(msgtype, message)
  local messageType = messageTypes[msgtype - messageTypes.first]

  if messageType.showOnConsole then
    -- TODO
  end

  if messageType.showOnWindow then
    local label
    if messageType.windowLocation == 'BottomLabel' then
      label = bottomLabelWidget
    elseif messageType.windowLocation == 'CenterLabel' then
      label = centerLabelWidget
    end

    label:setVisible(true)
    label:setText(message)
    label:setStyle(messageType.windowLocation)
    label:setForegroundColor(messageType.color)

    time = #message * 75
    removeEvent(hideEvent)
    hideEvent = scheduleEvent(function()
                    label:setVisible(false)
                  end, time)
  end
end


connect(Game, { onLogin = TextMessage.create,
                onLogout = TextMessage.destroy,
                onTextMessage = TextMessage.onTextMessage })
