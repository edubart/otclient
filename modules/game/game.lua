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
function Game.create()
  Game.gameUi = loadUI('/game/ui/gameinterface.otui', UI.root)
  Game.gameMapPanel = Game.gameUi:getChildById('mapPanel')
  Game.gameRightPanel = Game.gameUi:getChildById('rightPanel')
  Game.gameUi.onKeyPress = onGameKeyPress

  TextMessage.create()
end

function Game.destroy()
  if Game.gameUi then
    Game.gameUi:destroy()
    Game.gameUi = nil
  end
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
function Game.onLogin()
  Background.hide()
  CharacterList.destroyLoadBox()
  Game.show()
end

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

function Game.onLogout()
  Game.hide()
  Background.show()
  CharacterList.show()
end
