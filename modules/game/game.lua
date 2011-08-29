-- private functions
local function onGameKeyPress(self, keyCode, keyChar, keyboardModifiers)
  if keyboardModifiers == KeyboardCtrlModifier then
    if keyCode == KeyG then
      CharacterList.show()
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

function Game.onLogin()
  MainMenu.hide()
  CharacterList.destroyLoadBox()
  createMainInterface()
end

function Game.onLogout()
  MainMenu.show()
  CharacterList.show()
  destroyMainInterface()
end
