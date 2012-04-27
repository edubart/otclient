TextMessage = {}

-- require styles
importStyle 'textmessage.otui'

-- private variables
local MessageTypes = {
  consoleRed      = { color = '#F55E5E', consoleTab = tr('Default') },
  consoleOrange   = { color = '#FE6500', consoleTab = tr('Default') },
  consoleBlue     = { color = '#9F9DFD', consoleTab = tr('Default') },
  warning         = { color = '#F55E5E', consoleTab = tr('Server Log'), labelId = 'centerWarning', wrap = true },
  infoDescription = { color = '#00EB00', consoleTab = tr('Server Log'), labelId = 'centerInfo', consoleOption = 'showInfoMessagesInConsole', wrap = true },
  eventAdvance    = { color = '#FFFFFF', consoleTab = tr('Server Log'), labelId = 'centerAdvance', consoleOption = 'showEventMessagesInConsole', wrap = true },
  eventDefault    = { color = '#FFFFFF', consoleTab = tr('Server Log'), labelId = 'bottomStatus', consoleOption = 'showEventMessagesInConsole' },
  statusDefault   = { color = '#FFFFFF', consoleTab = tr('Server Log'), labelId = 'bottomStatus', consoleOption = 'showStatusMessagesInConsole' },
  statusSmall     = { color = '#FFFFFF', labelId = 'bottomStatus' },
}

local centerTextMessagePanel
local bottomStatusLabel

-- private functions
local function displayMessage(msgtype, msg, time)
  if not g_game.isOnline() then return end

  if msgtype.consoleTab ~= nil then
    if msgtype.consoleOption == nil or Options.getOption(msgtype.consoleOption) then
      Console.addText(msg, msgtype, msgtype.consoleTab)
    end
  end

  if msgtype.labelId  then
    local label = GameInterface.getMapPanel():recursiveGetChildById(msgtype.labelId)

    label:setText(msg)
    label:setColor(msgtype.color)

    if msgtype.wrap then
      label:setWidth(label:getParent():getWidth())
      label:setHeight(label:getTextSize().height)
    else
      label:resizeToText()
    end

    if not time then
      time = math.max(#msg * 100, 4000)
    else
      time = time * 1000
    end
    removeEvent(label.hideEvent)
    addEvent(function() label:setVisible(true) end)
    label.hideEvent = scheduleEvent(function() label:setVisible(false) end, time)
  end
end

local function createTextMessageLabel(id, parent)
  local label = createWidget('UILabel', parent)
  label:setFont('verdana-11px-rounded')
  label:setTextAlign(AlignCenter)
  label:setId(id)
  label:setMarginBottom(2)
  label:setTextWrap(true)
  label:setTextAutoResize(true)
  label:setVisible(false)
  return label
end

-- public functions
function TextMessage.init()
  connect(g_game, { onDeath = TextMessage.displayDeadMessage,
                    onTextMessage = TextMessage.display,
                    onGameStart = TextMessage.clearMessages })

  centerTextMessagePanel = createWidget('Panel', GameInterface.getMapPanel())
  centerTextMessagePanel:setId('centerTextMessagePanel')

  local layout = UIVerticalLayout.create(centerTextMessagePanel)
  layout:setFitChildren(true)
  centerTextMessagePanel:setLayout(layout)
  centerTextMessagePanel:setWidth(360)
  centerTextMessagePanel:centerIn('parent')

  createTextMessageLabel('centerWarning', centerTextMessagePanel)
  createTextMessageLabel('centerAdvance', centerTextMessagePanel)
  createTextMessageLabel('centerInfo', centerTextMessagePanel)

  bottomStatusLabel = createTextMessageLabel('bottomStatus', GameInterface.getMapPanel())
  bottomStatusLabel:setHeight(16)
  bottomStatusLabel:addAnchor(AnchorBottom, 'parent', AnchorBottom)
  bottomStatusLabel:addAnchor(AnchorLeft, 'parent', AnchorLeft)
  bottomStatusLabel:addAnchor(AnchorRight, 'parent', AnchorRight)
end

function TextMessage.terminate()
  disconnect(g_game, { onDeath = TextMessage.displayDeadMessage,
                       onTextMessage = TextMessage.display,
                       onGameStart = TextMessage.clearMessages })
  removeEvent(GameInterface.getMapPanel():recursiveGetChildById('centerWarning').hideEvent)
  removeEvent(GameInterface.getMapPanel():recursiveGetChildById('centerAdvance').hideEvent)
  removeEvent(GameInterface.getMapPanel():recursiveGetChildById('centerInfo').hideEvent)
  removeEvent(GameInterface.getMapPanel():recursiveGetChildById('bottomStatus').hideEvent)
  centerTextMessagePanel:destroy()
  centerTextMessagePanel = nil
  bottomStatusLabel:destroy()
  bottomStatusLabel = nil
  TextMessage = nil
end

function TextMessage.clearMessages()
  GameInterface.getMapPanel():recursiveGetChildById('centerWarning'):hide()
  GameInterface.getMapPanel():recursiveGetChildById('centerAdvance'):hide()
  GameInterface.getMapPanel():recursiveGetChildById('centerInfo'):hide()
  GameInterface.getMapPanel():recursiveGetChildById('bottomStatus'):hide()
end

function TextMessage.displayStatus(msg, time)
  displayMessage(MessageTypes.statusSmall, msg)
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

function TextMessage.displayDeadMessage()
  local advanceLabel = GameInterface.getMapPanel():recursiveGetChildById('centerAdvance')
  if advanceLabel:isVisible() then return end
  TextMessage.displayEventAdvance(tr('You are dead.'))
end
