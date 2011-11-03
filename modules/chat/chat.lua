Chat = {}

-- private variables
local chatPanel

-- public functions
function Chat.create()
  chatPanel = loadUI("/chat/chat.otui", Game.gameBottomPanel)
end

function Chat.destroy()
  chatPanel:destroy()
  chatPanel = nil
end

-- hooked events

connect(Game, { onLogin = Chat.create,
                onLogout = Chat.destroy })