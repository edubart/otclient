EnterGame = { }

-- private variables
local loadBox
local enterGame
local motdButton
local enterGameButton
local protocolBox

-- private functions
local function onError(protocol, message, errorCode)
  loadBox:destroy()
  loadBox = nil

  if not errorCode then
    EnterGame.clearAccountFields()
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
    g_settings.set('account', g_crypt.encrypt(G.account))
    g_settings.set('password', g_crypt.encrypt(G.password))
    g_settings.set('autologin', enterGame:getChildById('autoLoginBox'):isChecked())
  else
    EnterGame.clearAccountFields()
  end

  loadBox:destroy()
  loadBox = nil

  CharacterList.create(characters, premDays)
  CharacterList.show()

  local lastMotdNumber = g_settings.getNumber("motd")
  if G.motdNumber and G.motdNumber ~= lastMotdNumber then
    g_settings.set("motd", motdNumber)
    local motdBox = displayInfoBox(tr('Message of the day'), G.motdMessage)
    connect(motdBox, { onOk = CharacterList.show })
    CharacterList.hide()
  end
end

-- public functions
function EnterGame.init()
  enterGame = g_ui.displayUI('entergame.otui')
  enterGameButton = TopMenu.addLeftButton('enterGameButton', tr('Login') .. ' (Ctrl + G)', 'login.png', EnterGame.openWindow)
  motdButton = TopMenu.addLeftButton('motdButton', tr('Message of the day'), 'motd.png', EnterGame.displayMotd)
  motdButton:hide()
  g_keyboard.bindKeyDown('Ctrl+G', EnterGame.openWindow)

  if G.motdNumber then
    motdButton:show()
  end

  local account = g_crypt.decrypt(g_settings.get('account'))
  local password = g_crypt.decrypt(g_settings.get('password'))
  local host = g_settings.get('host')
  local port = g_settings.get('port')
  local autologin = g_settings.getBoolean('autologin')
  local clientVersion = g_game.getClientVersion()

  if not clientVersion or clientVersion == 0 then clientVersion = 960 end

  if port == nil or port == 0 then port = 7171 end

  enterGame:getChildById('accountNameTextEdit'):setText(account)
  enterGame:getChildById('accountPasswordTextEdit'):setText(password)
  enterGame:getChildById('serverHostTextEdit'):setText(host)
  enterGame:getChildById('serverPortTextEdit'):setText(port)
  enterGame:getChildById('autoLoginBox'):setChecked(autologin)
  enterGame:getChildById('rememberPasswordBox'):setChecked(#account > 0)
  enterGame:getChildById('accountNameTextEdit'):focus()

  protocolBox = enterGame:getChildById('protocolComboBox')
  for _i, proto in pairs(g_game.getSupportedProtocols()) do
    protocolBox:addOption(proto)
  end
  protocolBox:setCurrentOption(clientVersion)

  -- only open entergame when app starts
  if not g_app.isRunning() then
    if #host > 0 and #password > 0 and #account > 0 and autologin then
      addEvent(EnterGame.doLogin)
    end
  else
    enterGame:hide()
  end
end

function EnterGame.terminate()
  g_keyboard.unbindKeyDown('Ctrl+G')
  enterGame:destroy()
  enterGame = nil
  enterGameButton:destroy()
  enterGameButton = nil
  motdButton:destroy()
  motdButton = nil
  protocolBox = nil
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

function EnterGame.clearAccountFields()
  enterGame:getChildById('accountNameTextEdit'):clearText()
  enterGame:getChildById('accountPasswordTextEdit'):clearText()
  enterGame:getChildById('accountNameTextEdit'):focus()
  g_settings.remove('account')
  g_settings.remove('password')
end

function EnterGame.doLogin()
  G.account = enterGame:getChildById('accountNameTextEdit'):getText()
  G.password = enterGame:getChildById('accountPasswordTextEdit'):getText()
  G.host = enterGame:getChildById('serverHostTextEdit'):getText()
  G.port = tonumber(enterGame:getChildById('serverPortTextEdit'):getText())
  local clientVersion = tonumber(protocolBox:getText())
  EnterGame.hide()

  if g_game.isOnline() then
    local errorBox = displayErrorBox(tr('Login Error'), tr('Cannot login while already in game.'))
    connect(errorBox, { onOk = EnterGame.show })
    return
  end

  g_settings.set('host', G.host)
  g_settings.set('port', G.port)

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

  g_game.chooseRsa(G.host)
  g_game.setClientVersion(clientVersion)
  protocolLogin:login(G.host, G.port, G.account, G.password)
end

function EnterGame.displayMotd()
  displayInfoBox(tr('Message of the day'), G.motdMessage)
end

function EnterGame.setUniqueServer(host, port, protocol, windowHeight)
  local hostTextEdit = enterGame:getChildById('serverHostTextEdit')
  hostTextEdit:setText(host)
  hostTextEdit:setVisible(false)
  hostTextEdit:setHeight(0)
  local portTextEdit = enterGame:getChildById('serverPortTextEdit')
  portTextEdit:setText(port)
  portTextEdit:setVisible(false)
  portTextEdit:setHeight(0)

  protocolBox:setCurrentOption(protocol)
  protocolBox:setVisible(false)
  protocolBox:setHeight(0)

  local serverLabel = enterGame:getChildById('serverLabel')
  serverLabel:setVisible(false)
  serverLabel:setHeight(0)
  local portLabel = enterGame:getChildById('portLabel')
  portLabel:setVisible(false)
  portLabel:setHeight(0)
  local protocolLabel = enterGame:getChildById('protocolLabel')
  protocolLabel:setVisible(false)
  protocolLabel:setHeight(0)

  local rememberPasswordBox = enterGame:getChildById('rememberPasswordBox')
  rememberPasswordBox:setMarginTop(-5)

  if not windowHeight then windowHeight = 200 end
  enterGame:setHeight(windowHeight)
end
