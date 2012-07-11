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
  [1] = { speakTypeDesc = 'whisper', icon = 'icons/whisper.png' },
  [2] = { speakTypeDesc = 'say', icon = 'icons/say.png' },
  [3] = { speakTypeDesc = 'yell', icon = 'icons/yell.png' }
}

local MAX_HISTORY = 1000
local MAX_LINES = 100
local HELP_CHANNEL = 9

local consolePanel
local consoleContentPanel
local consoleTabBar
local consoleTextEdit
local channels
local channelsWindow
local ownPrivateName
local messageHistory = {}
local lastChannels = {}
local currentMessageIndex = 0
local ignoreNpcMessages = false

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
  if ignoreNpcMessages and speaktype == SpeakPrivateNpcToPlayer then return end
  local defaultMessage = speaktype < 3 and true or false
  speaktype = SpeakTypes[speaktype]
  if speaktype.hideInConsole then return end

  local composedMessage = applyMessagePrefixies(name, level, message)

  if speaktype.private then
    Console.addPrivateText(composedMessage, speaktype, name, false, name)
    if Options.getOption('showPrivateMessagesOnScreen') then
	  if(speaktype.speakType ~= SpeakPrivateNpcToPlayer) then
        TextMessage.displayPrivate(name .. ':\n' .. message)
	  end
    end
  else
    local channel = tr('Default')
    if not defaultMessage then
      channel = channels[channelId]
    end

    if channel then
      Console.addText(composedMessage, speaktype, channel, name)
    elseif channelId ~= 0 then
      -- server sent a message on a channel that is not open
      pwarning('message in channel id ' .. channelId .. ' which is unknown, this is a server bug, relogin if you want to see messages in this channel')
    end
  end
end

local function onOpenChannel(channelId, channelName)
  Console.addChannel(channelName, channelId)
end

local function onOpenPrivateChannel(receiver)
  Console.addPrivateChannel(receiver)
end

local function onOpenOwnPrivateChannel(channelId, channelName)
  local privateTab = Console.getTab(channelName)
  if privateTab == nil then
    Console.addChannel(channelName, channelId)
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

local function onGameStart()
  local tab = Console.getTab(tr('Default'))
  if tab then
    addEvent(function() consoleTabBar:selectTab(tab) end, false)
  end
  for _, channelid in pairs(lastChannels) do
    g_game.joinChannel(channelid)
  end
  lastChannels = {}
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

  consolePanel = g_ui.loadUI('console.otui', GameInterface.getBottomPanel())
  consoleTextEdit = consolePanel:getChildById('consoleTextEdit')
  consoleContentPanel = consolePanel:getChildById('consoleContentPanel')
  consoleTabBar = consolePanel:getChildById('consoleTabBar')
  consoleTabBar:setContentWidget(consoleContentPanel)
  channels = {}

  Console.addTab(tr('Default'), true)
  Console.addTab(tr('Server Log'), false)

  g_keyboard.bindKeyPress('Shift+Up', function() navigateMessageHistory(1) end, consolePanel)
  g_keyboard.bindKeyPress('Shift+Down', function() navigateMessageHistory(-1) end, consolePanel)
  g_keyboard.bindKeyPress('Tab', function() consoleTabBar:selectNextTab() end, consolePanel)
  g_keyboard.bindKeyPress('Shift+Tab', function() consoleTabBar:selectPrevTab() end, consolePanel)
  g_keyboard.bindKeyDown('Enter', Console.sendCurrentMessage, consolePanel)
  g_keyboard.bindKeyPress('Ctrl+A', function() consoleTextEdit:clearText() end, consolePanel)

  -- apply buttom functions after loaded
  consolePanel:getChildById('nextChannelButton').onClick = function() consoleTabBar:selectNextTab() end
  consolePanel:getChildById('prevChannelButton').onClick = function() consoleTabBar:selectPrevTab() end
  consoleTabBar.onTabChange = Console.onTabChange

  -- tibia like hotkeys
  g_keyboard.bindKeyDown('Ctrl+O', g_game.requestChannels)
  g_keyboard.bindKeyDown('Ctrl+E', Console.removeCurrentTab)
  g_keyboard.bindKeyDown('Ctrl+H', Console.openHelp)
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
    if tonumber(channelid) and tonumber(channelid) ~= 0 then
      g_game.leaveChannel(channelid)
    end
  end
  channels = {}

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

function Console.clear()
  lastChannels = {}
  for channelid, channelname in pairs(channels) do
    table.insert(lastChannels, tonumber(channelid))
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

function Console.openHelp()
  g_game.joinChannel(HELP_CHANNEL)
end

function Console.addTab(name, focus)
  local tab = Console.getTab(name)
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

function Console.addPrivateChannel(receiver)
  channels[receiver] = receiver
  return Console.addTab(receiver, true)
end

function Console.addPrivateText(text, speaktype, name, isPrivateCommand, creatureName)
  local focus = false
  if speaktype.speakType == SpeakPrivateNpcToPlayer then
    name = 'NPCs'
    focus = true
  end

  local privateTab = Console.getTab(name)
  if privateTab == nil then
    if (Options.getOption('showPrivateMessagesInConsole') and not focus) or (isPrivateCommand and not privateTab) then
      privateTab = Console.getTab(tr('Default'))
    else
      privateTab = Console.addTab(name, focus)
      channels[name] = name
    end
    privateTab.npcChat = speaktype.npcChat
  elseif focus then
    consoleTabBar:selectTab(privateTab)
  end
  Console.addTabText(text, speaktype, privateTab, creatureName)
end

function Console.addText(text, speaktype, tabName, creatureName)
  local tab = Console.getTab(tabName)
  if tab ~= nil then
    Console.addTabText(text, speaktype, tab, creatureName)
  end
end

function Console.addTabText(text, speaktype, tab, creatureName)
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

  label.onMouseRelease = function (self, mousePos, mouseButton) Console.popupMenu(mousePos, mouseButton, creatureName, text) end

  if consoleBuffer:getChildCount() > MAX_LINES then
    consoleBuffer:getFirstChild():destroy()
  end
end

function Console.popupMenu(mousePos, mouseButton, creatureName, text)
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

      if RuleViolation.hasWindowAccess() then
        menu:addSeparator()
        menu:addOption(tr('Rule Violation'), function() RuleViolation.show(creatureName, text:match('.+%:%s(.+)')) end)
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
  if #messageHistory == 0 or messageHistory[#messageHistory] ~= originalMessage then
    table.insert(messageHistory, originalMessage)
    if #messageHistory > MAX_HISTORY then
      table.remove(messageHistory, 1)
    end
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
    Console.addPrivateText(message, speaktype, name, isPrivateCommand, g_game.getCharacterName())
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

function Console.ignoreNpcMessages(ignore)
  ignoreNpcMessages = ignore
end
