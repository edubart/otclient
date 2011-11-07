TextMessage = {}

-- require styles
importStyles '/textmessage/textmessage.otui'

-- private variables
local bottomLabelWidget, centerLabelWidget
local messageTypes = {
  first = 12,
  { type = 'MessageOrange', color = '#C87832', showOnConsole = true, showOnWindow = false },
  { type = 'MessageOrange', color = '#C87832', showOnConsole = true, showOnWindow = false },
  { type = 'MessageRed', color = '#C83200', showOnConsole = true, showOnWindow = true, windowLocation = 'CenterLabel' },
  { type = 'MessageWhite', color = '#FFFFFF', showOnConsole = true, showOnWindow = true, windowLocation = 'CenterLabel' },
  { type = 'MessageWhite', color = '#FFFFFF', showOnConsole = true, showOnWindow = true, windowLocation = 'BottomLabel' },
  { type = 'MessageWhite', color = '#FFFFFF', showOnConsole = true, showOnWindow = true, windowLocation = 'BottomLabel' },
  { type = 'MessageGreen', color = '#3FBE32', showOnConsole = true, showOnWindow = true, windowLocation = 'CenterLabel' },
  { type = 'MessageWhite', color = '#FFFFFF', showOnConsole = false, showOnWindow = true, windowLocation = 'BottomLabel' },
  { type = 'MessageBlue', color = '#3264C8', showOnConsole = true, showOnWindow = false },
  { type = 'MessageRed', color = '#C83200', showOnConsole = true, showOnWindow = false }
}
local hideEvent

-- public functions
function TextMessage.create()
  bottomLabelWidget = UILabel.create()
  Game.gameMapPanel:addChild(bottomLabelWidget)

  centerLabelWidget = UILabel.create()
  Game.gameMapPanel:addChild(centerLabelWidget)
end

-- hooked events
function Game.onTextMessage(type, message)
  local messageType = messageTypes[type - messageTypes.first]

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
    label:setForegroundColor(messageType.color)
    label:setText(message)
    label:setStyle(messageType.windowLocation)

    time = #message * 75
    removeEvent(hideEvent)
    hideEvent = scheduleEvent(function()
                    label:setVisible(false)
                  end, time)
  end
end


connect(Game, { onLogin = TextMessage.create,
                onLogout = TextMessage.destroy })