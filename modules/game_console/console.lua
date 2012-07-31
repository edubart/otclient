SpeakTypesSettings = {
  none = {},
  say = { speakType = MessageModes.Say, color = '#FFFF00' },
  whisper = { speakType = MessageModes.Whisper, color = '#FFFF00' },
  yell = { speakType = MessageModes.Yell, color = '#FFFF00' },
  broadcast = { speakType = MessageModes.GamemasterPrivateFrom, color = '#F55E5E' },
  private = { speakType = MessageModes.PrivateTo, color = '#5FF7F7', private = true },
  privateRed = { speakType = MessageModes.GamemasterTo, color = '#F55E5E', private = true },
  privatePlayerToPlayer = { speakType = MessageModes.PrivateTo, color = '#9F9DFD', private = true },
  privatePlayerToNpc = { speakType = MessageModes.NpcTo, color = '#9F9DFD', private = true, npcChat = true },
  privateNpcToPlayer = { speakType = MessageModes.NpcFrom, color = '#5FF7F7', private = true, npcChat = true },
  channelYellow = { speakType = MessageModes.Channel, color = '#FFFF00' },
  channelWhite = { speakType = MessageModes.ChannelManagement, color = '#FFFFFF' },
  channelRed = { speakType = MessageModes.GamemasterChannel, color = '#F55E5E' },
  channelOrange = { speakType = MessageModes.ChannelHighlight, color = '#FE6500' },
  monsterSay = { speakType = MessageModes.MonsterSay, color = '#FE6500', hideInConsole = true},
  monsterYell = { speakType = MessageModes.MonsterYell, color = '#FE6500', hideInConsole = true},
}

SpeakTypes = {
  [MessageModes.Say] = SpeakTypesSettings.say,
  [MessageModes.Whisper] = SpeakTypesSettings.whisper,
  [MessageModes.Yell] = SpeakTypesSettings.yell,
  [MessageModes.GamemasterBroadcast] = SpeakTypesSettings.broadcast,
  [MessageModes.PrivateFrom] = SpeakTypesSettings.private,
  [MessageModes.GamemasterPrivateFrom] = SpeakTypesSettings.privateRed,
  [MessageModes.NpcTo] = SpeakTypesSettings.privatePlayerToNpc,
  [MessageModes.NpcFrom] = SpeakTypesSettings.privateNpcToPlayer,
  [MessageModes.Channel] = SpeakTypesSettings.channelYellow,
  [MessageModes.ChannelManagement] = SpeakTypesSettings.channelWhite,
  [MessageModes.GamemasterChannel] = SpeakTypesSettings.channelRed,
  [MessageModes.ChannelHighlight] = SpeakTypesSettings.channelOrange,
  [MessageModes.MonsterSay] = SpeakTypesSettings.monsterSay,
  [MessageModes.MonsterYell] = SpeakTypesSettings.monsterYell,

  -- ignored types
  [MessageModes.Spell] = SpeakTypesSettings.none,
  [MessageModes.BarkLow] = SpeakTypesSettings.none,
  [MessageModes.BarkLoud] = SpeakTypesSettings.none,
}

SayModes = {
  [1] = { speakTypeDesc = 'whisper', icon = 'icons/whisper.png' },
  [2] = { speakTypeDesc = 'say', icon = 'icons/say.png' },
  [3] = { speakTypeDesc = 'yell', icon = 'icons/yell.png' }
}

MAX_HISTORY = 1000
MAX_LINES = 100
HELP_CHANNEL = 9

consolePanel = nil
consoleContentPanel = nil
consoleTabBar = nil
consoleTextEdit = nil
channels = nil
channelsWindow = nil
ownPrivateName = nil
messageHistory = {}
currentMessageIndex = 0
ignoreNpcMessages = false


function init()
  connect(g_game, { onTalk = onTalk,
                    onChannelList = onChannelList,
                    onOpenChannel = onOpenChannel,
                    onOpenPrivateChannel = onOpenPrivateChannel,
                    onOpenOwnPrivateChannel = onOpenOwnPrivateChannel,
                    onCloseChannel = onCloseChannel,
                    onGameStart = onGameStart,
                    onGameEnd = clear })

  consolePanel = g_ui.loadUI('console.otui', modules.game_interface.getBottomPanel())
  consoleTextEdit = consolePanel:getChildById('consoleTextEdit')
  consoleContentPanel = consolePanel:getChildById('consoleContentPanel')
  consoleTabBar = consolePanel:getChildById('consoleTabBar')
  consoleTabBar:setContentWidget(consoleContentPanel)
  channels = {}

  addTab(tr('Default'), true)
  addTab(tr('Server Log'), false)

  g_keyboard.bindKeyPress('Shift+Up', function() navigateMessageHistory(1) end, consolePanel)
  g_keyboard.bindKeyPress('Shift+Down', function() navigateMessageHistory(-1) end, consolePanel)
  g_keyboard.bindKeyPress('Tab', function() consoleTabBar:selectNextTab() end, consolePanel)
  g_keyboard.bindKeyPress('Shift+Tab', function() consoleTabBar:selectPrevTab() end, consolePanel)
  g_keyboard.bindKeyDown('Enter', sendCurrentMessage, consolePanel)
  g_keyboard.bindKeyPress('Ctrl+A', function() consoleTextEdit:clearText() end, consolePanel)

  -- apply buttom functions after loaded
  consolePanel:getChildById('nextChannelButton').onClick = function() consoleTabBar:selectNextTab() end
  consolePanel:getChildById('prevChannelButton').onClick = function() consoleTabBar:selectPrevTab() end
  consoleTabBar.onTabChange = onTabChange

  -- tibia like hotkeys
  g_keyboard.bindKeyDown('Ctrl+O', g_game.requestChannels)
  g_keyboard.bindKeyDown('Ctrl+E', removeCurrentTab)
  g_keyboard.bindKeyDown('Ctrl+H', openHelp)
end

function terminate()
  disconnect(g_game, { onTalk = onTalk,
                       onChannelList = onChannelList,
                       onOpenChannel = onOpenChannel,
                       onOpenPrivateChannel = onOpenPrivateChannel,
                       onOpenOwnPrivateChannel = onOpenPrivateChannel,
                       onCloseChannel = onCloseChannel,
                       onGameStart = onGameStart,
                       onGameEnd = clear })

  if g_game.isOnline() then clear() end

  g_keyboard.unbindKeyDown('Ctrl+O')
  g_keyboard.unbindKeyDown('Ctrl+E')
  g_keyboard.unbindKeyDown('Ctrl+H')

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

function onTabChange(tabBar, tab)
  if tab:getText() == tr('Default') or tab:getText() == tr('Server Log') then
    consolePanel:getChildById('closeChannelButton'):disable()
  else
    consolePanel:getChildById('closeChannelButton'):enable()
  end
end

function clear()
  -- save last open channels
  local lastChannelsOpen = g_settings.getNode('lastChannelsOpen') or {}
  local char = g_game.getLocalPlayer():getName()
  local savedChannels = {}
  local set = false
  for channelId, channelName in pairs(channels) do
    if type(channelId) == 'number' then
      savedChannels[channelName] = channelId
      set = true
    end
  end
  if set then
    lastChannelsOpen[char] = savedChannels
  else
    lastChannelsOpen[char] = nil
  end
  g_settings.setNode('lastChannelsOpen', lastChannelsOpen)

  -- close channels
  for _, channelName in pairs(channels) do
    local tab = consoleTabBar:getTab(channelName)
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

function setTextEditText(text)
  consoleTextEdit:setText(text)
end

function openHelp()
  g_game.joinChannel(HELP_CHANNEL)
end

function addTab(name, focus)
  local tab = getTab(name)
  if(tab) then -- is channel already open
    if(not focus) then focus = true end
  else
    tab = consoleTabBar:addTab(name)
  end
  if focus then
    consoleTabBar:selectTab(tab)
  elseif name ~= tr('Server Log') then
    consoleTabBar:blinkTab(tab)
  end
  return tab
end

function removeCurrentTab()
  local tab = consoleTabBar:getCurrentTab()
  if tab:getText() == tr('Default') or tab:getText() == tr('Server Log') then return end

  -- notificate the server that we are leaving the channel
  if tab.channelId then
    for k, v in pairs(channels) do
      if (k == tab.channelId) then channels[k] = nil end
    end
    g_game.leaveChannel(tab.channelId)
  elseif tab:getText() == "NPCs" then
    g_game.closeNpcChannel()
  end

  consoleTabBar:removeTab(tab)
end

function getTab(name)
  return consoleTabBar:getTab(name)
end

function getCurrentTab()
  return consoleTabBar:getCurrentTab()
end

function addChannel(name, id)
  channels[id] = name
  local tab = addTab(name, true)
  tab.channelId = id
  return tab
end

function addPrivateChannel(receiver)
  channels[receiver] = receiver
  return addTab(receiver, true)
end

function addPrivateText(text, speaktype, name, isPrivateCommand, creatureName)
  local focus = false
  if speaktype.npcChat then
    name = 'NPCs'
    focus = true
  end

  local privateTab = getTab(name)
  if privateTab == nil then
    if (Options.getOption('showPrivateMessagesInConsole') and not focus) or (isPrivateCommand and not privateTab) then
      privateTab = getTab(tr('Default'))
    else
      privateTab = addTab(name, focus)
      channels[name] = name
    end
    privateTab.npcChat = speaktype.npcChat
  elseif focus then
    consoleTabBar:selectTab(privateTab)
  end
  addTabText(text, speaktype, privateTab, creatureName)
end

function addText(text, speaktype, tabName, creatureName)
  local tab = getTab(tabName)
  if tab ~= nil then
    addTabText(text, speaktype, tab, creatureName)
  end
end

function addTabText(text, speaktype, tab, creatureName)
  if Options.getOption('showTimestampsInConsole') then
    text = os.date('%H:%M') .. ' ' .. text
  end

  local panel = consoleTabBar:getTabPanel(tab)
  local consoleBuffer = panel:getChildById('consoleBuffer')
  local label = g_ui.createWidget('ConsoleLabel', consoleBuffer)
  label:setId('consoleLabel' .. panel:getChildCount())
  label:setText(text)
  label:setColor(speaktype.color)
  consoleTabBar:blinkTab(tab)

  label.onMouseRelease = function (self, mousePos, mouseButton) popupMenu(mousePos, mouseButton, creatureName, text) end

  if consoleBuffer:getChildCount() > MAX_LINES then
    consoleBuffer:getFirstChild():destroy()
  end
end

function popupMenu(mousePos, mouseButton, creatureName, text)
  if mouseButton == MouseRightButton then
    local menu = g_ui.createWidget('PopupMenu')
    if creatureName then
      if creatureName ~= g_game.getCharacterName() then
        menu:addOption(tr('Message to ' .. creatureName), function () g_game.openPrivateChannel(creatureName) end)
        if (not Player:hasVip(creatureName)) then
          menu:addOption(tr('Add to VIP list'), function () g_game.addVip(creatureName) end)
        end
        -- TODO ignore creatureName
        menu:addSeparator()
      end
      --TODO select all
      menu:addOption(tr('Copy message'), function () g_window.setClipboardText(text) end)

      if modules.game_ruleviolation.hasWindowAccess() then
        menu:addSeparator()
        menu:addOption(tr('Rule Violation'), function() modules.game_ruleviolation.show(creatureName, text:match('.+%:%s(.+)')) end)
      end

      menu:addSeparator()
      menu:addOption(tr('Copy name'), function () g_window.setClipboardText(creatureName) end)
    else
      --TODO select all
      menu:addOption(tr('Copy message'), function () g_window.setClipboardText(text) end)
    end
    menu:display(mousePos)
  end
end

function sendCurrentMessage()
  local message = consoleTextEdit:getText()
  if #message == 0 then return end
  consoleTextEdit:clearText()

  -- get current channel
  local tab = getCurrentTab()

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
  if #messageHistory == 0 or messageHistory[#messageHistory] ~= originalMessage then
    table.insert(messageHistory, originalMessage)
    if #messageHistory > MAX_HISTORY then
      table.remove(messageHistory, 1)
    end
  end

  -- when talking on server log, the message goes to default channel
  local name = tab:getText()
  if name == tr('Server Log') then
    tab = getTab(tr('Default'))
    name = tr('Default')
  end

  local speaktypedesc
  if (tab.channelId or name == tr('Default')) and not chatCommandPrivateReady then
    if name == tr('Default') then
      speaktypedesc = chatCommandSayMode or SayModes[consolePanel:getChildById('sayModeButton').sayMode].speakTypeDesc
      if speaktypedesc ~= 'say' then sayModeChange(2) end -- head back to say mode
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
    addPrivateText(message, speaktype, name, isPrivateCommand, g_game.getCharacterName())
  end
end

function sayModeChange(sayMode)
  local buttom = consolePanel:getChildById('sayModeButton')
  if sayMode == nil then
    sayMode = buttom.sayMode + 1
  end

  if sayMode > #SayModes then sayMode = 1 end

  buttom:setIcon(SayModes[sayMode].icon)
  buttom.sayMode = sayMode
end

function getOwnPrivateTab()
  if not ownPrivateName then return end
  return getTab(ownPrivateName)
end

function setIgnoreNpcMessages(ignore)
  ignoreNpcMessages = ignore
end


function navigateMessageHistory(step)
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

function onTalk(name, level, mode, message, channelId, creaturePos)
  if ignoreNpcMessages and mode == MessageModes.NpcFrom then return end
  local defaultMessage = mode < 3 and true or false
  speaktype = SpeakTypes[mode]

  if not speaktype then
    perror('unhandled onTalk message mode ' .. mode .. ': ' .. message)
    return
  end

  if speaktype == SpeakTypesSettings.none then return end

  if speaktype.hideInConsole then return end

  local composedMessage = applyMessagePrefixies(name, level, message)

  if speaktype.private then
    addPrivateText(composedMessage, speaktype, name, false, name)
    if Options.getOption('showPrivateMessagesOnScreen') and speaktype ~= SpeakTypesSettings.privateNpcToPlayer then
      modules.game_textmessage.displayPrivateMessage(name .. ':\n' .. message)
    end
  else
    local channel = tr('Default')
    if not defaultMessage then
      channel = channels[channelId]
    end

    if channel then
      addText(composedMessage, speaktype, channel, name)
    elseif channelId ~= 0 then
      -- server sent a message on a channel that is not open
      pwarning('message in channel id ' .. channelId .. ' which is unknown, this is a server bug, relogin if you want to see messages in this channel')
    end
  end
end

function onOpenChannel(channelId, channelName)
  addChannel(channelName, channelId)
end

function onOpenPrivateChannel(receiver)
  addPrivateChannel(receiver)
end

function onOpenOwnPrivateChannel(channelId, channelName)
  local privateTab = getTab(channelName)
  if privateTab == nil then
    addChannel(channelName, channelId)
  end
  ownPrivateName = channelName
end

function onCloseChannel(channelId)
  local channel = channels[channelId]
  if channel then
    local tab = getTab(channel)
    if tab then
      consoleTabBar:removeTab(tab)
    end
    for k, v in pairs(channels) do
      if (k == tab.channelId) then channels[k] = nil end
    end
  end
end

function doChannelListSubmit()
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

function onChannelList(channelList)
  if channelsWindow then channelsWindow:destroy() end
  channelsWindow = g_ui.displayUI('channelswindow.otui')
  local channelListPanel = channelsWindow:getChildById('channelList')
  channelsWindow.onEnter = doChannelListSubmit
  channelsWindow.onDestroy = function() channelsWindow = nil end
  g_keyboard.bindKeyPress('Down', function() channelListPanel:focusNextChild(KeyboardFocusReason) end, channelsWindow)
  g_keyboard.bindKeyPress('Up', function() channelListPanel:focusPreviousChild(KeyboardFocusReason) end, channelsWindow)

  for k,v in pairs(channelList) do
    local channelId = v[1]
    local channelName = v[2]

    if #channelName > 0 then
      local label = g_ui.createWidget('ChannelListLabel', channelListPanel)
      label.channelId = channelId
      label:setText(channelName)

      label:setPhantom(false)
      label.onDoubleClick = doChannelListSubmit
    end
  end
end

function onGameStart()
  -- open last channels
  local lastChannelsOpen = g_settings.getNode('lastChannelsOpen')
  local savedChannels = lastChannelsOpen[g_game.getLocalPlayer():getName()]
  if savedChannels then
    for channelName, channelId in pairs(savedChannels) do
      channelId = tonumber(channelId)
      if channelId ~= 0 then
        if not table.find(channels, channelId) then
          g_game.joinChannel(channelId)
        end
      end
    end
  end

  local tab = getTab(tr('Default'))
  if tab then
    --[[
      Known Issue: The server is calling to open channels after
      onGameStart is executed causing it to focus the last tab opened.

      Fix: Don't save channels to the settings that are opened by the server.
    ]]
    addEvent(function() consoleTabBar:selectTab(tab) end, true)
  end
end

