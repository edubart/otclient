TextMessage = {}

-- require styles
g_ui.importStyle('textmessage.otui')

-- private variables
local MessageTypes = {
  consoleRed      = { color = '#F55E5E', consoleTab = tr('Default') },
  consoleOrange   = { color = '#FE6500', consoleTab = tr('Default') },
  consoleBlue     = { color = '#9F9DFD', consoleTab = tr('Default') },
  warning         = { color = '#F55E5E', consoleTab = tr('Server Log'), labelId = 'centerWarning' },
  infoDescription = { color = '#00EB00', consoleTab = tr('Server Log'), labelId = 'centerInfo', consoleOption = 'showInfoMessagesInConsole' },
  eventAdvance    = { color = '#FFFFFF', consoleTab = tr('Server Log'), labelId = 'centerAdvance', consoleOption = 'showEventMessagesInConsole' },
  eventDefault    = { color = '#FFFFFF', consoleTab = tr('Server Log'), labelId = 'bottomStatus', consoleOption = 'showEventMessagesInConsole' },
  statusDefault   = { color = '#FFFFFF', consoleTab = tr('Server Log'), labelId = 'bottomStatus', consoleOption = 'showStatusMessagesInConsole' },
  statusSmall     = { color = '#FFFFFF', labelId = 'bottomStatus' },
  private         = { color = '#5FF7F7', labelId = 'centerPrivate' }
}

local centerTextMessagePanel
local bottomStatusLabel
local privateLabel

-- private functions
local function displayMessage(msgtype, msg, time)
  if not g_game.isOnline() then return end

  if msgtype.consoleTab ~= nil then
    if msgtype.consoleOption == nil or Options.getOption(msgtype.consoleOption) then
      Console.addText(msg, msgtype, msgtype.consoleTab)
    end
  end

  if msgtype.labelId then
    local label = GameInterface.getMapPanel():recursiveGetChildById(msgtype.labelId)

    label:setText(msg)
    label:setColor(msgtype.color)

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

local function createTextMessageLabel(id, parent, class)
  local label = g_ui.createWidget(class, parent)
  label:setFont('verdana-11px-rounded')
  label:setId(id)
  return label
end

-- public functions
function TextMessage.init()
  connect(g_game, { onDeath = TextMessage.displayDeadMessage,
                    onTextMessage = TextMessage.display,
                    onGameStart = TextMessage.clearMessages })

  centerTextMessagePanel = g_ui.createWidget('Panel', GameInterface.getMapPanel())
  centerTextMessagePanel:setId('centerTextMessagePanel')

  local layout = UIVerticalLayout.create(centerTextMessagePanel)
  layout:setFitChildren(true)
  centerTextMessagePanel:setLayout(layout)
  centerTextMessagePanel:setWidth(360)
  centerTextMessagePanel:centerIn('parent')

  createTextMessageLabel('centerWarning', centerTextMessagePanel, 'CenterLabel')
  createTextMessageLabel('centerAdvance', centerTextMessagePanel, 'CenterLabel')
  createTextMessageLabel('centerInfo', centerTextMessagePanel, 'CenterLabel')

  privateLabel = createTextMessageLabel('centerPrivate', GameInterface.getMapPanel(), 'TopCenterLabel')
  bottomStatusLabel = createTextMessageLabel('bottomStatus', GameInterface.getMapPanel(), 'BottomLabel')
end

function TextMessage.terminate()
  disconnect(g_game, { onDeath = TextMessage.displayDeadMessage,
                       onTextMessage = TextMessage.display,
                       onGameStart = TextMessage.clearMessages })
  removeEvent(GameInterface.getMapPanel():recursiveGetChildById('centerWarning').hideEvent)
  removeEvent(GameInterface.getMapPanel():recursiveGetChildById('centerAdvance').hideEvent)
  removeEvent(GameInterface.getMapPanel():recursiveGetChildById('centerInfo').hideEvent)
  removeEvent(GameInterface.getMapPanel():recursiveGetChildById('centerPrivate').hideEvent)
  removeEvent(GameInterface.getMapPanel():recursiveGetChildById('bottomStatus').hideEvent)
  centerTextMessagePanel:destroy()
  bottomStatusLabel:destroy()
  privateLabel:destroy()
  centerTextMessagePanel = nil
  bottomStatusLabel = nil
  privateLabel = nil
  TextMessage = nil
end

function TextMessage.clearMessages()
  GameInterface.getMapPanel():recursiveGetChildById('centerWarning'):hide()
  GameInterface.getMapPanel():recursiveGetChildById('centerAdvance'):hide()
  GameInterface.getMapPanel():recursiveGetChildById('centerInfo'):hide()
  GameInterface.getMapPanel():recursiveGetChildById('centerPrivate'):hide()
  GameInterface.getMapPanel():recursiveGetChildById('bottomStatus'):hide()
end

function TextMessage.displayStatus(msg, time)
  displayMessage(MessageTypes.statusSmall, msg)
end

function TextMessage.displayEventAdvance(msg, time)
  displayMessage(MessageTypes.eventAdvance, msg, time)
end

function TextMessage.displayPrivate(msg, time)
  displayMessage(MessageTypes.private, msg, time)
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
