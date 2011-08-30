local textMessageWidget

function Game.onTextMessage(message)
  if textMessageWidget then
    textMessageWidget:destroy()
  end

  local newTextMessageWidget = loadUI('/game/ui/textmessage.otui', Game.gameMapUi)
  time = #message * 75
  newTextMessageWidget:setText(message)
  scheduleEvent(function()
                  newTextMessageWidget:destroy()
                end, time)
  textMessageWidget = newTextMessageWidget
end
