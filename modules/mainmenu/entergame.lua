local account
local password

function EnterGame_characterWindow_okClicked()
  local charactersWindow = UI.root:getChildById('charactersWindow')
  local selected = charactersWindow:getChildById('charactersList'):getFocusedChild()
  if selected then
    Game.loginWorld(account, password, selected.worldHost, selected.worldPort, selected.characterName)
    Configs.set('lastUsedCharacter', selected.characterName)
    charactersWindow:destroy()
    mainMenu:hide()
  else
    displayErrorBox('Error', 'You must select a character to login!')
  end
end

local function showMotd(motdNumber, motdMessage)
  if motdNumber ~= Configs.get("motd") then
    displayInfoBox("Message of the day", motdMessage)
    Configs.set("motd", motdNumber)
  end
end

local function createCharactersWindow(characters, premDays)
  local charactersWindow = UI.loadAndDisplayLocked('/mainmenu/ui/charlist.otui')
  local charactersList = charactersWindow:getChildById('charactersList')
  local accountStatusLabel = charactersWindow:getChildById('accountStatusLabel')

  for i,characterInfo in ipairs(characters) do
    local characterName = characterInfo[1]
    local worldName = characterInfo[2]
    local worldHost = characterInfo[3]
    local worldIp = characterInfo[4]

    local label = UILabel.create()
    charactersList:addChild(label)
    label:setText(characterName .. '  (' .. worldName .. ')')
    label:setStyle('CharactersListLabel')
    label.characterName = characterName
    label.worldHost = worldHost
    label.worldPort = worldIp

    if i == 0 or Configs.get('lastUsedCharacter') == characterName then
      charactersList:focusChild(label, ActiveFocusReason)
    end
  end
  if premDays > 0 then
    accountStatusLabel:setText("Account Status:\nPremium Account (" .. premDays .. ' days left)')
  end
end

function EnterGame_connectToLoginServer()
  local protocolLogin = ProtocolLogin.create()

  local recreateEnterGame = function()
    UI.loadAndDisplayLocked('/mainmenu/ui/entergamewindow.otui')
  end

  local loadBox = displayCancelBox('Please wait', 'Connecting to login server...')

  loadBox.onCancel = function(msgbox)
    -- cancel protocol and reacreate enter game window
    protocolLogin:cancelLogin()
    recreateEnterGame()
  end

  protocolLogin.onError = function(protocol, error)
    loadBox:destroy()
    local errorBox = displayErrorBox('Login Error', error)
    errorBox.onOk = recreateEnterGame
  end

  protocolLogin.onMotd = function(protocol, motd)
    loadBox:destroy()
    local motdNumber = string.sub(motd, 0, string.find(motd, "\n"))
    local motdMessage = string.sub(motd, string.find(motd, "\n") + 1, string.len(motd))
    showMotd(motdNumber, motdMessage)
  end

  protocolLogin.onCharacterList = function(protocol, characters, premDays)
    loadBox:destroy()
    createCharactersWindow(characters, premDays)
  end

  local enterGameWindow =  UI.root:getChildById('enterGameWindow')
  account = enterGameWindow:getChildById('accountNameLineEdit'):getText()
  password = enterGameWindow:getChildById('accountPasswordLineEdit'):getText()
  protocolLogin:login(account, password)

  enterGameWindow:destroy()
end
