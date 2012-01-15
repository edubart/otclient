EnterGame = { }

-- private variables
local loadBox
local enterGame
local motdNumber
local motdMessage
local motdButton
local enterGameButton


-- private functions
local function clearAccountFields()
  enterGame:getChildById('accountNameLineEdit'):clearText()
  enterGame:getChildById('accountPasswordLineEdit'):clearText()
  enterGame:getChildById('accountNameLineEdit'):focus()
  Settings.remove('account')
  Settings.remove('password')
end

local function onError(protocol, message, connectionError)
  loadBox:destroy()
  if not connectionError then
    clearAccountFields()
  end
  local errorBox = displayErrorBox('Login Error', message)
  errorBox.onOk = EnterGame.show
end

local function onMotd(protocol, motd)
  motdNumber = tonumber(motd:sub(0, motd:find("\n")))
  motdMessage = motd:sub(motd:find("\n") + 1, #motd)
  motdButton:show()
end

local function onCharacterList(protocol, characters, premDays)
  if enterGame:getChildById('rememberPasswordBox'):isChecked() then
    Settings.set('account', EnterGame.account)
    Settings.set('password', EnterGame.password)
    Settings.set('autologin', enterGame:getChildById('autoLoginBox'):isChecked())
  else
    clearAccountFields()
  end

  loadBox:destroy()
  CharacterList.create(characters, premDays)

  local lastMotdNumber = Settings.getNumber("motd")
  if motdNumber and motdNumber ~= lastMotdNumber then
    Settings.set("motd", motdNumber)
    local motdBox = displayInfoBox("Message of the day", motdMessage)
    motdBox.onOk = CharacterList.show
    CharacterList.hide()
  end
end

-- public functions
function EnterGame.init()
  enterGameButton = TopMenu.addButton('enterGameButton', 'Login (Ctrl + G)', '/core_styles/icons/login.png', EnterGame.openWindow)
  Hotkeys.bindKeyDown('Ctrl+G', EnterGame.openWindow)
  motdButton = TopMenu.addButton('motdButton', 'Message of the day', '/core_styles/icons/motd.png', EnterGame.displayMotd)
  motdButton:hide()
  enterGame = displayUI('entergame.otui')

  local account = Settings.get('account')
  local password = Settings.get('password')
  local host = Settings.get('host')
  local port = Settings.get('port')
  local autologin = Settings.getBoolean('autologin')

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

function EnterGame.terminate()
  Hotkeys.unbindKeyDown('Ctrl+G')
  enterGame:destroy()
  enterGame = nil
  enterGameButton:destroy()
  enterGameButton = nil
  motdButton:destroy()
  motdButton = nil
end

function EnterGame.show()
  enterGame:show()
  enterGame:focus()
end

function EnterGame.hide()
  enterGame:hide()
end

function EnterGame.openWindow()
  if Game.isOnline() then
    CharacterList.show()
  elseif not CharacterList.isVisible() then
    EnterGame.show()
  end
end

function EnterGame.doLogin()
  EnterGame.account = enterGame:getChildById('accountNameLineEdit'):getText()
  EnterGame.password = enterGame:getChildById('accountPasswordLineEdit'):getText()
  EnterGame.host = enterGame:getChildById('serverHostLineEdit'):getText()
  EnterGame.port = tonumber(enterGame:getChildById('serverPortLineEdit'):getText())
  EnterGame.hide()

  Settings.set('host', EnterGame.host)
  Settings.set('port', EnterGame.port)

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
  displayInfoBox('Message of the day', motdMessage)
end

