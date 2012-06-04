EnterGame = { }

-- private variables
local loadBox
local enterGame
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
  local errorBox = displayErrorBox(tr('Login Error'), message)
  connect(errorBox, { onOk = EnterGame.show })
end

local function onMotd(protocol, motd)
  G.motdNumber = tonumber(motd:sub(0, motd:find("\n")))
  G.motdMessage = motd:sub(motd:find("\n") + 1, #motd)
  motdButton:show()
end

local function onCharacterList(protocol, characters, premDays)
  if enterGame:getChildById('rememberPasswordBox'):isChecked() then
    Settings.set('account', g_crypt.encrypt(G.account))
    Settings.set('password', g_crypt.encrypt(G.password))
    Settings.set('autologin', enterGame:getChildById('autoLoginBox'):isChecked())
  else
    clearAccountFields()
  end

  loadBox:destroy()
  loadBox = nil

  CharacterList.create(characters, premDays)
  CharacterList.show()

  local lastMotdNumber = Settings.getNumber("motd")
  if G.motdNumber and G.motdNumber ~= lastMotdNumber then
    Settings.set("motd", motdNumber)
    local motdBox = displayInfoBox(tr('Message of the day'), G.motdMessage)
    connect(motdBox, { onOk = CharacterList.show })
    CharacterList.hide()
  end
end

-- public functions
function EnterGame.init()
  enterGame = displayUI('entergame.otui')
  enterGameButton = TopMenu.addLeftButton('enterGameButton', tr('Login') .. ' (Ctrl + G)', 'login.png', EnterGame.openWindow)
  motdButton = TopMenu.addLeftButton('motdButton', tr('Message of the day'), 'motd.png', EnterGame.displayMotd)
  motdButton:hide()
  Keyboard.bindKeyDown('Ctrl+G', EnterGame.openWindow)

  if G.motdNumber then
    motdButton:show()
  end

  local account = g_crypt.decrypt(Settings.get('account'))
  local password = g_crypt.decrypt(Settings.get('password'))
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
  G.account = enterGame:getChildById('accountNameTextEdit'):getText()
  G.password = enterGame:getChildById('accountPasswordTextEdit'):getText()
  G.host = enterGame:getChildById('serverHostTextEdit'):getText()
  G.port = tonumber(enterGame:getChildById('serverPortTextEdit'):getText())
  EnterGame.hide()

  Settings.set('host', G.host)
  Settings.set('port', G.port)

  local protocolLogin = ProtocolLogin.create()
  protocolLogin.onError = onError
  protocolLogin.onMotd = onMotd
  protocolLogin.onCharacterList = onCharacterList

  loadBox = displayCancelBox(tr('Please wait'), tr('Connecting to login server...'))
  connect(loadBox, { onCancel = function(msgbox)
                                  loadBox = nil
                                  protocolLogin:cancelLogin()
                                  EnterGame.show()
                                end })

  protocolLogin:login(G.host, G.port, G.account, G.password)
end

function EnterGame.displayMotd()
  displayInfoBox(tr('Message of the day'), G.motdMessage)
end
