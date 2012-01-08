Console = {}

-- private variables
local SpeakTypes = {
  say = { color = '#FFFF00' },
  whisper = { color = '#FFFF00' },
  yell = { color = '#FFFF00' },
  monsterSay = { color = '#FE6500', hideInConsole = false},
  npcToPlayer = { color = '#5FF7F7' },
  channelYellow = { color = '#FFFF00' },
  channelWhite = { color = '#FFFFFF' },
  channelRed = { color = '#F55E5E' },
  channelOrange = { color = '#FE6500' },
  private = { color = '#FFFF00' },
  playerToNpc = { color = '#9F9DFD' },
  broadcast = { color = '#F55E5E' },
  privateRed = { color = '#F55E5E' }
}

local consolePanel
local consoleBuffer

-- public functions
function Console.create()
  consolePanel = displayUI('console.otui', { parent = Game.gameBottomPanel } )
  consoleBuffer = consolePanel:getChildById('consoleBuffer')
end

function Console.destroy()
  consolePanel:destroy()
  consolePanel = nil
end

function Console.addText(text, color)
  color = color or 'white'

  if Options.showTimestampsInConsole then
    text = os.date('%H:%M') .. ' ' .. text
  end

  local label = createWidget('ConsoleLabel', consoleBuffer)
  label:setText(text)
  label:setForegroundColor(color)
end

-- hooked events
local function onCreatureSpeak(name, level, speaktypedesc, message, channelId, creaturePos)
  speaktype = SpeakTypes[speaktypedesc]
  if speaktype == SpeakTypes.monsterSay then return end

  if name then
    if Options.showLevelsInConsole and level > 0 then
      message = name .. ' [' .. level .. ']: ' .. message
    else
      message = name .. ': ' .. message
    end
  end

  Console.addText(message, speaktype.color)
end

connect(Game, { onLogin = Console.create,
                onLogout = Console.destroy,
                onCreatureSpeak = onCreatureSpeak})