EnterGame = { }

serverConfig = {
  ip = "127.0.0.1",
  port = 7171
}

-- private variables
local loadBox
local enterGame
local motdWindow
local motdButton
local enterGameButton
local clientBox
local protocolLogin
local motdEnabled = true

-- private functions
local function onError(protocol, message, errorCode)
  if loadBox then
    loadBox:destroy()
    loadBox = nil
  end

  if not errorCode then
    EnterGame.clearAccountFields()
  end

  local errorBox = displayErrorBox(tr('Login Error'), message)
  connect(errorBox, { onOk = EnterGame.show })
end

local function onMotd(protocol, motd)
  G.motdNumber = tonumber(motd:sub(0, motd:find("\n")))
  G.motdMessage = motd:sub(motd:find("\n") + 1, #motd)
  if motdEnabled then
    motdButton:show()
  end
end

local function onSessionKey(protocol, sessionKey)
  G.sessionKey = sessionKey
end

local function onCharacterList(protocol, characters, account, otui)
  -- Try add server to the server list
  ServerList.add(G.host, G.port, g_game.getClientVersion())

  -- Save 'Stay logged in' setting
  g_settings.set('staylogged', enterGame:getChildById('stayLoggedBox'):isChecked())

  if enterGame:getChildById('rememberPasswordBox'):isChecked() then
    local account = g_crypt.encrypt(G.account)
    local password = g_crypt.encrypt(G.password)

    g_settings.set('account', account)
    g_settings.set('password', password)

    ServerList.setServerAccount(G.host, account)
    ServerList.setServerPassword(G.host, password)

    g_settings.set('autologin', enterGame:getChildById('autoLoginBox'):isChecked())
  else
    -- reset server list account/password
    ServerList.setServerAccount(G.host, '')
    ServerList.setServerPassword(G.host, '')

    EnterGame.clearAccountFields()
  end

  loadBox:destroy()
  loadBox = nil

  for _, characterInfo in pairs(characters) do
    if characterInfo.previewState and characterInfo.previewState ~= PreviewState.Default then
      characterInfo.worldName = characterInfo.worldName .. ', Preview'
    end
  end

  CharacterList.create(characters, account, otui)
  CharacterList.show()

  if motdEnabled then
    local lastMotdNumber = g_settings.getNumber("motd")
    if G.motdNumber and G.motdNumber ~= lastMotdNumber then
      g_settings.set("motd", G.motdNumber)
      motdWindow = displayInfoBox(tr('Message of the day'), G.motdMessage)
      connect(motdWindow, { onOk = function() CharacterList.show() motdWindow = nil end })
      CharacterList.hide()
    end
  end
end

local function onUpdateNeeded(protocol, signature)
  loadBox:destroy()
  loadBox = nil

  if EnterGame.updateFunc then
    local continueFunc = EnterGame.show
    local cancelFunc = EnterGame.show
    EnterGame.updateFunc(signature, continueFunc, cancelFunc)
  else
    local errorBox = displayErrorBox(tr('Update needed'), tr('Your client needs updating, try redownloading it.'))
    connect(errorBox, { onOk = EnterGame.show })
  end
end

-- public functions
function EnterGame.init()
  enterGame = g_ui.displayUI('entergame')
  enterGameButton = modules.client_topmenu.addLeftButton('enterGameButton', tr('Login') .. ' (Ctrl + G)', '/images/topbuttons/login', EnterGame.openWindow)
  motdButton = modules.client_topmenu.addLeftButton('motdButton', tr('Message of the day'), '/images/topbuttons/motd', EnterGame.displayMotd)
  motdButton:hide()
  g_keyboard.bindKeyDown('Ctrl+G', EnterGame.openWindow)

  if motdEnabled and G.motdNumber then
    motdButton:show()
  end

  local account = g_settings.get('account')
  local password = g_settings.get('password')
  local host = g_settings.get('host')
  local port = g_settings.get('port')
  local stayLogged = g_settings.getBoolean('staylogged')
  local autologin = g_settings.getBoolean('autologin')
  local clientVersion = g_settings.getInteger('client-version')
  if clientVersion == 0 then clientVersion = 1074 end

  if port == nil or port == 0 then port = 7171 end

  EnterGame.setAccountName(account)
  EnterGame.setPassword(password)

  enterGame:getChildById('serverHostTextEdit'):setText(host)
  enterGame:getChildById('serverPortTextEdit'):setText(port)
  enterGame:getChildById('autoLoginBox'):setChecked(autologin)
  enterGame:getChildById('stayLoggedBox'):setChecked(stayLogged)

  clientBox = enterGame:getChildById('clientComboBox')
  for _, proto in pairs(g_game.getSupportedClients()) do
    clientBox:addOption(proto)
  end
  clientBox:setCurrentOption(clientVersion)

  EnterGame.toggleAuthenticatorToken(clientVersion, true)
  EnterGame.toggleStayLoggedBox(clientVersion, true)
  connect(clientBox, { onOptionChange = EnterGame.onClientVersionChange })

  enterGame:hide()

  if g_app.isRunning() and not g_game.isOnline() then
    enterGame:show()
  end
  EnterGame.setUniqueServer(serverConfig.ip, serverConfig.port, 270, 210)
end

function EnterGame.firstShow()
  EnterGame.show()

  local account = g_crypt.decrypt(g_settings.get('account'))
  local password = g_crypt.decrypt(g_settings.get('password'))
  local host = g_settings.get('host')
  local autologin = g_settings.getBoolean('autologin')
  if #host > 0 and #password > 0 and #account > 0 and autologin then
    addEvent(function()
      if not g_settings.getBoolean('autologin') then return end
      EnterGame.doLogin()
    end)
  end
end

function EnterGame.terminate()
  g_keyboard.unbindKeyDown('Ctrl+G')
  disconnect(clientBox, { onOptionChange = EnterGame.onClientVersionChange })
  enterGame:destroy()
  enterGame = nil
  enterGameButton:destroy()
  enterGameButton = nil
  clientBox = nil
  if motdWindow then
    motdWindow:destroy()
    motdWindow = nil
  end
  if motdButton then
    motdButton:destroy()
    motdButton = nil
  end
  if loadBox then
    loadBox:destroy()
    loadBox = nil
  end
  if protocolLogin then
    protocolLogin:cancelLogin()
    protocolLogin = nil
  end
  EnterGame = nil
end

function EnterGame.show()
  if loadBox then return end
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
  elseif not g_game.isLogging() and not CharacterList.isVisible() then
    EnterGame.show()
  end
end

function EnterGame.setAccountName(account)
  local account = g_crypt.decrypt(account)
  enterGame:getChildById('accountNameTextEdit'):setText(account)
  enterGame:getChildById('accountNameTextEdit'):setCursorPos(-1)
  enterGame:getChildById('rememberPasswordBox'):setChecked(#account > 0)
end

function EnterGame.setPassword(password)
  local password = g_crypt.decrypt(password)
  enterGame:getChildById('accountPasswordTextEdit'):setText(password)
end

function EnterGame.clearAccountFields()
  enterGame:getChildById('accountNameTextEdit'):clearText()
  enterGame:getChildById('accountPasswordTextEdit'):clearText()
  enterGame:getChildById('authenticatorTokenTextEdit'):clearText()
  enterGame:getChildById('accountNameTextEdit'):focus()
  g_settings.remove('account')
  g_settings.remove('password')
end

function EnterGame.toggleAuthenticatorToken(clientVersion, init)
  local enabled = (clientVersion >= 1072)
  if enabled == enterGame.authenticatorEnabled then
    return
  end

  enterGame:getChildById('authenticatorTokenLabel'):setOn(enabled)
  enterGame:getChildById('authenticatorTokenTextEdit'):setOn(enabled)

  local newHeight = enterGame:getHeight()
  local newY = enterGame:getY()
  if enabled then
    newY = newY - enterGame.authenticatorHeight
    newHeight = newHeight + enterGame.authenticatorHeight
  else
    newY = newY + enterGame.authenticatorHeight
    newHeight = newHeight - enterGame.authenticatorHeight
  end

  if not init then
    enterGame:breakAnchors()
    enterGame:setY(newY)
    enterGame:bindRectToParent()
  end
  enterGame:setHeight(newHeight)

  enterGame.authenticatorEnabled = enabled
end

function EnterGame.toggleStayLoggedBox(clientVersion, init)
  local enabled = (clientVersion >= 1074)
  if enabled == enterGame.stayLoggedBoxEnabled then
    return
  end

  enterGame:getChildById('stayLoggedBox'):setOn(enabled)

  local newHeight = enterGame:getHeight()
  local newY = enterGame:getY()
  if enabled then
    newY = newY - enterGame.stayLoggedBoxHeight
    newHeight = newHeight + enterGame.stayLoggedBoxHeight
  else
    newY = newY + enterGame.stayLoggedBoxHeight
    newHeight = newHeight - enterGame.stayLoggedBoxHeight
  end

  if not init then
    enterGame:breakAnchors()
    enterGame:setY(newY)
    enterGame:bindRectToParent()
  end
  enterGame:setHeight(newHeight)

  enterGame.stayLoggedBoxEnabled = enabled
end

function EnterGame.onClientVersionChange(comboBox, text, data)
  local clientVersion = tonumber(text)
  EnterGame.toggleAuthenticatorToken(clientVersion)
  EnterGame.toggleStayLoggedBox(clientVersion)
end

function EnterGame.doLogin()
  G.account = enterGame:getChildById('accountNameTextEdit'):getText()
  G.password = enterGame:getChildById('accountPasswordTextEdit'):getText()
  G.authenticatorToken = enterGame:getChildById('authenticatorTokenTextEdit'):getText()
  G.stayLogged = enterGame:getChildById('stayLoggedBox'):isChecked()
  G.host = enterGame:getChildById('serverHostTextEdit'):getText()
  G.port = tonumber(enterGame:getChildById('serverPortTextEdit'):getText())
  local clientVersion = tonumber(clientBox:getText())
  EnterGame.hide()

  if g_game.isOnline() then
    local errorBox = displayErrorBox(tr('Login Error'), tr('Cannot login while already in game.'))
    connect(errorBox, { onOk = EnterGame.show })
    return
  end

  g_settings.set('host', G.host)
  g_settings.set('port', G.port)
  g_settings.set('client-version', clientVersion)

  protocolLogin = ProtocolLogin.create()
  protocolLogin.onLoginError = onError
  protocolLogin.onMotd = onMotd
  protocolLogin.onSessionKey = onSessionKey
  protocolLogin.onCharacterList = onCharacterList
  protocolLogin.onUpdateNeeded = onUpdateNeeded

  loadBox = displayCancelBox(tr('Please wait'), tr('Connecting to login server...'))
  connect(loadBox, { onCancel = function(msgbox)
                                  loadBox = nil
                                  protocolLogin:cancelLogin()
                                  EnterGame.show()
                                end })

  g_game.setClientVersion(clientVersion)
  g_game.setProtocolVersion(g_game.getClientProtocolVersion(clientVersion))
  g_game.chooseRsa(G.host)

  if modules.game_things.isLoaded() then
    protocolLogin:login(G.host, G.port, G.account, G.password, G.authenticatorToken, G.stayLogged)
  else
    loadBox:destroy()
    loadBox = nil
    EnterGame.show()
  end
end

function EnterGame.displayMotd()
  if not motdWindow then
    motdWindow = displayInfoBox(tr('Message of the day'), G.motdMessage)
    motdWindow.onOk = function() motdWindow = nil end
  end
end

function EnterGame.setDefaultServer(host, port, protocol)
  local hostTextEdit = enterGame:getChildById('serverHostTextEdit')
  local portTextEdit = enterGame:getChildById('serverPortTextEdit')
  local clientLabel = enterGame:getChildById('clientLabel')
  local accountTextEdit = enterGame:getChildById('accountNameTextEdit')
  local passwordTextEdit = enterGame:getChildById('accountPasswordTextEdit')
  local authenticatorTokenTextEdit = enterGame:getChildById('authenticatorTokenTextEdit')

  if hostTextEdit:getText() ~= host then
    hostTextEdit:setText(host)
    portTextEdit:setText(port)
    clientBox:setCurrentOption(protocol)
    accountTextEdit:setText('')
    passwordTextEdit:setText('')
    authenticatorTokenTextEdit:setText('')
  end
end

function EnterGame.setUniqueServer(host, port, protocol, windowWidth, windowHeight)
  local hostTextEdit = enterGame:getChildById('serverHostTextEdit')
  hostTextEdit:setText(host)
  hostTextEdit:setVisible(false)
  hostTextEdit:setHeight(0)
  local portTextEdit = enterGame:getChildById('serverPortTextEdit')
  portTextEdit:setText(port)
  portTextEdit:setVisible(false)
  portTextEdit:setHeight(0)

  local authenticatorTokenTextEdit = enterGame:getChildById('authenticatorTokenTextEdit')
  authenticatorTokenTextEdit:setText('')
  authenticatorTokenTextEdit:setOn(false)
  local authenticatorTokenLabel = enterGame:getChildById('authenticatorTokenLabel')
  authenticatorTokenLabel:setOn(false)

  local stayLoggedBox = enterGame:getChildById('stayLoggedBox')
  stayLoggedBox:setChecked(false)
  stayLoggedBox:setOn(false)

  clientBox:setCurrentOption(protocol)
  clientBox:setVisible(false)
  clientBox:setHeight(0)

  local serverLabel = enterGame:getChildById('serverLabel')
  serverLabel:setVisible(false)
  serverLabel:setHeight(0)
  local portLabel = enterGame:getChildById('portLabel')
  portLabel:setVisible(false)
  portLabel:setHeight(0)
  local clientLabel = enterGame:getChildById('clientLabel')
  clientLabel:setVisible(false)
  clientLabel:setHeight(0)

  local serverListButton = enterGame:getChildById('serverListButton')
  serverListButton:setVisible(false)
  serverListButton:setHeight(0)
  serverListButton:setWidth(0)

  local rememberPasswordBox = enterGame:getChildById('rememberPasswordBox')
  rememberPasswordBox:setMarginTop(-8)

  if not windowWidth then windowWidth = 236 end
  enterGame:setWidth(windowWidth)
  if not windowHeight then windowHeight = 210 end
  enterGame:setHeight(windowHeight)
end

function EnterGame.setServerInfo(message)
  local label = enterGame:getChildById('serverInfoLabel')
  label:setText(message)
end

function EnterGame.disableMotd()
  motdEnabled = false
  motdButton:hide()
end
