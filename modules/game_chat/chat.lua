Chat = {}

-- private variables
--[[
local SpeakTypes = {
  say = { color =   },
  whisper = { color = },
  yell,
  monsterSay,
  npcToPlayer,
  cgannelYellow,
  channelWhite,
  channelRed,
  channelOrange,
  private,
  playerToNpc,
  broadcast,
  privateRed
}
]]--

local chatPanel
local chatBuffer

-- public functions
function Chat.create()
  chatPanel = displayUI('chat.otui', { parent = Game.gameBottomPanel } )
  chatBuffer = chatPanel:getChildById('chatBuffer')
end

function Chat.destroy()
  chatPanel:destroy()
  chatPanel = nil
end

-- hooked events
local function onCreatureSpeak(name, level, msgtype, message)
  style = 'ChatLabel'
  if name and level > 0 then
      message = name .. ' [' .. level .. ']: ' .. message
      style = 'YellowChatLabel'
  end

  local label = createWidget(style)
  label:setText(message)
  chatBuffer:addChild(label)
end

connect(Game, { onLogin = Chat.create,
                onLogout = Chat.destroy,
                onCreatureSpeak = onCreatureSpeak})