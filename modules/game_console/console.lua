Console = {}

-- private variables
local SpeakTypes = {
  say = { color = '#FFFF00' },
  whisper = { color = '#FFFF00' },
  yell = { color = '#FFFF00' },
  monsterSay = { color = '#FE6500', hideInConsole = true},
  monsterYell = { color = '#FE6500', hideInConsole = true},
  npcToPlayer = { color = '#5FF7F7' },
  channelYellow = { color = '#FFFF00' },
  channelWhite = { color = '#FFFFFF' },
  channelRed = { color = '#F55E5E' },
  channelOrange = { color = '#FE6500' },
  private = { color = '#5FF7F7' },
  playerToNpc = { color = '#9F9DFD' },
  broadcast = { color = '#F55E5E' },
  privateRed = { color = '#F55E5E' }
}

local consolePanel
local consoleBuffer
local consoleTabBar
local defaultChannelTab
local serverLogTab
local currentTab

-- public functions
function Console.create()
  consolePanel = displayUI('console.otui', { parent = Game.gameBottomPanel } )
  consoleBuffer = consolePanel:getChildById('consoleBuffer')
  consoleTabBar = consolePanel:getChildById('consoleTabBar')
  consoleTabBar:setContentWidget(consoleBuffer)
  defaultChannelTab = consoleTabBar:addTab('Default')
  serverLogTab = consoleTabBar:addTab('Server Log')

  Hotkeys.bind('Tab', function() consoleTabBar:selectNextTab() end, consolePanel)
  Hotkeys.bind('Shift+Tab', function() consoleTabBar:selectPrevTab() end, consolePanel)
end

function Console.destroy()
  consolePanel:destroy()
  consolePanel = nil
end

function Console.addText(text, color, channelTab)
  color = color or 'white'

  if Options.showTimestampsInConsole then
    text = os.date('%H:%M') .. ' ' .. text
  end

  local label = createWidget('ConsoleLabel', consoleTabBar:getTabPanel(channelTab))
  label:setText(text)
  label:setColor(color)
  consoleTabBar:blinkTab(channelTab)
end

function Console.addChannelMessage(text, color, channel)
  if channel == 'Server Log' then
    Console.addText(text, color, serverLogTab)
  elseif channel == 'Default' then
    Console.addText(text, color, defaultChannelTab)
  end
end

-- hooked events
local function onCreatureSpeak(name, level, speaktypedesc, message, channelId, creaturePos)
  speaktype = SpeakTypes[speaktypedesc]
  if speaktype.hideInConsole then return end

  if name then
    if Options.showLevelsInConsole and level > 0 then
      message = name .. ' [' .. level .. ']: ' .. message
    else
      message = name .. ': ' .. message
    end
  end

  Console.addText(message, speaktype.color, defaultChannelTab)
end

connect(Game, { onLogin = Console.create,
                onLogout = Console.destroy,
                onCreatureSpeak = onCreatureSpeak})