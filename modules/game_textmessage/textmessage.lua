TextMessage = {}

-- require styles
importStyle 'textmessage.otui'

-- private variables
local MessageTypes = {
  consoleRed      = { color = '#F55E5E', consoleTab = 'Default' },
  consoleOrange   = { color = '#FE6500', consoleTab = 'Default' },
  consoleBlue     = { color = '#9F9DFD', consoleTab = 'Default' },
  warning         = { color = '#F55E5E', consoleTab = 'Server Log', labelId = 'centerWarning', wrap = true },
  infoDescription = { color = '#00EB00', consoleTab = 'Server Log', labelId = 'centerInfo', consoleOption = 'showInfoMessagesInConsole', wrap = true },
  eventAdvance    = { color = '#FFFFFF', consoleTab = 'Server Log', labelId = 'centerAdvance', consoleOption = 'showEventMessagesInConsole', wrap = true },
  eventDefault    = { color = '#FFFFFF', consoleTab = 'Server Log', labelId = 'bottomStatus', consoleOption = 'showEventMessagesInConsole' },
  statusDefault   = { color = '#FFFFFF', consoleTab = 'Server Log', labelId = 'bottomStatus', consoleOption = 'showStatusMessagesInConsole' },
  statusSmall     = { color = '#FFFFFF', labelId = 'bottomStatus' },
}

local centerTextMessagePanel
local centerLabel

-- private functions
local function displayMessage(msgtype, msg, time)
  if not g_game.isOnline() then return end

  if msgtype.consoleTab ~= nil then
    if msgtype.consoleOption == nil or Options[msgtype.consoleOption] then
      Console.addText(msg, msgtype, msgtype.consoleTab)
    end
  end

  if msgtype.labelId  then
    local label = g_game.gameMapPanel:recursiveGetChildById(msgtype.labelId)

    label:setVisible(true)
    label:setText(msg)
    label:setColor(msgtype.color)
    label:resizeToText()

    if msgtype.wrap then
      label:setWidth(label:getParent():getWidth())
      label:wrapText()
      label:setHeight(label:getTextSize().height)
    end

    if not time then
      time = math.max(#msg * 100, 4000)
    else
      time = time * 1000
    end
    removeEvent(label.hideEvent)
    label.hideEvent = scheduleEvent(function() label:setVisible(false) end, time)
  end
end

local function createTextMessageLabel(id, parent)
  local label = createWidget('UILabel', parent)
  label:setFont('verdana-11px-rounded')
  label:setTextAlign(AlignCenter)
  label:setId(id)
  label:setMarginBottom(2)
  label:setVisible(false)
  return label
end

-- public functions

function TextMessage.create()
  centerTextMessagePanel = createWidget('Panel', g_game.gameMapPanel)
  centerTextMessagePanel:setId('centerTextMessagePanel')
  local layout = UIVerticalLayout.create(centerTextMessagePanel)
  layout:setFitChildren(true)
  centerTextMessagePanel:setLayout(layout)
  centerTextMessagePanel:setWidth(360)
  centerTextMessagePanel:centerIn('parent')

  createTextMessageLabel('centerWarning', centerTextMessagePanel)
  createTextMessageLabel('centerAdvance', centerTextMessagePanel)
  createTextMessageLabel('centerInfo', centerTextMessagePanel)

  bottomStatusLabel = createTextMessageLabel('bottomStatus', g_game.gameMapPanel)
  bottomStatusLabel:setHeight(16)
  bottomStatusLabel:addAnchor(AnchorBottom, 'parent', AnchorBottom)
  bottomStatusLabel:addAnchor(AnchorLeft, 'parent', AnchorLeft)
  bottomStatusLabel:addAnchor(AnchorRight, 'parent', AnchorRight)
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
  local advanceLabel = g_game.gameMapPanel:recursiveGetChildById('centerAdvance')
  if advanceLabel:isVisible() then return end
  TextMessage.displayEventAdvance('You are dead.')
end

local function onGameTextMessage(msgtypedesc, msg)
  TextMessage.display(msgtypedesc, msg)
end

connect(g_game, { onGameStart = TextMessage.create,
                onGameEnd = TextMessage.destroy,
                onDeath = onGameDeath,
                onTextMessage = onGameTextMessage })
