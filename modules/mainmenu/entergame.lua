local account
local password

function EnterGame_characterWindow_okClicked()
  local charactersWindow = UI.root:getChildById('charactersWindow')
  local selected = charactersWindow:getChildById('charactersList'):getFocusedChild()
  if selected then
    Game.loginWorld(account, password, selected.worldHost, selected.worldPort, selected.characterName)
    charactersWindow:destroy()
    mainMenu:hide()
  end
end


function EnterGame_connectToLoginServer()
  local protocolLogin = ProtocolLogin.create()

  local recreateEnterGame = function()
    UI.loadAndDisplayLocked("/mainmenu/ui/entergamewindow.otui")
  end

  local loadBox = displayCancelBox("Please wait", "Connecting..")

  loadBox.onCancel = function(msgbox)
    -- cancel protocol and reacreate enter game window
    protocolLogin:cancelLogin()
    recreateEnterGame()
  end

  protocolLogin.onError = function(protocol, error)
    loadBox:destroy()
    local errorBox = displayErrorBox("Login Error", error)
    errorBox.onOk = recreateEnterGame
  end

  protocolLogin.onMotd = function(protocol, motd)
    loadBox:destroy()
    local motdNumber = string.sub(motd, 0, string.find(motd, "\n"))
    local motdText = string.sub(motd, string.find(motd, "\n") + 1, string.len(motd))
    if motdNumber ~= Configs.get("motd") then
      displayInfoBox("Message of the day", motdText)
      Configs.set("motd", motdNumber)
    end
  end

  protocolLogin.onCharacterList = function(protocol, characters, premDays)
    loadBox:destroy()
    local charactersWindow = UI.loadAndDisplayLocked('/mainmenu/ui/charlist.otui')
    local charactersList = charactersWindow:getChildById('charactersList')
    for i,characterInfo in ipairs(characters) do
      local characterName = characterInfo[1]
      local worldName = characterInfo[2]
      local worldHost = characterInfo[3]
      local worldIp = characterInfo[4]

      local label = UILabel.create()
      charactersList:addChild(label)
      label:setText(characterName .. ' (' .. worldName .. ')')
      label:setStyle('CharactersListLabel')
      label.characterName = characterName
      label.worldHost = worldHost
      label.worldPort = worldIp
    end
  end

  local enterGameWindow =  UI.root:getChildById("enterGameWindow")
  account = enterGameWindow:getChildById("accountNameLineEdit"):getText()
  password = enterGameWindow:getChildById("accountPasswordLineEdit"):getText()
  protocolLogin:login(account, password)

  enterGameWindow:destroy()
end
