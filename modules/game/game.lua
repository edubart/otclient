-- private variables
local m_mouseGrabberWidget

-- private functions
local function onGameKeyPress(self, keyCode, keyboardModifiers)
  if keyboardModifiers == KeyboardCtrlModifier then
    if keyCode == KeyG then
      CharacterList.show()
      return true
    elseif keyCode == KeyQ then
      g_game.safeLogout()
      return true
    end
  end
  return false
end

local function onUseWithMouseRelease(self, mousePosition, mouseButton)
  if g_game.selectedThing == nil then return false end
  if mouseButton == MouseLeftButton then
    local clickedWidget = g_game.gameUi:recursiveGetChildByPos(mousePosition)
    if clickedWidget then
      if clickedWidget:getClassName() == 'UIMap' then
        local tile = clickedWidget:getTile(mousePosition)
        if tile then
          g_game.useWith(g_game.selectedThing, tile:getTopMultiUseThing())
        end
      elseif clickedWidget:getClassName() == 'UIItem' and not clickedWidget:isVirtual() then
        g_game.useWith(g_game.selectedThing, clickedWidget:getItem())
      elseif clickedWidget.isBattleButton then
        g_game.useWith(g_game.selectedThing, clickedWidget.creature)
      end
    end
  end
  g_game.selectedThing = nil
  Mouse.restoreCursor()
  self:ungrabMouse()
  return true
end

-- public functions
function g_game.startUseWith(thing)
  g_game.selectedThing = thing
  m_mouseGrabberWidget:grabMouse()
  Mouse.setTargetCursor()
end

function g_game.createInterface()

  rootWidget:moveChildToIndex(g_game.gameUi, 1)
  g_game.gameMapPanel = g_game.gameUi:getChildById('gameMapPanel')
  g_game.gameRightPanel = g_game.gameUi:getChildById('gameRightPanel')
  g_game.gameBottomPanel = g_game.gameUi:getChildById('gameBottomPanel')
  m_mouseGrabberWidget = g_game.gameUi:getChildById('mouseGrabber')
  connect(g_game.gameUi, { onKeyPress = onGameKeyPress })
  connect(m_mouseGrabberWidget, { onMouseRelease = onUseWithMouseRelease })
end

function g_game.destroyInterface()
  if g_game.gameUi then
    g_game.gameUi:destroy()
    g_game.gameUi = nil
  end
  Background.show()
  CharacterList.show()
end

function g_game.show()
  g_game.gameUi:show()
  g_game.gameUi:focus()
  g_game.gameMapPanel:focus()
end

function g_game.hide()
  g_game.gameUi:hide()
end

-- hooked events

local function onApplicationClose()
  if g_game.isOnline() then
    g_game.forceLogout()
  else
    exit()
  end
end

setonclose(onApplicationClose)
connect(g_game, { onGameStart = g_game.createInterface }, true)
connect(g_game, { onGameEnd = g_game.destroyInterface })
