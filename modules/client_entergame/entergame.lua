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
  enterGame:getChildById('accountNameTextEdit'):clearText()
  enterGame:getChildById('accountPasswordTextEdit'):clearText()
  enterGame:getChildById('accountNameTextEdit'):focus()
  Settings.remove('account')
  Settings.remove('password')
end

local function onError(protocol, message, connectionError)
  loadBox:destroy()
  loadBox = nil

  if not connectionError then
    clearAccountFields()
  end
  local errorBox = displayErrorBox('Login Error', message)
  connect(errorBox, { onOk = EnterGame.show })
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
  loadBox = nil

  CharacterList.create(characters, premDays)

  local lastMotdNumber = Settings.getNumber("motd")
  if motdNumber and motdNumber ~= lastMotdNumber then
    Settings.set("motd", motdNumber)
    local motdBox = displayInfoBox("Message of the day", motdMessage)
    connect(motdBox, { onOk = CharacterList.show })
    CharacterList.hide()
  end
end

-- public functions
function EnterGame.init()
  enterGame = displayUI('entergame.otui')
  enterGameButton = TopMenu.addLeftButton('enterGameButton', 'Login (Ctrl + G)', 'login.png', EnterGame.openWindow)
  motdButton = TopMenu.addLeftButton('motdButton', 'Message of the day', 'motd.png', EnterGame.displayMotd)
  motdButton:hide()
  Keyboard.bindKeyDown('Ctrl+G', EnterGame.openWindow)

  local account = Settings.get('account')
  local password = Settings.get('password')
  local host = Settings.get('host')
  local port = Settings.get('port')
  local autologin = Settings.getBoolean('autologin')

  enterGame:getChildById('accountNameTextEdit'):setText(account)
  enterGame:getChildById('accountPasswordTextEdit'):setText(password)
  enterGame:getChildById('serverHostTextEdit'):setText(host)
  enterGame:getChildById('serverPortTextEdit'):setText(port)
  enterGame:getChildById('autoLoginBox'):setChecked(autologin)
  enterGame:getChildById('rememberPasswordBox'):setChecked(#account > 0)
  enterGame:getChildById('accountNameTextEdit'):focus()

  -- only open entergame when app starts
  enterGame:hide()
  if not g_app.isRunning() then
    if #account > 0 and autologin then
      addEvent(EnterGame.doLogin)
    end
  else
    enterGame:hide()
  end
end

function EnterGame.terminate()
  Keyboard.unbindKeyDown('Ctrl+G')
  enterGame:destroy()
  enterGame = nil
  enterGameButton:destroy()
  enterGameButton = nil
  motdButton:destroy()
  motdButton = nil
  EnterGame = nil
end

function EnterGame.show()
  enterGame:show()
  enterGame:raise()
  enterGame:focus()
end

function EnterGame.hide()
  enterGame:hide()
end

function EnterGame.openWindow()
  if g_game.isOnline() then
    CharacterList.show()
  elseif not CharacterList.isVisible() then
    EnterGame.show()
  end
end

function EnterGame.doLogin()
  EnterGame.account = enterGame:getChildById('accountNameTextEdit'):getText()
  EnterGame.password = enterGame:getChildById('accountPasswordTextEdit'):getText()
  EnterGame.host = enterGame:getChildById('serverHostTextEdit'):getText()
  EnterGame.port = tonumber(enterGame:getChildById('serverPortTextEdit'):getText())
  EnterGame.hide()

  Settings.set('host', EnterGame.host)
  Settings.set('port', EnterGame.port)

  local protocolLogin = ProtocolLogin.create()
  protocolLogin.onError = onError
  protocolLogin.onMotd = onMotd
  protocolLogin.onCharacterList = onCharacterList

  loadBox = displayCancelBox('Please wait', 'Connecting to login server...')
  connect(loadBox, { onCancel = function(msgbox)
                                  loadBox = nil
                                  protocolLogin:cancelLogin()
                                  EnterGame.show()
                                end })

  protocolLogin:login(EnterGame.host, EnterGame.port, EnterGame.account, EnterGame.password)
end

function EnterGame.displayMotd()
  displayInfoBox('Message of the day', motdMessage)
end
