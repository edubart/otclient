EnterGame = { }

-- private variables
local password
local loadBox
local enterGameWindow

-- private functions
local function onError(protocol, error)
  loadBox:destroy()
  local errorBox = displayErrorBox('Login Error', error)
  errorBox.onOk = EnterGame.create
end

local function onMotd(protocol, motd)
  loadBox:destroy()
  local motdNumber = string.sub(motd, 0, string.find(motd, "\n"))
  local motdMessage = string.sub(motd, string.find(motd, "\n") + 1, string.len(motd))
  if motdNumber ~= Configs.get("motd") then
    displayInfoBox("Message of the day", motdMessage)
    Configs.set("motd", motdNumber)
  end
end

local function onCharacterList(protocol, characters, premDays)
  CharacterList.create(characters, premDays)
end

-- public functions
function EnterGame.create()
  enterGameWindow = UI.loadAndDisplayLocked('/mainmenu/ui/entergamewindow.otui')
end

function EnterGame.destroy()
  enterGameWindow:destroy()
  enterGameWindow = nil
end

function EnterGame.doLogin()
  EnterGame.account = enterGameWindow:getChildById('accountNameLineEdit'):getText()
  EnterGame.password = enterGameWindow:getChildById('accountPasswordLineEdit'):getText()
  EnterGame.destroy()

  local protocolLogin = ProtocolLogin.create()
  protocolLogin.onError = onError
  protocolLogin.onMotd = onMotd
  protocolLogin.onCharacterList = onCharacterList

  loadBox = displayCancelBox('Please wait', 'Connecting to login server...')
  loadBox.onCancel = function(msgbox)
    protocolLogin:cancelLogin()
    EnterGame.create()
  end

  protocolLogin:login(EnterGame.account, EnterGame.password)
end
