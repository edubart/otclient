WALK_AUTO_REPEAT_DELAY = 90

gameRootPanel = nil
gameMapPanel = nil
gameRightPanel = nil
gameLeftPanel = nil
gameBottomPanel = nil
logoutButton = nil
mouseGrabberWidget = nil
countWindow = nil
logoutWindow = nil
exitWindow = nil

function init()
  g_ui.importStyle('styles/countwindow.otui')

  connect(g_game, { onGameStart = show,
                    onGameEnd = hide }, true)

  gameRootPanel = g_ui.displayUI('gameinterface.otui')
  gameRootPanel:hide()
  gameRootPanel:lower()

  mouseGrabberWidget = gameRootPanel:getChildById('mouseGrabber')
  mouseGrabberWidget.onMouseRelease = onMouseGrabberRelease

  gameMapPanel = gameRootPanel:getChildById('gameMapPanel')
  gameRightPanel = gameRootPanel:getChildById('gameRightPanel')
  gameLeftPanel = gameRootPanel:getChildById('gameLeftPanel')
  gameBottomPanel = gameRootPanel:getChildById('gameBottomPanel')
  connect(gameLeftPanel, { onVisibilityChange = onLeftPanelVisibilityChange })

  logoutButton = TopMenu.addRightButton('logoutButton', 'Logout', '/images/logout.png', tryLogout)
  logoutButton:hide()

  g_keyboard.bindKeyPress('Up', function() g_game.walk(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Right', function() g_game.walk(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Down', function() g_game.walk(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Left', function() g_game.walk(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad8', function() g_game.walk(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad9', function() g_game.walk(NorthEast) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad6', function() g_game.walk(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad3', function() g_game.walk(SouthEast) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad2', function() g_game.walk(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad1', function() g_game.walk(SouthWest) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad4', function() g_game.walk(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad7', function() g_game.walk(NorthWest) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Ctrl+Up', function() g_game.turn(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Ctrl+Right', function() g_game.turn(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Ctrl+Down', function() g_game.turn(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Ctrl+Left', function() g_game.turn(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Ctrl+Numpad8', function() g_game.turn(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Ctrl+Numpad6', function() g_game.turn(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Ctrl+Numpad2', function() g_game.turn(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Ctrl+Numpad4', function() g_game.turn(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Escape', function() g_game.cancelAttackAndFollow() end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Ctrl+=', function() gameMapPanel:zoomIn() end, gameRootPanel, 250)
  g_keyboard.bindKeyPress('Ctrl+-', function() gameMapPanel:zoomOut() end, gameRootPanel, 250)
  g_keyboard.bindKeyDown('Ctrl+Q', logout, gameRootPanel)
  g_keyboard.bindKeyDown('Ctrl+L', logout, gameRootPanel)
  g_keyboard.bindKeyDown('Ctrl+W', function() g_map.cleanTexts() modules.game_textmessage.clearMessages() end, gameRootPanel)

  g_keyboard.bindKeyDown('Ctrl+.', function()
    if gameMapPanel:isKeepAspectRatioEnabled() then
      gameMapPanel:setKeepAspectRatio(false)
    else
      gameMapPanel:setKeepAspectRatio(true)
      gameMapPanel:setVisibleDimension({ width = 15, height = 11 })
    end
  end, gameRootPanel)

  if g_game.isOnline() then
    show()
  end
end

function terminate()
  disconnect(g_game, { onGameStart = show,
                       onGameEnd = hide })
  disconnect(gameLeftPanel, { onVisibilityChange = onLeftPanelVisibilityChange })

  logoutButton:destroy()
  gameRootPanel:destroy()
end

function show()
  connect(g_app, { onClose = tryExit })
  logoutButton:show()
  Background.hide()
  gameRootPanel:show()
  gameRootPanel:focus()
  gameMapPanel:followCreature(g_game.getLocalPlayer())
end

function hide()
  disconnect(g_app, { onClose = tryExit })
  if logoutWindow then
    logoutWindow:destroy()
    logoutWindow = nil
  end
  if exitWindow then
    exitWindow:destroy()
    exitWindow = nil
  end
  if countWindow then
    countWindow:destroy()
    countWindow = nil
  end
  gameRootPanel:hide()
  logoutButton:hide()
  Background.show()
end

function forceExit()
  scheduleEvent(exit, 10)
  return true
end

function tryExit()
  if exitWindow then
    return true
  end

  local exitFunc = function() logout() forceExit() end
  local logoutFunc = function() logout() exitWindow:destroy() exitWindow = nil end
  local cancelFunc = function() exitWindow:destroy() exitWindow = nil end

  exitWindow = displayGeneralBox('Exit', tr("If you shut down the program, your character might stay in the game.\nClick on 'Logout' to ensure that you character leaves the game properly.\nClick on 'Exit' if you want to exit the program without logging out your character."),
  { { text='Force Exit', callback=exitFunc },
    { text='Logout', callback=logoutFunc },
    { text='Cancel', callback=cancelFunc },
    anchor=AnchorHorizontalCenter }, logoutFunc, cancelFunc)

  return true
end

function logout()
  if g_game.isOnline() then
    g_game.safeLogout()
    return true
  end
end

function tryLogout()
  if logoutWindow then
    return
  end

  local yesCallback = function() logout() logoutWindow:destroy() logoutWindow=nil end
  local noCallback = function() logoutWindow:destroy() logoutWindow=nil end

  logoutWindow = displayGeneralBox('Logout', tr('Are you sure you want to logout?'), {
    { text='Yes', callback=yesCallback },
    { text='No', callback=noCallback },
    anchor=AnchorHorizontalCenter}, yesCallback, noCallback)
end

function onMouseGrabberRelease(self, mousePosition, mouseButton)
  if selectedThing == nil then return false end
  if mouseButton == MouseLeftButton then
    local clickedWidget = gameRootPanel:recursiveGetChildByPos(mousePosition, false)
    if clickedWidget then
      if selectedType == 'use' then
        onUseWith(clickedWidget, mousePosition)
      elseif selectedType == 'trade' then
        onTradeWith(clickedWidget, mousePosition)
      end
    end
  end

  selectedThing = nil
  g_mouse.restoreCursor()
  self:ungrabMouse()
  return true
end

function onUseWith(clickedWidget, mousePosition)
  if clickedWidget:getClassName() == 'UIMap' then
    local tile = clickedWidget:getTile(mousePosition)
    if tile then
      g_game.useWith(selectedThing, tile:getTopMultiUseThing())
    end
  elseif clickedWidget:getClassName() == 'UIItem' and not clickedWidget:isVirtual() then
    g_game.useWith(selectedThing, clickedWidget:getItem())
  end
end

function onTradeWith(clickedWidget, mousePosition)
  if clickedWidget:getClassName() == 'UIMap' then
    local tile = clickedWidget:getTile(mousePosition)
    if tile then
      g_game.requestTrade(selectedThing, tile:getTopCreature())
    end
  end
end

function startUseWith(thing)
  selectedType = 'use'
  selectedThing = thing
  mouseGrabberWidget:grabMouse()
  g_mouse.setTargetCursor()
end

function startTradeWith(thing)
  selectedType = 'trade'
  selectedThing = thing
  mouseGrabberWidget:grabMouse()
  g_mouse.setTargetCursor()
end

function createThingMenu(menuPosition, lookThing, useThing, creatureThing)
  if not g_game.isOnline() then return end
  local menu = g_ui.createWidget('PopupMenu')

  if lookThing then
    menu:addOption(tr('Look'), function() g_game.look(lookThing) end)
  end

  if useThing then
    if useThing:isContainer() then
      if useThing:getParentContainer() then
        menu:addOption(tr('Open'), function() g_game.open(useThing, useThing:getParentContainer()) end)
        menu:addOption(tr('Open in new window'), function() g_game.open(useThing) end)
      else
        menu:addOption(tr('Open'), function() g_game.open(useThing) end)
      end
    else
      if useThing:isMultiUse() then
        menu:addOption(tr('Use with ...'), function() startUseWith(useThing) end)
      else
        menu:addOption(tr('Use'), function() g_game.use(useThing) end)
      end
    end

    if useThing:isRotateable() then
      menu:addOption(tr('Rotate'), function() g_game.rotate(useThing) end)
    end

  end

  if lookThing and not lookThing:isCreature() and not lookThing:isNotMoveable() and lookThing:isPickupable() then
    menu:addSeparator()
    menu:addOption(tr('Trade with ...'), function() startTradeWith(lookThing) end)
  end

  if lookThing then
    local parentContainer = lookThing:getParentContainer()
    if parentContainer and parentContainer:hasParent() then
      menu:addOption(tr('Move up'), function() g_game.moveToParentContainer(lookThing, lookThing:getCount()) end)
    end
  end

  if creatureThing then
    local localPlayer = g_game.getLocalPlayer()
    menu:addSeparator()

    if creatureThing:isLocalPlayer() then
      menu:addOption(tr('Set Outfit'), function() g_game.requestOutfit() end)

      if not localPlayer:isMounted() then
        menu:addOption(tr('Mount'), function() localPlayer:mount() end)
      else
        menu:addOption(tr('Dismount'), function() localPlayer:dismount() end)
      end

      if creatureThing:isPartyMember() then
        if creatureThing:isPartyLeader() then
          if creatureThing:isPartySharedExperienceActive() then
            menu:addOption(tr('Disable Shared Experience'), function() g_game.partyShareExperience(false) end)
          else
            menu:addOption(tr('Enable Shared Experience'), function() g_game.partyShareExperience(true) end)
          end
        end
        menu:addOption(tr('Leave Party'), function() g_game.partyLeave() end)
      end

    else
      if g_game.getAttackingCreature() ~= creatureThing then
        menu:addOption(tr('Attack'), function() g_game.attack(creatureThing) end)
      else
        menu:addOption(tr('Stop Attack'), function() g_game.cancelAttack() end)
      end

      if g_game.getFollowingCreature() ~= creatureThing then
        menu:addOption(tr('Follow'), function() g_game.follow(creatureThing) end)
      else
        menu:addOption(tr('Stop Follow'), function() g_game.cancelFollow() end)
      end

      if creatureThing:isPlayer() then
        menu:addSeparator()
        local creatureName = creatureThing:getName()
        menu:addOption(tr('Message to %s', creatureName), function() g_game.openPrivateChannel(creatureName) end)
        if modules.game_console.getOwnPrivateTab() then
          menu:addOption(tr('Invite to private chat'), function() g_game.inviteToOwnChannel(creatureName) end)
          menu:addOption(tr('Exclude from private chat'), function() g_game.excludeFromOwnChannel(creatureName) end) -- [TODO] must be removed after message's popup labels been implemented
        end
        if (not Player:hasVip(creatureName)) then
          menu:addOption(tr('Add to VIP list'), function() g_game.addVip(creatureName) end)
        end

        local localPlayerShield = localPlayer:getShield()
        local creatureShield = creatureThing:getShield()

        if localPlayerShield == ShieldNone or localPlayerShield == ShieldWhiteBlue then
          if creatureShield == ShieldWhiteYellow then
            menu:addOption(tr('Join %s\'s Party', creatureThing:getName()), function() g_game.partyJoin(creatureThing:getId()) end)
          else
            menu:addOption(tr('Invite to Party'), function() g_game.partyInvite(creatureThing:getId()) end)
          end
        elseif localPlayerShield == ShieldWhiteYellow then
          if creatureShield == ShieldWhiteBlue then
            menu:addOption(tr('Revoke %s\'s Invitation', creatureThing:getName()), function() g_game.partyRevokeInvitation(creatureThing:getId()) end)
          end
        elseif localPlayerShield == ShieldYellow or localPlayerShield == ShieldYellowSharedExp or localPlayerShield == ShieldYellowNoSharedExpBlink or localPlayerShield == ShieldYellowNoSharedExp then
          if creatureShield == ShieldWhiteBlue then
            menu:addOption(tr('Revoke %s\'s Invitation', creatureThing:getName()), function() g_game.partyRevokeInvitation(creatureThing:getId()) end)
          elseif creatureShield == ShieldBlue or creatureShield == ShieldBlueSharedExp or creatureShield == ShieldBlueNoSharedExpBlink or creatureShield == ShieldBlueNoSharedExp then
            menu:addOption(tr('Pass Leadership to %s', creatureThing:getName()), function() g_game.partyPassLeadership(creatureThing:getId()) end)
          else
            menu:addOption(tr('Invite to Party'), function() g_game.partyInvite(creatureThing:getId()) end)
          end
        end
      end
    end

    if modules.game_ruleviolation.hasWindowAccess() then
      menu:addSeparator()
      menu:addOption(tr('Rule Violation'), function() modules.game_ruleviolation.show(creatureThing:getName()) end)
    end

    menu:addSeparator()
    menu:addOption(tr('Copy Name'), function() g_window.setClipboardText(creatureThing:getName()) end)

  end

  menu:display(menuPosition)
end

function processMouseAction(menuPosition, mouseButton, autoWalkPos, lookThing, useThing, creatureThing, multiUseThing)
  local keyboardModifiers = g_keyboard.getModifiers()

  if not Options.getOption('classicControl') then
    if keyboardModifiers == KeyboardNoModifier and mouseButton == MouseRightButton then
      createThingMenu(menuPosition, lookThing, useThing, creatureThing)
      return true
    elseif lookThing and keyboardModifiers == KeyboardShiftModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.look(lookThing)
      return true
    elseif useThing and keyboardModifiers == KeyboardCtrlModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      if useThing:isContainer() then
        if useThing:getParentContainer() then
          g_game.open(useThing, useThing:getParentContainer())
        else
          g_game.open(useThing)
        end
        return true
      elseif useThing:isMultiUse() then
        startUseWith(useThing)
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
    if multiUseThing and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseRightButton and not g_mouse.isPressed(MouseLeftButton) then
      local player = g_game.getLocalPlayer()
      if creatureThing and creatureThing ~= player then
        g_game.attack(creatureThing)
        return true
      elseif multiUseThing:isContainer() then
        if multiUseThing:getParentContainer() then
          g_game.open(multiUseThing, multiUseThing:getParentContainer())
          return true
        else
          g_game.open(multiUseThing)
          return true
        end
      elseif multiUseThing:isMultiUse() then
        startUseWith(useThing)
        return true
      else
        g_game.use(multiUseThing)
      end
      return true
    elseif lookThing and keyboardModifiers == KeyboardShiftModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.look(lookThing)
      return true
    elseif lookThing and ((g_mouse.isPressed(MouseLeftButton) and mouseButton == MouseRightButton) or (g_mouse.isPressed(MouseRightButton) and mouseButton == MouseLeftButton)) then
       g_game.look(lookThing)
      return true
    elseif useThing and keyboardModifiers == KeyboardCtrlModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      createThingMenu(menuPosition, lookThing, useThing, creatureThing)
      return true
    elseif creatureThing and keyboardModifiers == KeyboardAltModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.attack(creatureThing)
      return true
    end
  end

  if autoWalkPos and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseLeftButton then
    local dirs = g_map.findPath(g_game.getLocalPlayer():getPosition(), autoWalkPos, 127, PathFindFlags.AllowNullTiles)
    if #dirs == 0 then
      modules.game_textmessage.displayStatusMessage(tr('There is no way.'))
      return true
    end
    g_game.autoWalk(dirs)
    return true
  end

  return false
end

function moveStackableItem(item, toPos)
  if(countWindow) then
    return
  end
  if g_keyboard.isCtrlPressed() then
    g_game.move(item, toPos, item:getCount())
    return
  elseif g_keyboard.isShiftPressed() then
    g_game.move(item, toPos, 1)
    return
  end

  local count = item:getCount()
  countWindow = g_ui.createWidget('CountWindow', rootWidget)
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
    countWindow = nil
  end
  local cancelButton = countWindow:getChildById('buttonCancel')
  local cancelFunc = function()
    cancelButton:getParent():destroy()
    countWindow = nil
  end

  countWindow.onEnter = moveFunc
  countWindow.onEscape = cancelFunc

  okButton.onClick = moveFunc
  cancelButton.onClick = cancelFunc
end

function getRootPanel()
  return gameRootPanel
end

function getMapPanel()
  return gameMapPanel
end

function getRightPanel()
  return gameRightPanel
end

function getLeftPanel()
  return gameLeftPanel
end

function getBottomPanel()
  return gameBottomPanel
end

function onLeftPanelVisibilityChange(leftPanel, visible)
  if not visible then
    local children = leftPanel:getChildren()
    for i=1,#children do
      children[i]:setParent(gameRightPanel)
    end
  end
end

