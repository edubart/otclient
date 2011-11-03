require 'textmessage'

-- private functions
local function onGameKeyPress(self, keyCode, keyChar, keyboardModifiers)
  if keyboardModifiers == KeyboardCtrlModifier then
    if keyCode == KeyG then
      CharacterList.show()
      return true
    elseif keyCode == KeyQ then
      Game.logout(false)
      return true
    end
  end
  return false
end

-- public functions
function Game.createInterface()
  Background.hide()
  CharacterList.destroyLoadBox()
  Game.gameUi = loadUI('/game/ui/gameinterface.otui', UI.root)
  Game.gameMapPanel = Game.gameUi:getChildById('mapPanel')
  Game.gameRightPanel = Game.gameUi:getChildById('rightPanel')
  Game.gameBottomPanel = Game.gameUi:getChildById('bottomPanel')
  Game.gameUi.onKeyPress = onGameKeyPress
end

function Game.destroyInterface()
  if Game.gameUi then
    Game.gameUi:destroy()
    Game.gameUi = nil
  end
  Background.show()
  CharacterList.show()
end

function Game.show()
  Game.gameUi:show()
  Game.gameUi:focus()
  Game.gameMapPanel:focus()
end

function Game.hide()
  Game.gameUi:hide()
end

-- hooked events
function Game.onLoginError(message)
  CharacterList.destroyLoadBox()
  local errorBox = displayErrorBox("Login Error", "Login error: " .. message)
  errorBox.onOk = CharacterList.show
end

function Game.onConnectionError(message)
  CharacterList.destroyLoadBox()
  local errorBox = displayErrorBox("Login Error", "Connection error: " .. message)
  errorBox.onOk = CharacterList.show
end

connect(Game, { onLogin = Game.createInterface,
                onLogout = Game.destroyInterface })
