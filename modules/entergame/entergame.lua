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
    Configs.set('account', nil)
    Configs.set('password', nil)
    enterGame:getChildById('accountNameLineEdit'):clearText()
    enterGame:getChildById('accountPasswordLineEdit'):clearText()
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
  enterGame = UI.loadAndDisplay('/entergame/entergame.otui')

  local account = Configs.get('account')
  local password = Configs.get('password')
  local autologin = toboolean(Configs.get('autologin'))

  enterGame:getChildById('accountNameLineEdit'):setText(account)
  enterGame:getChildById('accountPasswordLineEdit'):setText(password)

  if #account > 0 then
    enterGame:getChildById('rememberPasswordBox'):setChecked(true)
    if autologin then
        enterGame:getChildById('autoLoginBox'):setChecked(true)
        addEvent(EnterGame.doLogin)
    end
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

function EnterGame.displayMotd()
  displayInfoBox("Message of the day", motdMessage)
end
