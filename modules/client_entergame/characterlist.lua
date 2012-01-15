CharacterList = { }

-- private variables
local charactersWindow
local loadBox
local characterList

-- private functions
local function onCharactersWindowKeyPress(self, keyCode, keyboardModifiers, wouldFilter)
  if wouldFilter then return end
  if keyboardModifiers == KeyboardNoModifier then
    if keyCode == KeyUp then
      characterList:focusPreviousChild(ActiveFocusReason)
      return true
    elseif keyCode == KeyDown or keyCode == KeyTab then
      characterList:focusNextChild(ActiveFocusReason)
      return true
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
  Settings.set('lastUsedCharacter', charInfo.characterName)
end

-- public functions
function CharacterList.create(characters, premDays)
  if charactersWindow then
    charactersWindow:destroy()
  end

  charactersWindow = displayUI('characterlist.otui')
  characterList = charactersWindow:getChildById('characterList')
  local accountStatusLabel = charactersWindow:getChildById('accountStatusLabel')
  connect(charactersWindow, {onKeyPress = onCharactersWindowKeyPress })

  local focusLabel
  for i,characterInfo in ipairs(characters) do
    local characterName = characterInfo[1]
    local worldName = characterInfo[2]
    local worldHost = characterInfo[3]
    local worldIp = characterInfo[4]

    local label = createWidget('CharacterListLabel', characterList)
    label:setText(characterName .. '  (' .. worldName .. ')')
    label.characterName = characterName
    label.worldHost = worldHost
    label.worldPort = worldIp

    if i == 1 or Settings.get('lastUsedCharacter') == characterName then
      focusLabel = label
    end
  end

  characterList:focusChild(focusLabel, ActiveFocusReason)

  if premDays > 0 then
    accountStatusLabel:setText("Account Status:\nPremium Account (" .. premDays .. ' days left)')
  end
end

function CharacterList.hide()
  charactersWindow:hide()
end

function CharacterList.destroy()
  CharacterList.hide()
  if not Game.isOnline() then
    EnterGame.show()
  end
end

function CharacterList.show()
  if not loadBox then
    charactersWindow:show()
    charactersWindow:focus()
  end
end

function CharacterList.isVisible()
  if charactersWindow and charactersWindow:isVisible() then
    return true
  end
  return false
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
