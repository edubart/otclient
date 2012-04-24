GameInterface = {}

local WALK_AUTO_REPEAT_DELAY = 90
local gameRootPanel
local gameMapPanel
local gameRightPanel
local gameLeftPanel
local gameBottomPanel
local logoutButton
local mouseGrabberWidget

function GameInterface.init()
  connect(g_game, { onGameStart = GameInterface.show }, true)
  connect(g_game, { onGameEnd = GameInterface.hide }, true)

  gameRootPanel = displayUI('gameinterface.otui')
  gameRootPanel:hide()
  gameRootPanel:lower()

  mouseGrabberWidget = gameRootPanel:getChildById('mouseGrabber')
  mouseGrabberWidget.onMouseRelease = GameInterface.onUseWithMouseRelease

  gameMapPanel = gameRootPanel:getChildById('gameMapPanel')
  gameRightPanel = gameRootPanel:getChildById('gameRightPanel')
  gameLeftPanel = gameRootPanel:getChildById('gameLeftPanel')
  gameBottomPanel = gameRootPanel:getChildById('gameBottomPanel')

  logoutButton = TopMenu.addRightButton('logoutButton', 'Logout', 'images/logout.png', GameInterface.tryLogout)
  logoutButton:hide()

  Keyboard.bindKeyPress('Up', function() g_game.walk(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Right', function() g_game.walk(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Down', function() g_game.walk(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Left', function() g_game.walk(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad8', function() g_game.walk(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad9', function() g_game.walk(NorthEast) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad6', function() g_game.walk(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad3', function() g_game.walk(SouthEast) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad2', function() g_game.walk(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad1', function() g_game.walk(SouthWest) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad4', function() g_game.walk(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad7', function() g_game.walk(NorthWest) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Up', function() g_game.turn(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Right', function() g_game.turn(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Down', function() g_game.turn(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Left', function() g_game.turn(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Numpad8', function() g_game.turn(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Numpad6', function() g_game.turn(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Numpad2', function() g_game.turn(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Numpad4', function() g_game.turn(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Escape', function() g_game.cancelAttackAndFollow() end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+=', function() gameMapPanel:zoomIn() end, gameRootPanel, 250)
  Keyboard.bindKeyPress('Ctrl+-', function() gameMapPanel:zoomOut() end, gameRootPanel, 250)
  Keyboard.bindKeyDown('Ctrl+Q', GameInterface.tryLogout, gameRootPanel)

  if g_game.isOnline() then
    GameInterface.show()
  end
end

function GameInterface.terminate()
  disconnect(g_game, { onGameStart = GameInterface.show })
  disconnect(g_game, { onGameEnd = GameInterface.hide })

  logoutButton:destroy()
  logoutButton = nil
  gameRootPanel:destroy()
  gameRootPanel = nil
  gameMapPanel = nil
  gameRightPanel = nil
  gameLeftPanel = nil
  gameBottomPanel = nil
  mouseGrabberWidget = nil
  GameInterface = nil
end

function GameInterface.show()
  g_app.onClose = GameInterface.tryExit
  logoutButton:show()
  Background.hide()
  gameRootPanel:show()
  gameRootPanel:focus()
  gameMapPanel:followCreature(g_game.getLocalPlayer())
end

function GameInterface.hide()
  gameRootPanel:hide()
  logoutButton:hide()
  Background.show()
  g_app.onClose = nil
end

function GameInterface.tryExit()
  if g_game.isOnline() then
    g_game.forceLogout()
    scheduleEvent(exit, 10)
  end
end

function GameInterface.tryLogout()
  if g_game.isOnline() then
    g_game.safeLogout()
    return true
  end
end

function GameInterface.onUseWithMouseRelease(self, mousePosition, mouseButton)
  if GameInterface.selectedThing == nil then return false end
  if mouseButton == MouseLeftButton then
    local clickedWidget = gameRootPanel:recursiveGetChildByPos(mousePosition, false)
    if clickedWidget then
      if clickedWidget:getClassName() == 'UIMap' then
        local tile = clickedWidget:getTile(mousePosition)
        if tile then
          g_game.useWith(GameInterface.selectedThing, tile:getTopMultiUseThing())
        end
      elseif clickedWidget:getClassName() == 'UIItem' and not clickedWidget:isVirtual() then
        g_game.useWith(GameInterface.selectedThing, clickedWidget:getItem())
      end
    end
  end
  GameInterface.selectedThing = nil
  Mouse.restoreCursor()
  self:ungrabMouse()
  return true
end

function GameInterface.startUseWith(thing)
  GameInterface.selectedThing = thing
  mouseGrabberWidget:grabMouse()
  Mouse.setTargetCursor()
end

function GameInterface.createThingMenu(menuPosition, lookThing, useThing, creatureThing)
  local menu = createWidget('PopupMenu')

  if lookThing then
    menu:addOption('Look', function() g_game.look(lookThing) end)
  end

  if useThing then
    if useThing:isContainer() then
      if useThing:getParentContainer() then
        menu:addOption('Open', function() g_game.open(useThing, useThing:getParentContainer()) end)
        menu:addOption('Open in new window', function() g_game.open(useThing, nil) end)
      else
        menu:addOption('Open', function() g_game.open(useThing, nil) end)
      end
    else
      if useThing:isMultiUse() then
        menu:addOption('Use with ...', function() GameInterface.startUseWith(useThing) end)
      else
        menu:addOption('Use', function() g_game.use(useThing) end)
      end
    end

    if useThing:isRotateable() then
      menu:addOption('Rotate', function() g_game.rotate(useThing) end)
    end

  end

  if lookThing and not lookThing:asCreature() and not lookThing:isNotMoveable() and lookThing:isPickupable() then
    menu:addSeparator()
    menu:addOption('Trade with ...', function() print('trade with') end)
  end

  -- check for move up

  if creatureThing then
    menu:addSeparator()

    if creatureThing:asLocalPlayer() then
      menu:addOption('Set Outfit', function() g_game.requestOutfit() end)

      if creatureThing:isPartyMember() --[[and not fighting]] then
        if creatureThing:isPartyLeader() then
          if creatureThing:isPartySharedExperienceActive() then
            menu:addOption('Disable Shared Experience', function() g_game.partyShareExperience(false) end)
          else
            menu:addOption('Enable Shared Experience', function() g_game.partyShareExperience(true) end)
          end
        end
        menu:addOption('Leave Party', function() g_game.partyLeave() end)
      end

    else
      local localPlayer = g_game.getLocalPlayer()
      if localPlayer then
        if g_game.getAttackingCreature() ~= creatureThing then
          menu:addOption('Attack', function() g_game.attack(creatureThing) end)
        else
          menu:addOption('Stop Attack', function() g_game.cancelAttack() end)
        end

        if g_game.getFollowingCreature() ~= creatureThing then
          menu:addOption('Follow', function() g_game.follow(creatureThing) end)
        else
          menu:addOption('Stop Follow', function() g_game.cancelFollow() end)
        end

        if creatureThing:asPlayer() then
          menu:addSeparator()
          local creatureName = creatureThing:getName()
          menu:addOption('Message to ' .. creatureName, function() g_game.openPrivateChannel(creatureName) end)
          if Console.getOwnPrivateTab() then
            menu:addOption('Invite to private chat', function() g_game.inviteToOwnChannel(creatureName) end)
            menu:addOption('Exclude from private chat', function() g_game.excludeFromOwnChannel(creatureName) end) -- [TODO] must be removed after message's popup labels been implemented
          end
          menu:addOption('Add to VIP list', function() g_game.addVip(creatureName) end)

          local localPlayerShield = localPlayer:asCreature():getShield()
          local creatureShield = creatureThing:getShield()

          if localPlayerShield == ShieldNone or localPlayerShield == ShieldWhiteBlue then
            if creatureShield == ShieldWhiteYellow then
              menu:addOption('Join ' .. creatureThing:getName() .. '\'s Party', function() g_game.partyJoin(creatureThing:getId()) end)
            else
              menu:addOption('Invite to Party', function() g_game.partyInvite(creatureThing:getId()) end)
            end
          elseif localPlayerShield == ShieldWhiteYellow then
            if creatureShield == ShieldWhiteBlue then
              menu:addOption('Revoke ' .. creatureThing:getName() .. '\'s Invitation', function() g_game.partyRevokeInvitation(creatureThing:getId()) end)
            end
          elseif localPlayerShield == ShieldYellow or localPlayerShield == ShieldYellowSharedExp or localPlayerShield == ShieldYellowNoSharedExpBlink or localPlayerShield == ShieldYellowNoSharedExp then
            if creatureShield == ShieldWhiteBlue then
              menu:addOption('Revoke ' .. creatureThing:getName() .. '\'s Invitation', function() g_game.partyRevokeInvitation(creatureThing:getId()) end)
            elseif creatureShield == ShieldBlue or creatureShield == ShieldBlueSharedExp or creatureShield == ShieldBlueNoSharedExpBlink or creatureShield == ShieldBlueNoSharedExp then
              menu:addOption('Pass Leadership to ' .. creatureThing:getName(), function() g_game.partyPassLeadership(creatureThing:getId()) end)
            else
              menu:addOption('Invite to Party', function() g_game.partyInvite(creatureThing:getId()) end)
            end
          end
        end
      end
    end

    menu:addSeparator()
    menu:addOption('Copy Name', function() g_window.setClipboardText(creatureThing:getName()) end)

  end

  menu:display(menuPosition)
end

function GameInterface.processMouseAction(menuPosition, mouseButton, autoWalk, lookThing, useThing, creatureThing, multiUseThing)
  local keyboardModifiers = g_window.getKeyboardModifiers()

  if autoWalk and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseLeftButton then
    -- todo auto walk
    return true
  end

  if not Options.getOption('classicControl') then
    if keyboardModifiers == KeyboardNoModifier and mouseButton == MouseRightButton then
      GameInterface.createThingMenu(menuPosition, lookThing, useThing, creatureThing)
      return true
    elseif lookThing and keyboardModifiers == KeyboardShiftModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.look(lookThing)
      return true
    elseif useThing and keyboardModifiers == KeyboardCtrlModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      if useThing:isContainer() then
        if useThing:getParentContainer() then
          g_game.open(useThing, useThing:getParentContainer())
          return true
        else
          g_game.open(useThing, nil)
        return true
        end
      elseif useThing:isMultiUse() then
        GameInterface.startUseWith(useThing)
        return true
      else
        g_game.use(useThing)
        return true
      end
      return true
    elseif creatureThing and keyboardModifiers == KeyboardAltModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.attack(creatureThing)
      return true
    end
  else
    if multiUseThing and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseRightButton then
      if multiUseThing:asCreature() then
        g_game.attack(multiUseThing:asCreature())
        return true
      elseif multiUseThing:isContainer() then
        if multiUseThing:getParentContainer() then
          g_game.open(multiUseThing, multiUseThing:getParentContainer())
          return true
        else
          g_game.open(multiUseThing, nil)
          return true
        end
      elseif multiUseThing:isMultiUse() then
        GameInterface.startUseWith(useThing)
        return true
      else
        g_game.use(multiUseThing)
      end
      return true
    elseif lookThing and keyboardModifiers == KeyboardShiftModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.look(lookThing)
      return true
    elseif useThing and keyboardModifiers == KeyboardCtrlModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      GameInterface.createThingMenu(menuPosition, lookThing, useThing, creatureThing)
      return true
    elseif creatureThing and keyboardModifiers == KeyboardAltModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.attack(creatureThing)
      return true
    end
  end

  return false
end

function GameInterface.moveStackableItem(item, toPos)
  local count = item:getCount()

  local countWindow = createWidget('CountWindow', rootWidget)
  local spinbox = countWindow:getChildById('countSpinBox')
  local scrollbar = countWindow:getChildById('countScrollBar')
  spinbox:setMaximum(count)
  spinbox:setMinimum(1)
  spinbox:setValue(count)
  scrollbar:setMaximum(count)
  scrollbar:setMinimum(1)
  scrollbar:setValue(count)
  scrollbar.onValueChange = function(self, value) spinbox:setValue(value) end
  spinbox.onValueChange = function(self, value) scrollbar:setValue(value) end

  local okButton = countWindow:getChildById('buttonOk')
  local moveFunc = function()
    g_game.move(item, toPos, spinbox:getValue())
    okButton:getParent():destroy()
  end

  countWindow.onEnter = moveFunc
  okButton.onClick = moveFunc
end

function GameInterface.getRootPanel()
  return gameRootPanel
end

function GameInterface.getMapPanel()
  return gameMapPanel
end

function GameInterface.getRightPanel()
  return gameRightPanel
end

function GameInterface.getLeftPanel()
  return gameLeftPanel
end

function GameInterface.getBottomPanel()
  return gameBottomPanel
end
