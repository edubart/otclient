EnterGame = { }

-- private variables
local loadBox
local enterGame
local motdNumber
local motdMessage

-- private functions
local function onError(protocol, error)
  loadBox:destroy()
  local errorBox = displayErrorBox('Login Error', error)
  errorBox.onOk = EnterGame.create
end

local function onMotd(protocol, motd)
  motdNumber = tonumber(string.sub(motd, 0, string.find(motd, "\n")))
  motdMessage = string.sub(motd, string.find(motd, "\n") + 1, string.len(motd))
end

local function onCharacterList(protocol, characters, premDays)
  loadBox:destroy()
  CharacterList.create(characters, premDays)

  local lastMotdNumber = tonumber(Configs.get("motd"))
  if motdNumber and motdNumber ~= lastMotdNumber then
    Configs.set("motd", motdNumber)
    local motdBox = displayInfoBox("Message of the day", motdMessage)
    motdBox.onOk = CharacterList.show
    CharacterList.hide()
  end
end

-- public functions
function EnterGame.create()
  enterGame = UI.loadAndDisplay('/entergame/entergame.otui')
end

function EnterGame.destroy()
  enterGame:destroy()
  enterGame = nil
end

function EnterGame.show()
  enterGame:show()
end

function EnterGame.hide()
  enterGame:hide()
end

function EnterGame.doLogin()
  EnterGame.account = enterGame:getChildById('accountNameLineEdit'):getText()
  EnterGame.password = enterGame:getChildById('accountPasswordLineEdit'):getText()
  EnterGame.hide()

  local protocolLogin = ProtocolLogin.create()
  protocolLogin.onError = onError
  protocolLogin.onMotd = onMotd
  protocolLogin.onCharacterList = onCharacterList

  loadBox = displayCancelBox('Please wait', 'Connecting to login server...')
  loadBox.onCancel = function(msgbox)
    protocolLogin:cancelLogin()
    EnterGame.show()
  end

  protocolLogin:login(EnterGame.account, EnterGame.password)
end
