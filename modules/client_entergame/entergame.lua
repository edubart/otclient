EnterGame = { }

-- private variables
local loadBox
local enterGame
local motdWindow
local motdButton
local enterGameButton
local protocolBox
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

local function onCharacterList(protocol, characters, account, otui)
  -- Try add server to the server list
  ServerList.add(G.host, G.port, g_game.getProtocolVersion())

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

  CharacterList.create(characters, account, otui)
  CharacterList.show()

  if motdEnabled then
    local lastMotdNumber = g_settings.getNumber("motd")
    if G.motdNumber and G.motdNumber ~= lastMotdNumber then
      g_settings.set("motd", motdNumber)
      motdWindow = displayInfoBox(tr('Message of the day'), G.motdMessage)
      connect(motdWindow, { onOk = function() CharacterList.show() motdWindow = nil end })
      CharacterList.hide()
    end
  end
end

local function onChangeProtocol(combobox, option)
  local clients = g_game.getSupportedClients(option)
  protocolBox:setTooltip("Supports Client" .. (#clients > 1 and "s" or "") .. ": " .. table.tostring(clients))
end

local function onUpdateNeeded(protocol, signature)
  loadBox:destroy()
  loadBox = nil

  if EnterGame.updateFunc then
    local continueFunc = EnterGame.show
    local cancelFunc = EnterGame.show
    EnterGame.updateFunc(signature, continueFunc, cancelFunc)
  else
    local errorBox = displayErrorBox(tr('Update needed'), tr('Your client needs update, try redownloading it.'))
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
  local autologin = g_settings.getBoolean('autologin')
  local protocolVersion = g_settings.getInteger('protocol-version')

  if port == nil or port == 0 then port = 7171 end

  EnterGame.setAccountName(account)
  EnterGame.setPassword(password)

  enterGame:getChildById('serverHostTextEdit'):setText(host)
  enterGame:getChildById('serverPortTextEdit'):setText(port)
  enterGame:getChildById('autoLoginBox'):setChecked(autologin)

  protocolBox = enterGame:getChildById('protocolComboBox')
  protocolBox.onOptionChange = onChangeProtocol
  if protocolVersion then
    protocolBox:setCurrentOption(protocolVersion)
  end

  enterGame:hide()

  if g_app.isRunning() and not g_game.isOnline() then
    enterGame:show()
  end
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
  enterGame:destroy()
  enterGame = nil
  enterGameButton:destroy()
  enterGameButton = nil
  protocolBox = nil
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
  enterGame:getChildById('accountNameTextEdit'):focus()
  g_settings.remove('account')
  g_settings.remove('password')
end

function EnterGame.doLogin()
  G.account = enterGame:getChildById('accountNameTextEdit'):getText()
  G.password = enterGame:getChildById('accountPasswordTextEdit'):getText()
  G.host = enterGame:getChildById('serverHostTextEdit'):getText()
  G.port = tonumber(enterGame:getChildById('serverPortTextEdit'):getText())
  local protocolVersion = tonumber(protocolBox:getText())
  local clientVersions = g_game.getSupportedClients(protocolVersion)
  EnterGame.hide()

  if g_game.isOnline() then
    local errorBox = displayErrorBox(tr('Login Error'), tr('Cannot login while already in game.'))
    connect(errorBox, { onOk = EnterGame.show })
    return
  end

  g_settings.set('host', G.host)
  g_settings.set('port', G.port)

  protocolLogin = ProtocolLogin.create()
  protocolLogin.onLoginError = onError
  protocolLogin.onMotd = onMotd
  protocolLogin.onCharacterList = onCharacterList
  protocolLogin.onUpdateNeeded = onUpdateNeeded

  loadBox = displayCancelBox(tr('Please wait'), tr('Connecting to login server...'))
  connect(loadBox, { onCancel = function(msgbox)
                                  loadBox = nil
                                  protocolLogin:cancelLogin()
                                  EnterGame.show()
                                end })

  g_game.chooseRsa(G.host)
  g_game.setProtocolVersion(protocolVersion)
  if #clientVersions > 0 then
    g_game.setClientVersion(clientVersions[#clientVersions])
  end

  if modules.game_things.isLoaded() then
    protocolLogin:login(G.host, G.port, G.account, G.password)
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
  local protocolLabel = enterGame:getChildById('protocolLabel')
  local accountTextEdit = enterGame:getChildById('accountNameTextEdit')
  local passwordTextEdit = enterGame:getChildById('accountPasswordTextEdit')

  if hostTextEdit:getText() ~= host then
    hostTextEdit:setText(host)
    portTextEdit:setText(port)
    protocolBox:setCurrentOption(protocol)
    accountTextEdit:setText('')
    passwordTextEdit:setText('')
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

  local serverListButton = enterGame:getChildById('serverListButton')
  serverListButton:setVisible(false)
  serverListButton:setHeight(0)
  serverListButton:setWidth(0)

  local rememberPasswordBox = enterGame:getChildById('rememberPasswordBox')
  rememberPasswordBox:setMarginTop(-5)

  if not windowWidth then windowWidth = 236 end
  enterGame:setWidth(windowWidth)
  if not windowHeight then windowHeight = 200 end
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
