CharacterList = { }

-- private variables
local charactersWindow
local loadBox
local characterList

-- private functions
local function onCharactersWindowKeyPress(self, keyCode, keyboardModifiers)
  if keyboardModifiers == KeyboardNoModifier then
    if keyCode == KeyUp then
      characterList:focusPreviousChild(KeyboardFocusReason)
      return true
    elseif keyCode == KeyDown or keyCode == KeyTab then
      characterList:focusNextChild(KeyboardFocusReason)
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

  if g_game.isOnline() then
    g_game.safeLogout()
    if tries == 1 then
      loadBox = displayCancelBox('Please wait', 'Loggin out...')
    end
    scheduleEvent(function() tryLogin(charInfo, tries+1) end, 250)
    return
  end

  CharacterList.destroyLoadBox()

  g_game.loginWorld(EnterGame.account, EnterGame.password, charInfo.worldHost, charInfo.worldPort, charInfo.characterName)

  loadBox = displayCancelBox('Please wait', 'Connecting to game server...')
  connect(loadBox, { onCancel = function()
                                  loadBox = nil
                                  g_game.cancelLogin()
                                  CharacterList.show()
                                end })

  -- save last used character
  Settings.set('lastUsedCharacter', charInfo.characterName)
end

function onGameLoginError(message)
  CharacterList.destroyLoadBox()
  local errorBox = displayErrorBox("Login Error", "Login error: " .. message)
  errorBox.onOk = CharacterList.showAgain
end

function onGameConnectionError(message)
  CharacterList.destroyLoadBox()
  local errorBox = displayErrorBox("Login Error", "Connection error: " .. message)
  errorBox.onOk = CharacterList.showAgain
end

-- public functions
function CharacterList.init()
  charactersWindow = displayUI('characterlist.otui')
  charactersWindow:hide()
  characterList = charactersWindow:getChildById('characterList')
  charactersWindow.onKeyPress = onCharactersWindowKeyPress
  connect(g_game, { onLoginError = onGameLoginError })
  connect(g_game, { onConnectionError = onGameConnectionError })
  connect(g_game, { onGameStart = CharacterList.destroyLoadBox })
  connect(g_game, { onGameEnd = CharacterList.showAgain })
end

function CharacterList.terminate()
  disconnect(g_game, { onLoginError = onGameLoginError })
  disconnect(g_game, { onConnectionError = onGameConnectionError })
  disconnect(g_game, { onGameStart = CharacterList.destroyLoadBox })
  disconnect(g_game, { onGameEnd = CharacterList.showAgain })
  characterList = nil
  charactersWindow:destroy()
  charactersWindow = nil
  if loadBox then
    g_game.cancelLogin()
    loadBox:destroy()
    loadBox = nil
  end
  CharacterList = nil
end

function CharacterList.create(characters, premDays)
  CharacterList.show()
  characterList:destroyChildren()
  local accountStatusLabel = charactersWindow:getChildById('accountStatusLabel')

  local focusLabel
  for i,characterInfo in ipairs(characters) do
    local characterName = characterInfo[1]
    local worldName = characterInfo[2]
    local worldHost = characterInfo[3]
    local worldIp = characterInfo[4]

    local label = createWidget('CharacterListLabel', characterList)
    label:setText(characterName .. '  (' .. worldName .. ')')
    label:setPhantom(false)
    label.characterName = characterName
    label.worldHost = worldHost
    label.worldPort = worldIp

    connect(label, { onDoubleClick = function () CharacterList.doLogin() return true end } )

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
  if not g_game.isOnline() then
    EnterGame.show()
  end
end

function CharacterList.show()
  if not loadBox then
    charactersWindow:show()
    charactersWindow:raise()
    charactersWindow:focus()
  end
end

function CharacterList.showAgain()
  if characterList:hasChildren() then
    CharacterList.show()
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
