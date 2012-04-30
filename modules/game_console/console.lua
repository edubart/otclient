Console = {}

-- private variables
local SpeakTypesSettings = {
  say = { speakType = SpeakSay, color = '#FFFF00' },
  whisper = { speakType = SpeakWhisper, color = '#FFFF00' },
  yell = { speakType = SpeakYell, color = '#FFFF00' },
  broadcast = { speakType = SpeakBroadcast, color = '#F55E5E' },
  private = { speakType = SpeakPrivate, color = '#5FF7F7', private = true },
  privateRed = { speakType = SpeakPrivateRed, color = '#F55E5E', private = true },
  privatePlayerToPlayer = { speakType = SpeakPrivate, color = '#9F9DFD', private = true },
  privatePlayerToNpc = { speakType = SpeakPrivatePlayerToNpc, color = '#9F9DFD', private = true, npcChat = true },
  privateNpcToPlayer = { speakType = SpeakPrivateNpcToPlayer, color = '#5FF7F7', private = true, npcChat = true },
  channelYellow = { speakType = SpeakChannelYellow, color = '#FFFF00' },
  channelWhite = { speakType = SpeakChannelWhite, color = '#FFFFFF' },
  channelRed = { speakType = SpeakChannelRed, color = '#F55E5E' },
  channelOrange = { speakType = SpeakChannelOrange, color = '#FE6500' },
  monsterSay = { speakType = SpeakMonsterSay, color = '#FE6500', hideInConsole = true},
  monsterYell = { speakType = SpeakMonsterYell, color = '#FE6500', hideInConsole = true},
}

local SpeakTypes = {
  [SpeakSay] = SpeakTypesSettings.say,
  [SpeakWhisper] = SpeakTypesSettings.whisper,
  [SpeakYell] = SpeakTypesSettings.yell,
  [SpeakBroadcast] = SpeakTypesSettings.broadcast,
  [SpeakPrivate] = SpeakTypesSettings.private,
  [SpeakPrivateRed] = SpeakTypesSettings.privateRed,
  [SpeakPrivatePlayerToNpc] = SpeakTypesSettings.privatePlayerToNpc,
  [SpeakPrivateNpcToPlayer] = SpeakTypesSettings.privateNpcToPlayer,
  [SpeakChannelYellow] = SpeakTypesSettings.channelYellow,
  [SpeakChannelWhite] = SpeakTypesSettings.channelWhite,
  [SpeakChannelRed] = SpeakTypesSettings.channelRed,
  [SpeakChannelOrange] = SpeakTypesSettings.channelOrange,
  [SpeakMonsterSay] = SpeakTypesSettings.monsterSay,
  [SpeakMonsterYell] = SpeakTypesSettings.monsterYell,
}

local SayModes = {
  [1] = { speakTypeDesc = 'whisper', icon = '/core_styles/icons/whisper.png' },
  [2] = { speakTypeDesc = 'say', icon = '/core_styles/icons/say.png' },
  [3] = { speakTypeDesc = 'yell', icon = '/core_styles/icons/yell.png' }
}

local MAX_HISTORY = 1000
local MAX_LINES = 100

local consolePanel
local consoleContentPanel
local consoleTabBar
local consoleTextEdit
local channels
local channelsWindow
local ownPrivateName
local messageHistory = {}
local currentMessageIndex = 0

-- private functions
local function navigateMessageHistory(step)
  local numCommands = #messageHistory
  if numCommands > 0 then
    currentMessageIndex = math.min(math.max(currentMessageIndex + step, 0), numCommands)
    if currentMessageIndex > 0 then
      local command = messageHistory[numCommands - currentMessageIndex + 1]
      consoleTextEdit:setText(command)
    else
      consoleTextEdit:clearText()
    end
  end
end

function applyMessagePrefixies(name, level, message)
  if name then
    if Options.getOption('showLevelsInConsole') and level > 0 then
      message = name .. ' [' .. level .. ']: ' .. message
    else
      message = name .. ': ' .. message
    end
  end
  return message
end

-- hooked events
local function onCreatureSpeak(name, level, speaktype, message, channelId, creaturePos)
  local defaultMessage = speaktype < 3 and true or false
  speaktype = SpeakTypes[speaktype]
  if speaktype.hideInConsole then return end

  message = applyMessagePrefixies(name, level, message)

  if speaktype.private then
    Console.addPrivateText(message, speaktype, name, false)
  else
    local channel = tr('Default')
    if not defaultMessage then
      channel = channels[channelId]
    end

    if channel then
      Console.addText(message, speaktype, channel)
    elseif channelId ~= 0 then
      -- server sent a message on a channel that is not open
      warning('message in channel id ' .. channelId .. ' which is unknown, this is a server bug, relogin if you want to see messages in this channel')
    end
  end
end

local function onOpenChannel(channelId, channelName)
  Console.addChannel(channelName, channelId)
end

local function onOpenPrivateChannel(receiver)
  local privateTab = Console.getTab(receiver)
  if privateTab == nil then
    channels[receiver] = receiver
    Console.addTab(receiver, true)
  end
end

local function onOpenOwnPrivateChannel(channelId, channelName)
  local privateTab = Console.getTab(channelName)
  if privateTab == nil then
    --channels[channelId] = channelName (this should be tested)
    Console.addChannel(channelName, channelId, true)
  end
  ownPrivateName = channelName
end

local function onCloseChannel(channelId)
  local channel = channels[channelId]
  if channel then
    local tab = Console.getTab(channel)
    if tab then
      consoleTabBar:removeTab(tab)
    end
  end
end

local function doChannelListSubmit()
  local channelListPanel = channelsWindow:getChildById('channelList')
  local openPrivateChannelWith = channelsWindow:getChildById('openPrivateChannelWith'):getText()
  if openPrivateChannelWith ~= '' then
    g_game.openPrivateChannel(openPrivateChannelWith)
  else
    local selectedChannelLabel = channelListPanel:getFocusedChild()
    if not selectedChannelLabel then return end
    if selectedChannelLabel.channelId == 0xFFFF then
      g_game.openOwnChannel()
    else
      g_game.joinChannel(selectedChannelLabel.channelId)
    end
  end

  channelsWindow:destroy()
end

local function onChannelList(channelList)
  if channelsWindow then channelsWindow:destroy() end
  channelsWindow = displayUI('channelswindow.otui')
  local channelListPanel = channelsWindow:getChildById('channelList')
  channelsWindow.onEnter = doChannelListSubmit
  channelsWindow.onDestroy = function() channelsWindow = nil end
  Keyboard.bindKeyPress('Down', function() channelListPanel:focusNextChild(KeyboardFocusReason) end, channelsWindow)
  Keyboard.bindKeyPress('Up', function() channelListPanel:focusPreviousChild(KeyboardFocusReason) end, channelsWindow)

  for k,v in pairs(channelList) do
    local channelId = v[1]
    local channelName = v[2]

    if #channelName > 0 then
      local label = createWidget('ChannelListLabel', channelListPanel)
      label.channelId = channelId
      label:setText(channelName)

      label:setPhantom(false)
      label.onDoubleClick = doChannelListSubmit
    end
  end
end

local function onGameStart()
  local tab = Console.getTab(tr('Default'))
  if tab then
    addEvent(function() consoleTabBar:selectTab(tab) end, false)
  end
end

-- public functions
function Console.init()
  connect(g_game, { onCreatureSpeak = onCreatureSpeak,
                    onChannelList = onChannelList,
                    onOpenChannel = onOpenChannel,
                    onOpenPrivateChannel = onOpenPrivateChannel,
                    onOpenOwnPrivateChannel = onOpenOwnPrivateChannel,
                    onCloseChannel = onCloseChannel,
                    onGameStart = onGameStart,
                    onGameEnd = Console.clear })

  consolePanel = displayUI('console.otui', GameInterface.getBottomPanel())
  consoleTextEdit = consolePanel:getChildById('consoleTextEdit')
  consoleContentPanel = consolePanel:getChildById('consoleContentPanel')
  consoleTabBar = consolePanel:getChildById('consoleTabBar')
  consoleTabBar:setContentWidget(consoleContentPanel)
  channels = {}

  Console.addTab(tr('Default'), true)
  Console.addTab(tr('Server Log'), false)

  Keyboard.bindKeyPress('Shift+Up', function() navigateMessageHistory(1) end, consolePanel)
  Keyboard.bindKeyPress('Shift+Down', function() navigateMessageHistory(-1) end, consolePanel)
  Keyboard.bindKeyPress('Tab', function() consoleTabBar:selectNextTab() end, consolePanel)
  Keyboard.bindKeyPress('Shift+Tab', function() consoleTabBar:selectPrevTab() end, consolePanel)
  Keyboard.bindKeyDown('Enter', Console.sendCurrentMessage, consolePanel)
  Keyboard.bindKeyPress('Ctrl+A', function() consoleTextEdit:clearText() end, consolePanel)

  -- apply buttom functions after loaded
  consolePanel:getChildById('nextChannelButton').onClick = function() consoleTabBar:selectNextTab() end
  consolePanel:getChildById('prevChannelButton').onClick = function() consoleTabBar:selectPrevTab() end
  consoleTabBar.onTabChange = Console.onTabChange

  -- tibia like hotkeys
  Keyboard.bindKeyDown('Ctrl+O', g_game.requestChannels)
  Keyboard.bindKeyDown('Ctrl+E', Console.removeCurrentTab)
end

function Console.terminate()
  disconnect(g_game, { onCreatureSpeak = onCreatureSpeak,
                       onChannelList = onChannelList,
                       onOpenChannel = onOpenChannel,
                       onOpenPrivateChannel = onOpenPrivateChannel,
                       onOpenOwnPrivateChannel = onOpenPrivateChannel,
                       onCloseChannel = onCloseChannel,
                       onGameStart = onGameStart,
                       onGameEnd = Console.clear })

  for channelid, channelname in pairs(channels) do
    if channelid ~= 0 then
      g_game.leaveChannel(channelid)
    end
  end
  channels = {}

  Keyboard.unbindKeyDown('Ctrl+O')
  Keyboard.unbindKeyDown('Ctrl+E')

  if channelsWindow then
    channelsWindow:destroy()
    channelsWindow = nil
  end

  consolePanel:destroy()
  consolePanel = nil
  consoleTextEdit = nil
  consoleContentPanel = nil
  consoleTabBar = nil

  ownPrivateName = nil

  Console = nil
end

function Console.clear()
  for channelid, channelname in pairs(channels) do
    local tab = consoleTabBar:getTab(channelname)
    consoleTabBar:removeTab(tab)
  end
  
  channels = {}

  consoleTabBar:getTab(tr('Default')).tabPanel:getChildById('consoleBuffer'):destroyChildren()
  consoleTabBar:getTab(tr('Server Log')).tabPanel:getChildById('consoleBuffer'):destroyChildren()

  local npcTab = consoleTabBar:getTab('NPCs')
  if npcTab then
    consoleTabBar:removeTab(npcTab)
  end

  consoleTextEdit:clearText()

  if channelsWindow then
    channelsWindow:destroy()
    channelsWindow = nil
  end
end

function Console.setTextEditText(text)
  consoleTextEdit:setText(text)
end

function Console.addTab(name, focus)
  local tab = consoleTabBar:addTab(name)
  if focus then
    consoleTabBar:selectTab(tab)
  elseif name ~= tr('Server Log') then
    consoleTabBar:blinkTab(tab)
  end
  return tab
end

function Console.onTabChange(tabBar, tab)
  if tab:getText() == tr('Default') or tab:getText() == tr('Server Log') then
    consolePanel:getChildById('closeChannelButton'):disable()
  else
    consolePanel:getChildById('closeChannelButton'):enable()
  end
end

function Console.removeCurrentTab()
  local tab = consoleTabBar:getCurrentTab()
  if tab:getText() == tr('Default') or tab:getText() == tr('Server Log') then return end

  -- notificate the server that we are leaving the channel
  if tab.channelId then
    g_game.leaveChannel(tab.channelId)
  elseif tab:getText() == "NPCs" then
    g_game.closeNpcChannel()
  end

  consoleTabBar:removeTab(tab)
end

function Console.getTab(name)
  return consoleTabBar:getTab(name)
end

function Console.getCurrentTab()
  return consoleTabBar:getCurrentTab()
end

function Console.addChannel(name, id)
  channels[id] = name
  local tab = Console.addTab(name, true)
  tab.channelId = id
  return tab
end

function Console.addPrivateText(text, speaktype, name, isPrivateCommand)
  local focus = false
  if speaktype.speakType == SpeakPrivateNpcToPlayer then
    name = 'NPCs'
    focus = true
  end

  local privateTab = Console.getTab(name)
  if privateTab == nil then
    if Options['showPrivateMessagesInConsole'] or (isPrivateCommand and not privateTab) then
      privateTab = Console.getTab(tr('Default'))
    else
      privateTab = Console.addTab(name, focus)
    end
    privateTab.npcChat = speaktype.npcChat
  elseif focus then
    consoleTabBar:selectTab(privateTab)
  end
  Console.addTabText(text, speaktype, privateTab)
end

function Console.addText(text, speaktype, tabName)
  local tab = Console.getTab(tabName)
  if tab ~= nil then
    Console.addTabText(text, speaktype, tab)
  end
end

function Console.addTabText(text, speaktype, tab)
  if Options.getOption('showTimestampsInConsole') then
    text = os.date('%H:%M') .. ' ' .. text
  end

  local panel = consoleTabBar:getTabPanel(tab)
  local consoleBuffer = panel:getChildById('consoleBuffer')
  local label = createWidget('ConsoleLabel', consoleBuffer)
  label:setId('consoleLabel' .. panel:getChildCount())
  label:setText(text)
  label:setColor(speaktype.color)
  consoleTabBar:blinkTab(tab)

  if consoleBuffer:getChildCount() > MAX_LINES then
    consoleBuffer:getFirstChild():destroy()
  end
end

function Console.sendCurrentMessage()
  local message = consoleTextEdit:getText()
  if #message == 0 then return end
  consoleTextEdit:clearText()

  -- get current channel
  local tab = Console.getCurrentTab()

  -- handling chat commands
  local originalMessage = message
  local chatCommandSayMode
  local chatCommandPrivate
  local chatCommandPrivateReady

  local chatCommandMessage = message:match("^%#y (.*)")
  if chatCommandMessage ~= nil then chatCommandSayMode = 'yell' end -- player used yell command
  message = chatCommandMessage or message

  local chatCommandMessage = message:match("^%#w (.*)")
  if chatCommandMessage ~= nil then chatCommandSayMode = 'whisper' end -- player used whisper
  message = chatCommandMessage or message

  local findIni, findEnd, chatCommandInitial, chatCommandPrivate, chatCommandEnd, chatCommandMessage = message:find("([%*%@])(.+)([%*%@])(.*)")
  if findIni ~= nil and findIni == 1 then -- player used private chat command
    if chatCommandInitial == chatCommandEnd then
      chatCommandPrivateRepeat = false
      if chatCommandInitial == "*" then
        consoleTextEdit:setText('*'.. chatCommandPrivate .. '* ')
      end
      message = chatCommandMessage:trim()
      chatCommandPrivateReady = true
    end
  end

  message = message:gsub("^(%s*)(.*)","%2") -- remove space characters from message init
  if #message == 0 then return end

  -- add new command to history
  currentMessageIndex = 0
  table.insert(messageHistory, originalMessage)
  if #messageHistory > MAX_HISTORY then
    table.remove(messageHistory, 1)
  end

  -- when talking on server log, the message goes to default channel
  local name = tab:getText()
  if name == tr('Server Log') then
    tab = Console.getTab(tr('Default'))
    name = tr('Default')
  end
  
  local speaktypedesc
  if (tab.channelId or name == tr('Default')) and not chatCommandPrivateReady then
    if name == tr('Default') then
      speaktypedesc = chatCommandSayMode or SayModes[consolePanel:getChildById('sayModeButton').sayMode].speakTypeDesc
      if speaktypedesc ~= 'say' then Console.sayModeChange(2) end -- head back to say mode
    else
      speaktypedesc = 'channelYellow'
    end

    g_game.talkChannel(SpeakTypesSettings[speaktypedesc].speakType, tab.channelId, message)
    return
  else
    local isPrivateCommand = false
    if chatCommandPrivateReady then
      speaktypedesc = 'privatePlayerToPlayer'
      name = chatCommandPrivate
      isPrivateCommand = true
    elseif tab.npcChat then
      speaktypedesc = 'privatePlayerToNpc'
    else
      speaktypedesc = 'privatePlayerToPlayer'
    end


    local speaktype = SpeakTypesSettings[speaktypedesc]
    local player = g_game.getLocalPlayer()
    g_game.talkPrivate(speaktype.speakType, name, message)

    message = applyMessagePrefixies(player:getName(), player:getLevel(), message)
    Console.addPrivateText(message, speaktype, name, isPrivateCommand)
  end
end

function Console.sayModeChange(sayMode)
  local buttom = consolePanel:getChildById('sayModeButton')
  if sayMode == nil then
    sayMode = buttom.sayMode + 1
  end

  if sayMode > #SayModes then sayMode = 1 end

  buttom:setIcon(SayModes[sayMode].icon)
  buttom.sayMode = sayMode
end

function Console.getOwnPrivateTab()
  if not ownPrivateName then return end
  return Console.getTab(ownPrivateName)
end
