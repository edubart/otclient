-- private variables
local showCharacterListOnLogout = true

-- private functions
local function onGameKeyPress(self, keyCode, keyChar, keyboardModifiers)
  if keyboardModifiers == KeyboardCtrlModifier then
    if keyCode == KeyG then
      CharacterList.show()
      return true
    elseif keyCode == KeyQ then
      Game.logout()
      return true
    end
  end
  return false
end

local function createMainInterface()
  gameUi = loadUI('/game/ui/gameinterface.otui', UI.root)
  gameUi.onKeyPress = onGameKeyPress
end


local function destroyMainInterface()
  if gameUi then
    gameUi:destroy()
    gameUi = nil
  end
end

-- public functions
function Game.onLogin()
  MainMenu.hide()
  CharacterList.destroyLoadBox()
  createMainInterface()
end

function Game.onConnectionError(message)
  CharacterList.destroyLoadBox()
  local errorBox = displayErrorBox("Login Error", "Connection error: " .. message)
  errorBox.onOk = CharacterList.show
  showCharacterListOnLogout = false
end

function Game.onLogout()
  MainMenu.show()

  if showCharacterListOnLogout then
    CharacterList.show()
  else
    showCharacterListOnLogout = true
  end

  destroyMainInterface()
end
