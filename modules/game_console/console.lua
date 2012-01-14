Console = {}

-- private variables
local SpeakTypes = {
  say = { color = '#FFFF00' },
  whisper = { color = '#FFFF00' },
  yell = { color = '#FFFF00' },
  monsterSay = { color = '#FE6500', hideInConsole = true},
  monsterYell = { color = '#FE6500', hideInConsole = true},
  npcToPlayer = { color = '#5FF7F7', private = true, npcChat = true },
  channelYellow = { color = '#FFFF00' },
  channelWhite = { color = '#FFFFFF' },
  channelRed = { color = '#F55E5E' },
  channelOrange = { color = '#FE6500' },
  private = { color = '#5FF7F7', private = true },
  playerToNpc = { color = '#9F9DFD', private = true, npcChat = true },
  broadcast = { color = '#F55E5E', private = true },
  privateRed = { color = '#F55E5E', private = true }
}

local consolePanel
local consoleBuffer
local consoleTabBar
local consoleLineEdit
local channels

-- private functions
function applyMessagePrefixies(name, level, message)
  if name then
    if Options.showLevelsInConsole and level > 0 then
      message = name .. ' [' .. level .. ']: ' .. message
    else
      message = name .. ': ' .. message
    end
  end
  return message
end

-- public functions
function Console.create()
  consolePanel = displayUI('console.otui', { parent = Game.gameBottomPanel } )
  consoleLineEdit = consolePanel:getChildById('consoleLineEdit')
  consoleBuffer = consolePanel:getChildById('consoleBuffer')
  consoleTabBar = consolePanel:getChildById('consoleTabBar')
  consoleTabBar:setContentWidget(consoleBuffer)
  channels = {}

  Console.addChannel('Default', 0)
  Console.addTab('Server Log')

  Hotkeys.bind('Tab', function() consoleTabBar:selectNextTab() end, consolePanel)
  Hotkeys.bind('Shift+Tab', function() consoleTabBar:selectPrevTab() end, consolePanel)
  Hotkeys.bind('Enter', Console.sendCurrentMessage, consolePanel)
  Hotkeys.bind('Return', Console.sendCurrentMessage, consolePanel)
end

function Console.destroy()
  consolePanel:destroy()
  consolePanel = nil
end

function Console.addTab(name)
  local tab = consoleTabBar:addTab(name)
  consoleTabBar:selectTab(tab)
  return tab
end

function Console.getTab(name)
  return consoleTabBar:getTab(name)
end

function Console.getCurrentTab()
  return consoleTabBar:getCurrentTab()
end

function Console.addChannel(name, id)
  channels[id] = name
  local tab = Console.addTab(name)
  tab.channelId = id
  return tab
end

function Console.addPrivateText(text, speaktype, name)
  local privateTab = Console.getTab(name)
  if privateTab == nil then
    if Options.showPrivateMessagesInConsole then
      privateTab = Console.getTab('Default')
    else
      privateTab = Console.addTab(name)
    end
    privateTab.npcChat = speaktype.npcChat
  end
  Console.addTabText(text, speaktype, privateTab)
end

function Console.addText(text, speaktype, tabName)
  local tab = Console.getTab(tabName)
  Console.addTabText(text, speaktype, tab)
end

function Console.addTabText(text, speaktype, tab)
  if Options.showTimestampsInConsole then
    text = os.date('%H:%M') .. ' ' .. text
  end

  local label = createWidget('ConsoleLabel', consoleTabBar:getTabPanel(tab))
  label:setText(text)
  label:setColor(speaktype.color)
  consoleTabBar:blinkTab(tab)
end

function Console.sendCurrentMessage()
  local message = consoleLineEdit:getText()
  if #message == 0 then return end
  consoleLineEdit:clearText()

  local tab = Console.getCurrentTab()
  local name = tab:getText()
  if name == 'Server Log' then name = 'Default' end

  local speaktypedesc = 'say'
  if tab.npcChat then
    speaktypedesc = 'playerToNpc'
  end

  if tab.channelId then
    if tab.channelId ~= 0 then
      speaktypedesc = 'channelYellow'
    end

    Game.talkChannel(speaktypedesc, tab.channelId, message)
    return
  else
    local speaktype = SpeakTypes[speaktypedesc]
    local player = Game.getLocalPlayer()
    message = applyMessagePrefixies(player:getName(), player:getLevel(), message)
    Console.addPrivateText(message, speaktype, name)

    Game.talkPrivate(speaktypedesc, name, message)
  end
end

-- hooked events
local function onCreatureSpeak(name, level, speaktypedesc, message, channelId, creaturePos)
  speaktype = SpeakTypes[speaktypedesc]
  if speaktype.hideInConsole then return end

  message = applyMessagePrefixies(name, level, message)

  if speaktype.private then
    Console.addPrivateText(message, speaktype, name)
  else
    Console.addText(message, speaktype, channels[channelId])
  end
end

local function onOpenChannel(channelId, channelName)
  Console.addChannel(channelName, channelId)
end

local function onChannelList(channelList)
  local channelsWindow = displayUI('channelswindow.otui')
  local channelListPanel = channelsWindow:getChildById('channelList')
  channelsWindow.onEnter = function(self)
    local selectedChannelLabel = channelListPanel:getFocusedChild()
    if not selectedChannelLabel then return end
    --Game.joinChannel(selectedChannelLabel.channelId)
    --Console.addChannel(selectedChannelLabel:getText(), selectedChannelLabel .channelId)
    channelsWindow:destroy()
  end
  for k,v in pairs(channelList) do
    local channelId = v[1]
    local channelName = v[2]

    if channelId ~= 0 and #channelName > 0 then
      local label = createWidget('ChannelListLabel', channelListPanel)
      print(channelId, channelName)
      label.channelId = channelId
      label:setText(channelName)
    end
  end
end

connect(Game, { onLogin = Console.create,
                onLogout = Console.destroy,
                onCreatureSpeak = onCreatureSpeak,
                onChannelList = onChannelList,
                onOpenChannel = onOpenChannel})