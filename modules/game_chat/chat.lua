Chat = {}

-- private variables
local chatPanel
local chatBuffer

-- private functions
local function onCreatureSpeak(name, level, msgtype, message)
  style = 'ChatLabel'
  if name and level > 0 then
      message = name .. ' [' .. level .. ']: ' .. message
      style = 'YellowChatLabel'
  end

  local label = UILabel.create()
  label:setStyle(style)
  label:setText(message)
  chatBuffer:addChild(label)
end

-- public functions
function Chat.create()
  chatPanel = UI.display('chat.otui', { parent = Game.gameBottomPanel } )
  chatBuffer = chatPanel:getChildById('chatBuffer')
end

function Chat.destroy()
  chatPanel:destroy()
  chatPanel = nil
end

-- hooked events

connect(Game, { onLogin = Chat.create,
                onLogout = Chat.destroy,
                onCreatureSpeak = onCreatureSpeak})