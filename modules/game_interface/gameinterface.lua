WALK_AUTO_REPEAT_DELAY = 150
WALK_STEPS_RETRY = 10

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
bottomSplitter = nil

lastWalkDir = nil
arrowKeys = {
  [North] = 'Up',
  [South] = 'Down',
  [East] = 'Right',
  [West] = 'Left',
  [NorthEast] = 'Numpad9',
  [SouthEast] = 'Numpad3',
  [NorthWest] = 'Numpad7',
  [SouthWest] = 'Numpad1'
}

function init()
  g_ui.importStyle('styles/countwindow.otui')

  connect(g_game, {
    onGameStart = show,
    onGameEnd = hide,
    onLoginAdvice = onLoginAdvice
  }, true)

  gameRootPanel = g_ui.displayUI('gameinterface.otui')
  gameRootPanel:hide()
  gameRootPanel:lower()
  gameRootPanel.onGeometryChange = updateStretchShrink

  mouseGrabberWidget = gameRootPanel:getChildById('mouseGrabber')
  mouseGrabberWidget.onMouseRelease = onMouseGrabberRelease

  bottomSplitter = gameRootPanel:getChildById('bottomSplitter')
  gameMapPanel = gameRootPanel:getChildById('gameMapPanel')
  gameRightPanel = gameRootPanel:getChildById('gameRightPanel')
  gameLeftPanel = gameRootPanel:getChildById('gameLeftPanel')
  gameBottomPanel = gameRootPanel:getChildById('gameBottomPanel')
  connect(gameLeftPanel, { onVisibilityChange = onLeftPanelVisibilityChange })

  logoutButton = TopMenu.addRightButton('logoutButton', 'Logout', '/images/logout.png', tryLogout)
  logoutButton:hide()

  bindKeys()

  if g_game.isOnline() then
    show()
  end
end

function bindKeys()
  g_keyboard.bindKeyPress('Up', function() smartWalk(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Right', function() smartWalk(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Down', function() smartWalk(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Left', function() smartWalk(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)

  g_keyboard.bindKeyDown('Up', function() smartWalk(North) end, gameRootPanel)
  g_keyboard.bindKeyDown('Right', function() smartWalk(East) end, gameRootPanel)
  g_keyboard.bindKeyDown('Down', function() smartWalk(South) end, gameRootPanel)
  g_keyboard.bindKeyDown('Left', function() smartWalk(West) end, gameRootPanel)

  g_keyboard.bindKeyPress('Numpad8', function() smartWalk(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad9', function() smartWalk(NorthEast) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad6', function() smartWalk(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad3', function() smartWalk(SouthEast) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad2', function() smartWalk(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad1', function() smartWalk(SouthWest) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad4', function() smartWalk(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  g_keyboard.bindKeyPress('Numpad7', function() smartWalk(NorthWest) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
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
  g_keyboard.bindKeyDown('Ctrl+.', toggleAspectRatio, gameRootPanel)
  g_keyboard.bindKeyDown('Ctrl+N', function() gameMapPanel:setDrawTexts(not gameMapPanel:isDrawingTexts()) end, gameRootPanel)
end

function terminate()
  disconnect(g_game, { 
    onGameStart = show,
    onGameEnd = hide,
    onLoginAdvice = onLoginAdvice
  })

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
  updateStretchShrink()
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

function onLoginAdvice(message)
  displayInfoBox("For Your Information", message)
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

function smartWalk(defaultDir)
  local rebindKey = false
  local lastKey = arrowKeys[lastWalkDir]

  -- choose the new direction
  if not g_keyboard.isKeyPressed(arrowKeys[defaultDir]) then
    local changeDir = false
    for k,v in pairs(arrowKeys) do
      if g_keyboard.isKeyPressed(v) then
        defaultDir = k
        changeDir = true
        break
      end
    end
    if not changeDir then
      return
    end
  end

  -- key is still pressed from previous walk event
  if lastWalkDir and lastWalkDir ~= defaultDir and g_keyboard.isKeyPressed(lastKey) then
    if g_keyboard.isKeySetPressed(arrowKeys) then
      g_keyboard.unbindKeyPress(lastKey, gameRootPanel)
      rebindKey = true
    end
  end

  local dir = defaultDir
  if Options.getOption('smartWalk') then
    if g_keyboard.isKeyPressed('Up') and g_keyboard.isKeyPressed('Left') then
      dir = NorthWest
    elseif g_keyboard.isKeyPressed('Up') and g_keyboard.isKeyPressed('Right') then
      dir = NorthEast
    elseif g_keyboard.isKeyPressed('Down') and g_keyboard.isKeyPressed('Left') then
      dir = SouthWest
    elseif g_keyboard.isKeyPressed('Down') and g_keyboard.isKeyPressed('Right') then
      dir = SouthEast
    end
  end

  if Options.getOption('walkBooster') then
    if g_game.getLocalPlayer():canWalk(dir) then
      g_game.walk(dir)
    else
      g_game.forceWalk(dir)
    end
  else
    g_game.walk(dir)
  end

  if rebindKey then
    g_keyboard.bindKeyPress(lastKey, function() smartWalk(lastWalkDir) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  end
  lastWalkDir = dir
end

function updateStretchShrink() 
  if Options.getOption('dontStretchShrink') then
    gameMapPanel:setKeepAspectRatio(true)
    gameMapPanel:setVisibleDimension({ width = 15, height = 11 })
    
    -- Set gameMapPanel size to height = 11 * 32
    bottomSplitter:setMarginBottom(bottomSplitter:getMarginBottom() + (gameMapPanel:getHeight() - 32 * 11) - 10)
  end
end

function toggleAspectRatio()
  if gameMapPanel:isKeepAspectRatioEnabled() then
    gameMapPanel:setKeepAspectRatio(false)
  else
    gameMapPanel:setKeepAspectRatio(true)
    gameMapPanel:setVisibleDimension({ width = 15, height = 11 })
  end
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
  elseif clickedWidget:getClassName() == 'UICreatureButton' then
    local creature = clickedWidget:getCreature()
    if creature then
      g_game.useWith(selectedThing, creature)
    end
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

      if g_game.getFeature(GamePlayerMounts) then
        if not localPlayer:isMounted() then
          menu:addOption(tr('Mount'), function() localPlayer:mount() end)
        else
          menu:addOption(tr('Dismount'), function() localPlayer:dismount() end)
        end
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
		
		if modules.game_console.isIgnored(creatureName) then
		  menu:addOption(tr('Unignore') .. ' ' .. creatureName, function() modules.game_console.removeIgnoredPlayer(creatureName) end)
		else
		  menu:addOption(tr('Ignore') .. ' ' .. creatureName, function() modules.game_console.addIgnoredPlayer(creatureName) end)
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

function processMouseAction(menuPosition, mouseButton, autoWalkPos, lookThing, useThing, creatureThing)
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
      end
      return true
    elseif creatureThing and keyboardModifiers == KeyboardAltModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.attack(creatureThing)
      return true
    end

  -- classic control
  else
    if useThing and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseRightButton and not g_mouse.isPressed(MouseLeftButton) then
      local player = g_game.getLocalPlayer()
      if creatureThing and creatureThing ~= player then
        g_game.attack(creatureThing)
        return true
      elseif useThing:isContainer() then
        if useThing:getParentContainer() then
          g_game.open(useThing, useThing:getParentContainer())
          return true
        else
          g_game.open(useThing)
          return true
        end
      elseif useThing:isMultiUse() then
        startUseWith(useThing)
        return true
      else
        g_game.use(useThing)
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

  local player = g_game.getLocalPlayer()
  player:stopAutoWalkUpdate()

  if autoWalkPos and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseLeftButton then
    if not player:autoWalk(autoWalkPos) then
      modules.game_textmessage.displayStatusMessage(tr('There is no way.'))
    end
    return true
  end

  return false
end

function moveStackableItem(item, toPos)
  if countWindow then
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
  local itembox = countWindow:getChildById('item')
  local scrollbar = countWindow:getChildById('countScrollBar')
  itembox:setItemId(item:getId())
  itembox:setItemCount(count)
  scrollbar:setMaximum(count)
  scrollbar:setMinimum(1)
  scrollbar:setValue(count)

  local spinbox = countWindow:getChildById('spinBox')
  spinbox:setMaximum(count)
  spinbox:setMinimum(0)
  spinbox:setValue(0)
  spinbox:hideButtons()
  spinbox:focus()

  local spinBoxValueChange = function(self, value)
    scrollbar:setValue(value)
  end
  spinbox.onValueChange = spinBoxValueChange

  scrollbar.onValueChange = function(self, value)
    item:setCount(count - value)
    itembox:setItemCount(value)
    spinbox.onValueChange = nil
    spinbox:setValue(value)
    spinbox.onValueChange = spinBoxValueChange
  end

  local okButton = countWindow:getChildById('buttonOk')
  local moveFunc = function()
    g_game.move(item, toPos, itembox:getItemCount())
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
