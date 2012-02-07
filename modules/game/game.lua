-- private variables
local m_mouseGrabberWidget

-- private functions
local function onGameKeyPress(self, keyCode, keyboardModifiers)
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

local function onUseWithMouseRelease(self, mousePosition, mouseButton)
  if Game.selectedThing == nil then return false end
  if mouseButton == MouseLeftButton then
    local clickedWidget = Game.gameUi:recursiveGetChildByPos(mousePosition)
    if clickedWidget then
      if clickedWidget:getClassName() == 'UIMap' then
        local tile = clickedWidget:getTile(mousePosition)
        if tile then
          Game.useWith(Game.selectedThing, tile:getTopMultiUseThing())
        end
      elseif clickedWidget:getClassName() == 'UIItem' and not clickedWidget:isVirtual() then
        Game.useWith(Game.selectedThing, clickedWidget:getItem())
      end
    end
  end
  Game.selectedThing = nil
  Mouse.restoreCursor()
  self:ungrabMouse()
  return true
end

-- public functions
function Game.startUseWith(thing)
  Game.selectedThing = thing
  m_mouseGrabberWidget:grabMouse()
  Mouse.setTargetCursor()
end

function Game.createInterface()
  Background.hide()
  CharacterList.destroyLoadBox()
  Game.gameUi = displayUI('game.otui')

  --Keyboard.bindKeyPress('Up', function() Game.walk(North) end)
  --Keyboard.bindKeyPress('Down', function() Game.walk(South) end)
  --Keyboard.bindKeyPress('Left', function() Game.walk(West) end)
  --Keyboard.bindKeyPress('Right', function() Game.walk(East) end)

  Keyboard.bindKeyPress('Ctrl+Shift+Up', function() Game.forceWalk(North) end)
  Keyboard.bindKeyPress('Ctrl+Shift+Down', function() Game.forceWalk(South) end)
  Keyboard.bindKeyPress('Ctrl+Shift+Left', function() Game.forceWalk(West) end)
  Keyboard.bindKeyPress('Ctrl+Shift+Right', function() Game.forceWalk(East) end)

  rootWidget:moveChildToIndex(Game.gameUi, 1)
  Game.gameMapPanel = Game.gameUi:getChildById('gameMapPanel')
  Game.gameRightPanel = Game.gameUi:getChildById('gameRightPanel')
  Game.gameBottomPanel = Game.gameUi:getChildById('gameBottomPanel')
  m_mouseGrabberWidget = Game.gameUi:getChildById('mouseGrabber')
  connect(Game.gameUi, { onKeyPress = onGameKeyPress })
  connect(m_mouseGrabberWidget, { onMouseRelease = onUseWithMouseRelease })
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
  connect(errorBox, { onOk = CharacterList.show })
end

function Game.onConnectionError(message)
  CharacterList.destroyLoadBox()
  local errorBox = displayErrorBox("Login Error", "Connection error: " .. message)
  connect(errorBox, { onOk = CharacterList.show })
end

local function onApplicationClose()
  if Game.isOnline() then
    Game.logout(true)
  else
    exit()
  end
end

setonclose(onApplicationClose)
connect(Game, { onGameStart = Game.createInterface }, true)
connect(Game, { onGameEnd = Game.destroyInterface })
