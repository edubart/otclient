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
  [1] = { speakTypeDesc = 'whisper', icon = '/images/game/console/whisper' },
  [2] = { speakTypeDesc = 'say', icon = '/images/game/console/say' },
  [3] = { speakTypeDesc = 'yell', icon = '/images/game/console/yell' }
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
ignoreWindow = nil
ownPrivateName = nil
messageHistory = {}
currentMessageIndex = 0
ignoreNpcMessages = false
defaultTab = nil
serverTab = nil

local ignoreSettings = {
    privateMessages = false,
    yelling = false,
    players = {}
}

function init()
  connect(g_game, { onTalk = onTalk,
                    onChannelList = onChannelList,
                    onOpenChannel = onOpenChannel,
                    onOpenPrivateChannel = onOpenPrivateChannel,
                    onOpenOwnPrivateChannel = onOpenOwnPrivateChannel,
                    onCloseChannel = onCloseChannel,
                    onGameStart = onGameStart,
                    onGameEnd = clear })

  consolePanel = g_ui.loadUI('console', modules.game_interface.getBottomPanel())
  consoleTextEdit = consolePanel:getChildById('consoleTextEdit')
  consoleContentPanel = consolePanel:getChildById('consoleContentPanel')
  consoleTabBar = consolePanel:getChildById('consoleTabBar')
  consoleTabBar:setContentWidget(consoleContentPanel)
  consoleTabBar:setTabSpacing(-1)
  channels = {}

  consolePanel.onKeyPress = function(self, keyCode, keyboardModifiers)
    if not (keyboardModifiers == KeyboardCtrlModifier and keyCode == KeyC) then return false end

    local tab = consoleTabBar:getCurrentTab()
    if not tab then return false end

    local consoleBuffer = tab.tabPanel:getChildById('consoleBuffer')
    if not consoleBuffer then return false end

    local consoleLabel = consoleBuffer:getFocusedChild()
    if not consoleLabel or not consoleLabel:hasSelection() then return false end

    g_window.setClipboardText(consoleLabel:getSelection())
    return true
  end

  defaultTab = addTab(tr('Default'), true)
  serverTab = addTab(tr('Server Log'), false)

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
  
  -- Ignore List
  loadIgnoreSettings()
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

  saveIgnoreSettings()
  
  if channelsWindow then
    channelsWindow:destroy()
    channelsWindow = nil
  end
  
  if ignoreWindow then
    ignoreWindow:destroy()
    ignoreWindow = nil
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
  if tab == defaultTab or tab == serverTab then
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

  defaultTab.tabPanel:getChildById('consoleBuffer'):destroyChildren()
  serverTab.tabPanel:getChildById('consoleBuffer'):destroyChildren()

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

function clearChannel(consoleTabBar)
  consoleTabBar:getCurrentTab().tabPanel:getChildById('consoleBuffer'):destroyChildren()
  consoleTabBar:getCurrentTab().tabPanel:getChildById('consoleBufferHighlight'):destroyChildren()
end

function setTextEditText(text)
  consoleTextEdit:setText(text)
  consoleTextEdit:setCursorPos(-1)
end

function openHelp()
  local helpChannel = 9
  if g_game.getProtocolVersion() <= 810 then
    helpChannel = 8
  end
  g_game.joinChannel(helpChannel)
end

function addTab(name, focus)
  local tab = getTab(name)
  if tab then -- is channel already open
    if not focus then focus = true end
  else
    tab = consoleTabBar:addTab(name, nil, processChannelTabMenu)
  end
  if focus then
    consoleTabBar:selectTab(tab)
  elseif not serverTab or name ~= serverTab:getText() then
    consoleTabBar:blinkTab(tab)
  end
  return tab
end

function removeTab(name)
  local tab = consoleTabBar:getTab(name)
  if tab == defaultTab or tab == serverTab then return end

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

function removeCurrentTab()
  local tab = consoleTabBar:getCurrentTab()
  if tab == defaultTab or tab == serverTab then return end

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
    if (modules.client_options.getOption('showPrivateMessagesInConsole') and not focus) or (isPrivateCommand and not privateTab) then
      privateTab = defaultTab
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

-- Contains letter width for font "verdana-11px-antialised" as console is based on it
local letterWidth = {  -- New line (10) and Space (32) have width 1 because they are printed and not replaced with spacer
  [10] = 1, [32] = 1, [33] = 3, [34] = 6, [35] = 8, [36] = 7, [37] = 13, [38] = 9, [39] = 3, [40] = 5, [41] = 5, [42] = 6, [43] = 8, [44] = 4, [45] = 5, [46] = 3, [47] = 8,
  [48] = 7, [49] = 6, [50] = 7, [51] = 7, [52] = 7, [53] = 7, [54] = 7, [55] = 7, [56] = 7, [57] = 7, [58] = 3, [59] = 4, [60] = 8, [61] = 8, [62] = 8, [63] = 6,
  [64] = 10, [65] = 9, [66] = 7, [67] = 7, [68] = 8, [69] = 7, [70] = 7, [71] = 8, [72] = 8, [73] = 5, [74] = 5, [75] = 7, [76] = 7, [77] = 9, [78] = 8, [79] = 8,
  [80] = 7, [81] = 8, [82] = 8, [83] = 7, [84] = 8, [85] = 8, [86] = 8, [87] = 12, [88] = 8, [89] = 8, [90] = 7, [91] = 5, [92] = 8, [93] = 5, [94] = 9, [95] = 8,
  [96] = 5, [97] = 7, [98] = 7, [99] = 6, [100] = 7, [101] = 7, [102] = 5, [103] = 7, [104] = 7, [105] = 3, [106] = 4, [107] = 7, [108] = 3, [109] = 11, [110] = 7,
  [111] = 7, [112] = 7, [113] = 7, [114] = 6, [115] = 6, [116] = 5, [117] = 7, [118] = 8, [119] = 10, [120] = 8, [121] = 8, [122] = 6, [123] = 7, [124] = 4, [125] = 7, [126] = 8,
  [127] = 1, [128] = 7, [129] = 6, [130] = 3, [131] = 7, [132] = 6, [133] = 11, [134] = 7, [135] = 7, [136] = 7, [137] = 13, [138] = 7, [139] = 4, [140] = 11, [141] = 6, [142] = 6,
  [143] = 6, [144] = 6, [145] = 4, [146] = 3, [147] = 7, [148] = 6, [149] = 6, [150] = 7, [151] = 10, [152] = 7, [153] = 10, [154] = 6, [155] = 5, [156] = 11, [157] = 6, [158] = 6,
  [159] = 8, [160] = 4, [161] = 3, [162] = 7, [163] = 7, [164] = 7, [165] = 8, [166] = 4, [167] = 7, [168] = 6, [169] = 10, [170] = 6, [171] = 8, [172] = 8, [173] = 16, [174] = 10,
  [175] = 8, [176] = 5, [177] = 8, [178] = 5, [179] = 5, [180] = 6, [181] = 7, [182] = 7, [183] = 3, [184] = 5, [185] = 6, [186] = 6, [187] = 8, [188] = 12, [189] = 12, [190] = 12,
  [191] = 6, [192] = 9, [193] = 9, [194] = 9, [195] = 9, [196] = 9, [197] = 9, [198] = 11, [199] = 7, [200] = 7, [201] = 7, [202] = 7, [203] = 7, [204] = 5, [205] = 5, [206] = 6,
  [207] = 5, [208] = 8, [209] = 8, [210] = 8, [211] = 8, [212] = 8, [213] = 8, [214] = 8, [215] = 8, [216] = 8, [217] = 8, [218] = 8, [219] = 8, [220] = 8, [221] = 8, [222] = 7,
  [223] = 7, [224] = 7, [225] = 7, [226] = 7, [227] = 7, [228] = 7, [229] = 7, [230] = 11, [231] = 6, [232] = 7, [233] = 7, [234] = 7, [235] = 7, [236] = 3, [237] = 4, [238] = 4,
  [239] = 4, [240] = 7, [241] = 7, [242] = 7, [243] = 7, [244] = 7, [245] = 7, [246] = 7, [247] = 9, [248] = 7, [249] = 7, [250] = 7, [251] = 7, [252] = 7, [253] = 8, [254] = 7, [255] = 8
}

-- Return information about start, end in the string and the highlighted words
function getHighlightedText(text)
  local tmpData = {}

  repeat
    local tmp = {string.find(text, "{([^}]+)}", tmpData[#tmpData-1])}
    for _, v in pairs(tmp) do
      table.insert(tmpData, v)
    end
  until not(string.find(text, "{([^}]+)}", tmpData[#tmpData-1]))

  return tmpData
end

function addTabText(text, speaktype, tab, creatureName)
  if modules.client_options.getOption('showTimestampsInConsole') then
    text = os.date('%H:%M') .. ' ' .. text
  end

  local panel = consoleTabBar:getTabPanel(tab)
  local consoleBuffer = panel:getChildById('consoleBuffer')
  local label = g_ui.createWidget('ConsoleLabel', consoleBuffer)
  label:setId('consoleLabel' .. consoleBuffer:getChildCount())
  label:setText(text)
  label:setColor(speaktype.color)
  consoleTabBar:blinkTab(tab)

  -- Overlay for consoleBuffer which shows highlighted words only
  local consoleBufferHighlight = panel:getChildById('consoleBufferHighlight')
  local labelHighlight = g_ui.createWidget('ConsolePhantomLabel', consoleBufferHighlight)

  labelHighlight:setId('consoleLabel' .. consoleBufferHighlight:getChildCount())
  labelHighlight:setColor("#1f9ffe")

  local player = g_game.getLocalPlayer()
  if speaktype.npcChat and (player:getName() ~= creatureName or player:getName() == 'Account Manager') then  -- Check if it is the npc who is talking
    local highlightData = getHighlightedText(text)
    if #highlightData == 0 then
      labelHighlight:setText("")
    else
      -- Remove the curly braces
      for i = 1, #highlightData / 3 do
        local dataBlock = { _start = highlightData[(i-1)*3+1], _end = highlightData[(i-1)*3+2], words = highlightData[(i-1)*3+3] }
        text = text:gsub("{"..dataBlock.words.."}", dataBlock.words)

        -- Recalculate positions as braces are removed
        highlightData[(i-1)*3+1] = dataBlock._start - ((i-1) * 2)
        highlightData[(i-1)*3+2] = dataBlock._end - (1 + (i-1) * 2)
      end
      label:setText(text)

      -- Calculate the positions of the highlighted text and fill with string.char(127) [Width: 1]
      local drawText = label:getDrawText()
      local tmpText = ""
      for i = 1, #highlightData / 3 do
        local dataBlock = { _start = highlightData[(i-1)*3+1], _end = highlightData[(i-1)*3+2], words = highlightData[(i-1)*3+3] }
        local lastBlockEnd = (highlightData[(i-2)*3+2] or 1)

        for letter = lastBlockEnd, dataBlock._start-1 do
          local tmpChar = string.byte(drawText:sub(letter, letter))
          local fillChar = (tmpChar == 10 or tmpChar == 32) and string.char(tmpChar) or string.char(127)

          tmpText = tmpText .. string.rep(fillChar, letterWidth[tmpChar])
        end
        tmpText = tmpText .. dataBlock.words
      end

      -- Fill the highlight label to the same size as default label
      local finalBlockEnd = (highlightData[(#highlightData/3-1)*3+2] or 1)
      for letter = finalBlockEnd, drawText:len() do
          local tmpChar = string.byte(drawText:sub(letter, letter))
          local fillChar = (tmpChar == 10 or tmpChar == 32) and string.char(tmpChar) or string.char(127)

          tmpText = tmpText .. string.rep(fillChar, letterWidth[tmpChar])
      end

      labelHighlight:setText(tmpText)
    end
  else
    labelHighlight:setText("")
  end

  label.onMouseRelease = function (self, mousePos, mouseButton)
    processMessageMenu(mousePos, mouseButton, creatureName, text, self)
  end

  if consoleBuffer:getChildCount() > MAX_LINES then
    consoleBuffer:getFirstChild():destroy()
  end

  if consoleBufferHighlight:getChildCount() > MAX_LINES then
    consoleBufferHighlight:getFirstChild():destroy()
  end
end

function processChannelTabMenu(tab, mousePos, mouseButton)
  local menu = g_ui.createWidget('PopupMenu')

  channelName = tab:getText()
  if tab ~= defaultTab and tab ~= serverTab then
    menu:addOption(tr('Close'), function() removeTab(channelName) end)
    --menu:addOption(tr('Show Server Messages'), function() --[[TODO]] end)
    menu:addSeparator()
  end
  menu:addOption(tr('Clear Messages'), function() clearChannel(consoleTabBar) end)
  --menu:addOption(tr('Save Messages'), function() --[[TODO]] end)

  menu:display(mousePos)
end

function processMessageMenu(mousePos, mouseButton, creatureName, text, label)
  if mouseButton == MouseRightButton then
    local menu = g_ui.createWidget('PopupMenu')
    if creatureName then
      if creatureName ~= g_game.getCharacterName() then
        menu:addOption(tr('Message to ' .. creatureName), function () g_game.openPrivateChannel(creatureName) end)
        if not g_game.getLocalPlayer():hasVip(creatureName) then
          menu:addOption(tr('Add to VIP list'), function () g_game.addVip(creatureName) end)
        end
        if modules.game_console.getOwnPrivateTab() then
          menu:addSeparator()
          menu:addOption(tr('Invite to private chat'), function() g_game.inviteToOwnChannel(creatureName) end)
          menu:addOption(tr('Exclude from private chat'), function() g_game.excludeFromOwnChannel(creatureName) end)
        end
        if isIgnored(creatureName) then
          menu:addOption(tr('Unignore') .. ' ' .. creatureName, function() removeIgnoredPlayer(creatureName) end)
        else
          menu:addOption(tr('Ignore') .. ' ' .. creatureName, function() addIgnoredPlayer(creatureName) end)
        end
        menu:addSeparator()
      end
      if modules.game_ruleviolation.hasWindowAccess() then
        menu:addOption(tr('Rule Violation'), function() modules.game_ruleviolation.show(creatureName, text:match('.+%:%s(.+)')) end)
        menu:addSeparator()
      end

      menu:addOption(tr('Copy name'), function () g_window.setClipboardText(creatureName) end)
    end
    if label:hasSelection() then
      menu:addOption(tr('Copy'), function() g_window.setClipboardText(label:getSelection()) end, '(Ctrl+C)')
    end
    menu:addOption(tr('Copy message'), function() g_window.setClipboardText(text) end)
    menu:addOption(tr('Select all'), function() label:selectAll() end)
    menu:display(mousePos)
  end
end

function sendCurrentMessage()
  local message = consoleTextEdit:getText()
  if #message == 0 then return end
  consoleTextEdit:clearText()

  -- send message
  sendMessage(message)
end

function sendMessage(message, tab)
  local tab = tab or getCurrentTab()

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
        setTextEditText('*'.. chatCommandPrivate .. '* ')
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
  if tab == serverTab then
    tab = defaultTab
    name = defaultTab:getText()
  end

  local speaktypedesc
  if (tab.channelId or tab == defaultTab) and not chatCommandPrivateReady then
    if tab == defaultTab then
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
      setTextEditText(command)
    else
      consoleTextEdit:clearText()
    end
  end
end

function applyMessagePrefixies(name, level, message)
  if name then
    if modules.client_options.getOption('showLevelsInConsole') and level > 0 then
      message = name .. ' [' .. level .. ']: ' .. message
    else
      message = name .. ': ' .. message
    end
  end
  return message
end

function onTalk(name, level, mode, message, channelId, creaturePos)
  if mode == MessageModes.GamemasterBroadcast then
    modules.game_textmessage.displayBroadcastMessage(name .. ': ' .. message)
    return
  end

  if ignoreNpcMessages and mode == MessageModes.NpcFrom then return end
  
  speaktype = SpeakTypes[mode]
  if name ~= g_game.getLocalPlayer():getName() then
    if mode == MessageModes.Yell and isIgnoringYelling() then
      return
    elseif speaktype.private and isIgnoringPrivate() and mode ~= MessageModes.NpcFrom then
      return
    elseif isIgnored(name) then
      return
    end
  end

  if (mode == MessageModes.Say or mode == MessageModes.Whisper or mode == MessageModes.Yell or
      mode == MessageModes.Spell or mode == MessageModes.MonsterSay or mode == MessageModes.MonsterYell or
      mode == MessageModes.NpcFrom or mode == MessageModes.BarkLow or mode == MessageModes.BarkLoud) and
     creaturePos then
      -- Remove curly braces from screen message
      local staticMessage = message
      if mode == MessageModes.NpcFrom then
        local highlightData = getHighlightedText(staticMessage)
        if #highlightData > 0 then
          for i = 1, #highlightData / 3 do
            local dataBlock = { _start = highlightData[(i-1)*3+1], _end = highlightData[(i-1)*3+2], words = highlightData[(i-1)*3+3] }
            staticMessage = staticMessage:gsub("{"..dataBlock.words.."}", dataBlock.words)
          end
        end
      end

    local staticText = StaticText.create()
    staticText:addMessage(name, mode, staticMessage)
    g_map.addThing(staticText, creaturePos, -1)
  end

  local defaultMessage = mode <= 3 and true or false

  if not speaktype then
    perror('unhandled onTalk message mode ' .. mode .. ': ' .. message)
    return
  end

  if speaktype == SpeakTypesSettings.none then return end

  if speaktype.hideInConsole then return end

  local composedMessage = applyMessagePrefixies(name, level, message)

  if speaktype.private then
    addPrivateText(composedMessage, speaktype, name, false, name)
    if modules.client_options.getOption('showPrivateMessagesOnScreen') and speaktype ~= SpeakTypesSettings.privateNpcToPlayer then
      modules.game_textmessage.displayPrivateMessage(name .. ':\n' .. message)
    end
  else
    local channel = tr('Default')
    if not defaultMessage then
      channel = channels[channelId]
    end

    if channel then
      addText(composedMessage, speaktype, channel, name)
    else
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
    if openPrivateChannelWith:lower() ~= g_game.getLocalPlayer():getName():lower() then
      g_game.openPrivateChannel(openPrivateChannelWith)
    else
      modules.game_textmessage.displayFailureMessage('You cannot create a private chat channel with yourself.')
    end
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
  channelsWindow = g_ui.displayUI('channelswindow')
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

function loadIgnoreSettings()
  local ignoreNode = g_settings.getNode('IgnorePlayers')
  if ignoreNode then
    for i = 1, #ignoreNode do
      table.insert(ignoreSettings.players, ignoreNode[i])
    end
  end
  ignoreSettings.privateMessages = g_settings.getBoolean('IgnorePrivateMessages')
  ignoreSettings.yelling = g_settings.getBoolean('IgnoreYelling')
end

function saveIgnoreSettings()
  local tmpSettings = {}
  for i = 1, #ignoreSettings.players do    
    table.insert(tmpSettings, ignoreSettings.players[i])
  end
  g_settings.set('IgnorePrivateMessages', ignoreSettings.privateMessages)
  g_settings.set('IgnoreYelling', ignoreSettings.yelling)
  g_settings.setNode('IgnorePlayers', tmpSettings)
end

function isIgnored(name)
  return table.find(ignoreSettings.players, name, true)
end

function addIgnoredPlayer(name)
  if not isIgnored(name) then
    table.insert(ignoreSettings.players, name)
  end
end

function removeIgnoredPlayer(name)
  table.removevalue(ignoreSettings.players, name)
end

function isIgnoringPrivate()
  return ignoreSettings.privateMessages
end

function isIgnoringYelling()
  return ignoreSettings.yelling
end

function onClickIgnoreButton()
  if ignoreWindow then return end
  ignoreWindow = g_ui.displayUI('ignorewindow')
  local ignoreListPanel = ignoreWindow:getChildById('ignoreList')
  ignoreWindow.onDestroy = function() ignoreWindow = nil end
  
  local removeButton = ignoreWindow:getChildById('buttonRemove')
  removeButton:disable()
  ignoreListPanel.onChildFocusChange = function() removeButton:enable() end
  removeButton.onClick = function() 
            local selection = ignoreListPanel:getFocusedChild() 
            if selection then
                ignoreListPanel:removeChild(selection)
                selection:destroy()
            end
            if ignoreListPanel:getChildCount() == 0 then
                removeButton:disable()
            end
        end
  
  local newlyIgnoredPlayers = {}
  local addName = ignoreWindow:getChildById('ignoreNameEdit')
  local addButton = ignoreWindow:getChildById('buttonAdd')
  local addFunction = function() 
            if addName:getText() == '' then return end
            if table.find(ignoreSettings.players, addName:getText()) then return end
            if table.find(newlyIgnoredPlayers, addName:getText()) then return end
            local label = g_ui.createWidget('IgnoreListLabel', ignoreListPanel)
            label:setText(addName:getText())
            table.insert(newlyIgnoredPlayers, addName:getText())
            label:setPhantom(false)
            addName:setText('')
        end
  addButton.onClick = addFunction
  ignoreWindow.onEnter = addFunction
    
  local ignorePrivateMessageBox = ignoreWindow:getChildById('checkboxIgnorePrivateMessages')
  ignorePrivateMessageBox:setChecked(ignoreSettings.privateMessages)
  local ignoreYellingBox = ignoreWindow:getChildById('checkboxIgnoreYelling')
  ignoreYellingBox:setChecked(ignoreSettings.yelling)
    
  local saveButton = ignoreWindow:getChildById('buttonSave')
  saveButton.onClick = function()
                ignoreSettings.players = {}
                for i = 1, ignoreListPanel:getChildCount() do
                    addIgnoredPlayer(ignoreListPanel:getChildByIndex(i):getText())
                    --table.insert(ignoreSettings.players, ignoreListPanel:getChildByIndex(i):getText())
                end
                
                ignoreSettings.yelling = ignoreYellingBox:isChecked()
                ignoreSettings.privateMessages = ignorePrivateMessageBox:isChecked()
                ignoreWindow:destroy()
            end
    
  for _, name in pairs(ignoreSettings.players) do
      local label = g_ui.createWidget('IgnoreListLabel', ignoreListPanel)
      label:setText(name)
      label:setPhantom(false)
  end
end

function onGameStart()
  -- open last channels
  local lastChannelsOpen = g_settings.getNode('lastChannelsOpen')
  if lastChannelsOpen then
    local savedChannels = lastChannelsOpen[g_game.getLocalPlayer():getName()]
    if savedChannels then
      for channelName, channelId in pairs(savedChannels) do
        channelId = tonumber(channelId)
        if channelId ~= -1 and channelId < 100 then
          if not table.find(channels, channelId) then
            g_game.joinChannel(channelId)
          end
        end
      end
    end
  end

  local tab = defaultTab
  if tab then
    --[[
      Known Issue: The server is calling to open channels after
      onGameStart is executed causing it to focus the last tab opened.

      Fix: Don't save channels to the settings that are opened by the server.
    ]]
    addEvent(function() consoleTabBar:selectTab(tab) end, true)
  end
end

