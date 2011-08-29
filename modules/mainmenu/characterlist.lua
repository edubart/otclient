CharacterList = { }

-- private variables
local charactersWindow
local loadBox
local characterList

-- private functions
local function onCharactersWindowKeyPress(self, keyCode, keyChar, keyboardModifiers)
  if keyboardModifiers == KeyboardNoModifier then
    if keyCode == KeyUp or keyCode == KeyTab then
      characterList:focusPreviousChild(ActiveFocusReason)
    elseif keyCode == KeyDown then
      characterList:focusNextChild(ActiveFocusReason)
    end
  end
  return false
end

local function tryLogin(charInfo, tries)
  tries = tries or 1

  if tries > 4 then
    CharacterList.destroyLoadBox()
    displayErrorBox('Error', 'Could not logout.')
    return
  end

  if Game.isOnline() then
    Game.logout(false)
    if tries == 1 then
      loadBox = displayCancelBox('Please wait', 'Loggin out...')
    end
    scheduleEvent(function() tryLogin(charInfo, tries+1) end, 250)
    return
  end

  CharacterList.destroyLoadBox()

  Game.loginWorld(EnterGame.account, EnterGame.password, charInfo.worldHost, charInfo.worldPort, charInfo.characterName)

  loadBox = displayCancelBox('Please wait', 'Connecting to game server...')
  function loadBox.onCancel()
    loadBox = nil
    Game.cancelLogin()
    CharacterList.show()
  end

  -- save last used character
  Configs.set('lastUsedCharacter', charInfo.characterName)
end

-- public functions
function CharacterList.create(characters, premDays)
  if charactersWindow then
    charactersWindow:destroy()
  end

  charactersWindow = UI.loadAndDisplayLocked('/mainmenu/ui/charlist.otui')
  characterList = charactersWindow:getChildById('characterList')
  local accountStatusLabel = charactersWindow:getChildById('accountStatusLabel')
  charactersWindow.onKeyPress = onCharactersWindowKeyPress

  for i,characterInfo in ipairs(characters) do
    local characterName = characterInfo[1]
    local worldName = characterInfo[2]
    local worldHost = characterInfo[3]
    local worldIp = characterInfo[4]

    local label = UILabel.create()
    characterList:addChild(label)
    label:setText(characterName .. '  (' .. worldName .. ')')
    label:setStyle('CharacterListLabel')
    label.characterName = characterName
    label.worldHost = worldHost
    label.worldPort = worldIp

    if i == 0 or Configs.get('lastUsedCharacter') == characterName then
      characterList:focusChild(label, ActiveFocusReason)
    end
  end

  if premDays > 0 then
    accountStatusLabel:setText("Account Status:\nPremium Account (" .. premDays .. ' days left)')
  end
end

function CharacterList.hide()
  charactersWindow:unlock()
  charactersWindow:hide()
end

function CharacterList.show()
  if not loadBox then
    charactersWindow:show()
    charactersWindow:lock()
  end
end

function CharacterList.doLogin()
  local selected = charactersWindow:getChildById('characterList'):getFocusedChild()
  if selected then
    local charInfo = { worldHost = selected.worldHost,
                       worldPort = selected.worldPort,
                       characterName = selected.characterName }
    CharacterList.hide()
    tryLogin(charInfo)
  else
    displayErrorBox('Error', 'You must select a character to login!')
  end
end

function CharacterList.destroyLoadBox()
  if loadBox then
    loadBox:destroy()
    loadBox = nil
  end
end
