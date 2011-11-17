EnterGame = { }

-- private variables
local loadBox
local enterGame
local motdNumber
local motdMessage

-- private functions
local function clearAccountFields()
  enterGame:getChildById('accountNameLineEdit'):clearText()
  enterGame:getChildById('accountPasswordLineEdit'):clearText()
  enterGame:getChildById('accountNameLineEdit'):focus()
  Configs.set('account', nil)
  Configs.set('password', nil)
end

local function onError(protocol, error)
  loadBox:destroy()
  clearAccountFields()
  local errorBox = displayErrorBox('Login Error', error)
  errorBox.onOk = EnterGame.show
end

local function onMotd(protocol, motd)
  motdNumber = tonumber(string.sub(motd, 0, string.find(motd, "\n")))
  motdMessage = string.sub(motd, string.find(motd, "\n") + 1, string.len(motd))
  TopMenu.getButton('motdButton'):show()
end

local function onCharacterList(protocol, characters, premDays)
  if enterGame:getChildById('rememberPasswordBox'):isChecked() then
    Configs.set('account', EnterGame.account)
    Configs.set('password', EnterGame.password)
    Configs.set('autologin', tostring(enterGame:getChildById('autoLoginBox'):isChecked()))
  else
    clearAccountFields()
  end

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
  enterGame = UI.display('entergame.otui')

  local account = Configs.get('account')
  local password = Configs.get('password')
  local host = Configs.get('host')
  local port = tonumber(Configs.get('port'))
  local autologin = toboolean(Configs.get('autologin'))

  enterGame:getChildById('accountNameLineEdit'):setText(account)
  enterGame:getChildById('accountPasswordLineEdit'):setText(password)
  enterGame:getChildById('serverHostLineEdit'):setText(host)
  enterGame:getChildById('serverPortLineEdit'):setText(port)
  enterGame:getChildById('autoLoginBox'):setChecked(autologin)
  enterGame:getChildById('rememberPasswordBox'):setChecked(#account > 0)
  enterGame:getChildById('accountNameLineEdit'):focus()

  if #account > 0 and autologin then
    addEvent(EnterGame.doLogin)
  end
end

function EnterGame.destroy()
  enterGame:destroy()
  enterGame = nil
end

function EnterGame.show()
  enterGame:show()
  enterGame:focus()
end

function EnterGame.hide()
  enterGame:hide()
end

function EnterGame.doLogin()
  EnterGame.account = enterGame:getChildById('accountNameLineEdit'):getText()
  EnterGame.password = enterGame:getChildById('accountPasswordLineEdit'):getText()
  EnterGame.host = enterGame:getChildById('serverHostLineEdit'):getText()
  EnterGame.port = enterGame:getChildById('serverPortLineEdit'):getText()
  EnterGame.hide()

  Configs.set('host', EnterGame.host)
  Configs.set('port', EnterGame.port)

  local protocolLogin = ProtocolLogin.create()
  protocolLogin.onError = onError
  protocolLogin.onMotd = onMotd
  protocolLogin.onCharacterList = onCharacterList

  loadBox = displayCancelBox('Please wait', 'Connecting to login server...')
  loadBox.onCancel = function(msgbox)
    protocolLogin:cancelLogin()
    EnterGame.show()
  end

  protocolLogin:login(EnterGame.host, EnterGame.port, EnterGame.account, EnterGame.password)
end

function EnterGame.displayMotd()
  displayInfoBox("Message of the day", motdMessage)
end
