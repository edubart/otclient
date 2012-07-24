MessageTypes = {
  ConsoleRed      = { color = '#F55E5E', consoleTab = tr('Default') },
  ConsoleOrange   = { color = '#FE6500', consoleTab = tr('Default') },
  ConsoleBlue     = { color = '#9F9DFD', consoleTab = tr('Default') },
  Warning         = { color = '#F55E5E', consoleTab = tr('Server Log'), labelId = 'warningLabel' },
  Info            = { color = '#00EB00', consoleTab = tr('Server Log'), labelId = 'infoLabel', consoleOption = 'showInfoMessagesInConsole' },
  EventAdvance    = { color = '#FFFFFF', consoleTab = tr('Server Log'), labelId = 'advanceLabel', consoleOption = 'showEventMessagesInConsole' },
  EventDefault    = { color = '#FFFFFF', consoleTab = tr('Server Log'), labelId = 'statusLabel', consoleOption = 'showEventMessagesInConsole' },
  StatusDefault   = { color = '#FFFFFF', consoleTab = tr('Server Log'), labelId = 'statusLabel', consoleOption = 'showStatusMessagesInConsole' },
  StatusSmall     = { color = '#FFFFFF', labelId = 'statusLabel' },
  Private         = { color = '#5FF7F7', labelId = 'privateLabel' }
}

centerTextMessagePanel = nil
statusLabel = nil
privateLabel = nil
warningLabel = nil
advanceLabel = nil
infoLabel = nil

function init()
  connect(g_game, {
    onTextMessage = displayMessage,
    onGameStart = clearMessages
  })
  registerProtocol()

  g_ui.importStyle('textmessage.otui')
  centerTextMessagePanel = g_ui.createWidget('Panel', modules.game_interface.getMapPanel())
  centerTextMessagePanel:setId('centerTextMessagePanel')

  local layout = UIVerticalLayout.create(centerTextMessagePanel)
  layout:setFitChildren(true)
  centerTextMessagePanel:setLayout(layout)
  centerTextMessagePanel:setWidth(360)
  centerTextMessagePanel:centerIn('parent')

  warningLabel = createTextMessageLabel('warningLabel', centerTextMessagePanel, 'CenterLabel')
  advanceLabel = createTextMessageLabel('advanceLabel', centerTextMessagePanel, 'CenterLabel')
  infoLabel = createTextMessageLabel('infoLabel', centerTextMessagePanel, 'CenterLabel')
  privateLabel = createTextMessageLabel('privateLabel', modules.game_interface.getMapPanel(), 'TopCenterLabel')
  statusLabel = createTextMessageLabel('statusLabel', modules.game_interface.getMapPanel(), 'BottomLabel')
end

function terminate()
  disconnect(g_game, {
    onTextMessage = display,
    onGameStart = clearMessages
  })
  unregisterProtocol()

  removeEvent(warningLabel.hideEvent)
  removeEvent(advanceLabel.hideEvent)
  removeEvent(infoLabel.hideEvent)
  removeEvent(privateLabel.hideEvent)
  removeEvent(statusLabel.hideEvent)

  centerTextMessagePanel:destroy()
  statusLabel:destroy()
  privateLabel:destroy()
end

function clearMessages()
  warningLabel:hide()
  advanceLabel:hide()
  infoLabel:hide()
  privateLabel:hide()
  statusLabel:hide()
end

function createTextMessageLabel(id, parent, class)
  local label = g_ui.createWidget(class, parent)
  label:setFont('verdana-11px-rounded')
  label:setId(id)
  return label
end

function displayMessage(msgtype, msg, time)
  if not g_game.isOnline() then return end
  msgtype = MessageTypes[msgtype]

  if msgtype.consoleTab ~= nil then
    if msgtype.consoleOption == nil or Options.getOption(msgtype.consoleOption) then
      modules.game_console.addText(msg, msgtype, msgtype.consoleTab)
    end
  end

  if msgtype.labelId then
    local label = modules.game_interface.getMapPanel():recursiveGetChildById(msgtype.labelId)

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

function displayStatus(msg, time)
  displayMessage('StatusSmall', msg)
end

function displayEventAdvance(msg, time)
  displayMessage('EventAdvance', msg, time)
end

function displayPrivate(msg, time)
  displayMessage('Private', time)
end
