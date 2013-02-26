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
limitZoom = false
currentViewMode = 0
smartWalkDirs = {}
smartWalkDir = nil
walkFunction = nil

function init()
  g_ui.importStyle('styles/countwindow')

  connect(g_game, {
    onGameStart = onGameStart,
    onGameEnd = onGameEnd,
    onLoginAdvice = onLoginAdvice,
  }, true)

  gameRootPanel = g_ui.displayUI('gameinterface')
  gameRootPanel:hide()
  gameRootPanel:lower()
  gameRootPanel.onGeometryChange = updateStretchShrink
  gameRootPanel.onFocusChange = stopSmartWalk

  mouseGrabberWidget = gameRootPanel:getChildById('mouseGrabber')
  mouseGrabberWidget.onMouseRelease = onMouseGrabberRelease

  bottomSplitter = gameRootPanel:getChildById('bottomSplitter')
  gameMapPanel = gameRootPanel:getChildById('gameMapPanel')
  gameRightPanel = gameRootPanel:getChildById('gameRightPanel')
  gameLeftPanel = gameRootPanel:getChildById('gameLeftPanel')
  gameBottomPanel = gameRootPanel:getChildById('gameBottomPanel')
  connect(gameLeftPanel, { onVisibilityChange = onLeftPanelVisibilityChange })

  logoutButton = modules.client_topmenu.addLeftButton('logoutButton', tr('Exit'), '/images/topbuttons/logout', tryLogout, true)

  setupViewMode(0)

  bindKeys()
  load()

  if g_game.isOnline() then
    show()
  end
end

function bindKeys()
  gameRootPanel:setAutoRepeatDelay(250)
  g_keyboard.bindKeyDown('Up', function() changeWalkDir(North) end, gameRootPanel, true)
  g_keyboard.bindKeyDown('Right', function() changeWalkDir(East) end, gameRootPanel, true)
  g_keyboard.bindKeyDown('Down', function() changeWalkDir(South) end, gameRootPanel, true)
  g_keyboard.bindKeyDown('Left', function() changeWalkDir(West) end, gameRootPanel, true)
  g_keyboard.bindKeyDown('Numpad8', function() changeWalkDir(North) end, gameRootPanel, true)
  g_keyboard.bindKeyDown('Numpad9', function() changeWalkDir(NorthEast) end, gameRootPanel, true)
  g_keyboard.bindKeyDown('Numpad6', function() changeWalkDir(East) end, gameRootPanel, true)
  g_keyboard.bindKeyDown('Numpad3', function() changeWalkDir(SouthEast) end, gameRootPanel, true)
  g_keyboard.bindKeyDown('Numpad2', function() changeWalkDir(South) end, gameRootPanel, true)
  g_keyboard.bindKeyDown('Numpad1', function() changeWalkDir(SouthWest) end, gameRootPanel, true)
  g_keyboard.bindKeyDown('Numpad4', function() changeWalkDir(West) end, gameRootPanel, true)
  g_keyboard.bindKeyDown('Numpad7', function() changeWalkDir(NorthWest) end, gameRootPanel, true)
  g_keyboard.bindKeyUp('Up', function() changeWalkDir(North, true) end, gameRootPanel, true)
  g_keyboard.bindKeyUp('Right', function() changeWalkDir(East, true) end, gameRootPanel, true)
  g_keyboard.bindKeyUp('Down', function() changeWalkDir(South, true) end, gameRootPanel, true)
  g_keyboard.bindKeyUp('Left', function() changeWalkDir(West, true) end, gameRootPanel, true)
  g_keyboard.bindKeyUp('Numpad8', function() changeWalkDir(North, true) end, gameRootPanel, true)
  g_keyboard.bindKeyUp('Numpad9', function() changeWalkDir(NorthEast, true) end, gameRootPanel, true)
  g_keyboard.bindKeyUp('Numpad6', function() changeWalkDir(East, true) end, gameRootPanel, true)
  g_keyboard.bindKeyUp('Numpad3', function() changeWalkDir(SouthEast, true) end, gameRootPanel, true)
  g_keyboard.bindKeyUp('Numpad2', function() changeWalkDir(South, true) end, gameRootPanel, true)
  g_keyboard.bindKeyUp('Numpad1', function() changeWalkDir(SouthWest, true) end, gameRootPanel, true)
  g_keyboard.bindKeyUp('Numpad4', function() changeWalkDir(West, true) end, gameRootPanel, true)
  g_keyboard.bindKeyUp('Numpad7', function() changeWalkDir(NorthWest, true) end, gameRootPanel, true)
  g_keyboard.bindKeyPress('Up', function() smartWalk(North) end, gameRootPanel)
  g_keyboard.bindKeyPress('Right', function() smartWalk(East) end, gameRootPanel)
  g_keyboard.bindKeyPress('Down', function() smartWalk(South) end, gameRootPanel)
  g_keyboard.bindKeyPress('Left', function() smartWalk(West) end, gameRootPanel)
  g_keyboard.bindKeyPress('Numpad8', function() smartWalk(North) end, gameRootPanel)
  g_keyboard.bindKeyPress('Numpad9', function() smartWalk(NorthEast) end, gameRootPanel)
  g_keyboard.bindKeyPress('Numpad6', function() smartWalk(East) end, gameRootPanel)
  g_keyboard.bindKeyPress('Numpad3', function() smartWalk(SouthEast) end, gameRootPanel)
  g_keyboard.bindKeyPress('Numpad2', function() smartWalk(South) end, gameRootPanel)
  g_keyboard.bindKeyPress('Numpad1', function() smartWalk(SouthWest) end, gameRootPanel)
  g_keyboard.bindKeyPress('Numpad4', function() smartWalk(West) end, gameRootPanel)
  g_keyboard.bindKeyPress('Numpad7', function() smartWalk(NorthWest) end, gameRootPanel)

  g_keyboard.bindKeyPress('Ctrl+Up', function() g_game.turn(North) changeWalkDir(North) end, gameRootPanel)
  g_keyboard.bindKeyPress('Ctrl+Right', function() g_game.turn(East) changeWalkDir(East) end, gameRootPanel)
  g_keyboard.bindKeyPress('Ctrl+Down', function() g_game.turn(South) changeWalkDir(South) end, gameRootPanel)
  g_keyboard.bindKeyPress('Ctrl+Left', function() g_game.turn(West) changeWalkDir(West) end, gameRootPanel)
  g_keyboard.bindKeyPress('Ctrl+Numpad8', function() g_game.turn(North) changeWalkDir(North) end, gameRootPanel)
  g_keyboard.bindKeyPress('Ctrl+Numpad6', function() g_game.turn(East) changeWalkDir(East) end, gameRootPanel)
  g_keyboard.bindKeyPress('Ctrl+Numpad2', function() g_game.turn(South) changeWalkDir(South) end, gameRootPanel)
  g_keyboard.bindKeyPress('Ctrl+Numpad4', function() g_game.turn(West) changeWalkDir(West) end, gameRootPanel)
  g_keyboard.bindKeyPress('Escape', function() g_game.cancelAttackAndFollow() end, gameRootPanel)
  g_keyboard.bindKeyPress('Ctrl+=', function() gameMapPanel:zoomIn() end, gameRootPanel)
  g_keyboard.bindKeyPress('Ctrl+-', function() gameMapPanel:zoomOut() end, gameRootPanel)
  g_keyboard.bindKeyDown('Ctrl+Q', logout, gameRootPanel)
  g_keyboard.bindKeyDown('Ctrl+L', logout, gameRootPanel)
  g_keyboard.bindKeyDown('Ctrl+W', function() g_map.cleanTexts() modules.game_textmessage.clearMessages() end, gameRootPanel)
  g_keyboard.bindKeyDown('Ctrl+.', nextViewMode, gameRootPanel)
end

function terminate()
  save()
  hide()

  stopSmartWalk()

  disconnect(g_game, {
    onGameStart = onGameStart,
    onGameEnd = onGameEnd,
    onLoginAdvice = onLoginAdvice
  })

  disconnect(gameLeftPanel, { onVisibilityChange = onLeftPanelVisibilityChange })

  logoutButton:destroy()
  gameRootPanel:destroy()
end

function onGameStart()
  show()

  -- open tibia has delay in auto walking
  if not g_game.isOfficialTibia() then
    g_game.enableFeature(GameForceFirstAutoWalkStep)
  else
    g_game.disableFeature(GameForceFirstAutoWalkStep)
  end

  addEvent(function()
    if not limitZoom or g_game.isGM() then
      gameMapPanel:setMaxZoomOut(513)
      gameMapPanel:setLimitVisibleRange(false)
    else
      gameMapPanel:setMaxZoomOut(11)
      gameMapPanel:setLimitVisibleRange(true)
    end
  end)
end

function onGameEnd()
  setupViewMode(0)
  hide()
end

function show()
  connect(g_app, { onClose = tryExit })
  modules.client_background.hide()
  gameRootPanel:show()
  gameRootPanel:focus()
  gameMapPanel:followCreature(g_game.getLocalPlayer())
  setupViewMode(0)
  updateStretchShrink()
  logoutButton:setTooltip(tr('Logout'))
end

function hide()
  disconnect(g_app, { onClose = tryExit })
  logoutButton:setTooltip(tr('Exit'))
  
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
  modules.client_background.show()
end

function save()
  local settings = {}
  settings.splitterMarginBottom = bottomSplitter:getMarginBottom()
  g_settings.setNode('game_interface', settings)
end

function load()
  local settings = g_settings.getNode('game_interface')
  if settings then
    if settings.splitterMarginBottom then
      bottomSplitter:setMarginBottom(settings.splitterMarginBottom)
    end
  end
end

function onLoginAdvice(message)
  displayInfoBox(tr("For Your Information"), message)
end

function forceExit()
  g_game.cancelLogin()
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

  exitWindow = displayGeneralBox(tr('Exit'), tr("If you shut down the program, your character might stay in the game.\nClick on 'Logout' to ensure that you character leaves the game properly.\nClick on 'Exit' if you want to exit the program without logging out your character."),
  { { text=tr('Force Exit'), callback=exitFunc },
    { text=tr('Logout'), callback=logoutFunc },
    { text=tr('Cancel'), callback=cancelFunc },
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
  if not g_game.isOnline() then
    exit()
  end

  if logoutWindow then
    return
  end

  local yesCallback = function() logout() logoutWindow:destroy() logoutWindow=nil end
  local noCallback = function() logoutWindow:destroy() logoutWindow=nil end

  logoutWindow = displayGeneralBox(tr('Logout'), tr('Are you sure you want to logout?'), {
    { text=tr('Yes'), callback=yesCallback },
    { text=tr('No'), callback=noCallback },
    anchor=AnchorHorizontalCenter}, yesCallback, noCallback)
end

function stopSmartWalk()
  smartWalkDirs = {}
  smartWalkDir = nil
end

function changeWalkDir(dir, pop)
  while table.removevalue(smartWalkDirs, dir) do end
  if pop then
    if #smartWalkDirs == 0 then
      stopSmartWalk()
      return
    end
  else
    table.insert(smartWalkDirs, 1, dir)
  end

  smartWalkDir = smartWalkDirs[1]
  if modules.client_options.getOption('smartWalk') and #smartWalkDirs > 1 then
    for _,d in pairs(smartWalkDirs) do
      if (smartWalkDir == North and d == West) or (smartWalkDir == West and d == North) then
        smartWalkDir = NorthWest
        break
      elseif (smartWalkDir == North and d == East) or (smartWalkDir == East and d == North) then
        smartWalkDir = NorthEast
        break
      elseif (smartWalkDir == South and d == West) or (smartWalkDir == West and d == South) then
        smartWalkDir = SouthWest
        break
      elseif (smartWalkDir == South and d == East) or (smartWalkDir == East and d == South) then
        smartWalkDir = SouthEast
        break
      end
    end
  end
end

function smartWalk(dir)
  if g_keyboard.getModifiers() == KeyboardNoModifier then
    local func = walkFunction
    if not func then
      if modules.client_options.getOption('dashWalk') then
        func = g_game.dashWalk
      else
        func = g_game.walk
      end
    end
    local dire = smartWalkDir or dir
    func(dire)
    return true
  end
  return false
end

function updateStretchShrink()
  if modules.client_options.getOption('dontStretchShrink') and not alternativeView then
    gameMapPanel:setVisibleDimension({ width = 15, height = 11 })

    -- Set gameMapPanel size to height = 11 * 32 + 2
    bottomSplitter:setMarginBottom(bottomSplitter:getMarginBottom() + (gameMapPanel:getHeight() - 32 * 11) - 10)
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
  g_mouse.popCursor('target')
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
  if not thing then return end
  if g_ui.isMouseGrabbed() then
    if selectedThing then
      selectedThing = thing
      selectedType = 'use'
    end
    return
  end
  selectedType = 'use'
  selectedThing = thing
  mouseGrabberWidget:grabMouse()
  g_mouse.pushCursor('target')
end

function startTradeWith(thing)
  if not thing then return end
  if g_ui.isMouseGrabbed() then
    if selectedThing then
      selectedThing = thing
      selectedType = 'trade'
    end
    return
  end
  selectedType = 'trade'
  selectedThing = thing
  mouseGrabberWidget:grabMouse()
  g_mouse.pushCursor('target')
end

function createThingMenu(menuPosition, lookThing, useThing, creatureThing)
  if not g_game.isOnline() then return end
  local menu = g_ui.createWidget('PopupMenu')
  local classic = modules.client_options.getOption('classicControl')
  local shortcut = nil

  if not classic then shortcut = '(Shift)' else shortcut = nil end
  if lookThing then
    menu:addOption(tr('Look'), function() g_game.look(lookThing) end, shortcut)
  end

  if not classic then shortcut = '(Ctrl)' else shortcut = nil end
  if useThing then
    if useThing:isContainer() then
      if useThing:getParentContainer() then
        menu:addOption(tr('Open'), function() g_game.open(useThing, useThing:getParentContainer()) end, shortcut)
        menu:addOption(tr('Open in new window'), function() g_game.open(useThing) end)
      else
        menu:addOption(tr('Open'), function() g_game.open(useThing) end, shortcut)
      end
    else
      if useThing:isMultiUse() then
        menu:addOption(tr('Use with ...'), function() startUseWith(useThing) end, shortcut)
      else
        menu:addOption(tr('Use'), function() g_game.use(useThing) end, shortcut)
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
      if not classic then shortcut = '(Alt)' else shortcut = nil end
      if g_game.getAttackingCreature() ~= creatureThing then
        menu:addOption(tr('Attack'), function() g_game.attack(creatureThing) end, shortcut)
      else
        menu:addOption(tr('Stop Attack'), function() g_game.cancelAttack() end, shortcut)
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
        if not localPlayer:hasVip(creatureName) then
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

  if not modules.client_options.getOption('classicControl') then
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
    elseif creatureThing and g_keyboard.isAltPressed() and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
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
        return true
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
    elseif creatureThing and g_keyboard.isAltPressed() and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.attack(creatureThing)
      return true
    end
  end

  local player = g_game.getLocalPlayer()
  player:stopAutoWalk()

  if autoWalkPos and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseLeftButton then
    player:autoWalk(autoWalkPos)
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
  spinbox.firstEdit = true

  local spinBoxValueChange = function(self, value)
    spinbox.firstEdit = false
    scrollbar:setValue(value)
  end
  spinbox.onValueChange = spinBoxValueChange

  local check = function()
    if spinbox.firstEdit then
      spinbox:setValue(spinbox:getMaximum())
      spinbox.firstEdit = false
    end
  end
  g_keyboard.bindKeyPress("Up", function() check() spinbox:up() end, spinbox)
  g_keyboard.bindKeyPress("Down", function() check() spinbox:down() end, spinbox)
  g_keyboard.bindKeyPress("PageUp", function() check() spinbox:setValue(spinbox:getValue()+10) end, spinbox)
  g_keyboard.bindKeyPress("PageDown", function() check() spinbox:setValue(spinbox:getValue()-10) end, spinbox)

  scrollbar.onValueChange = function(self, value)
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
  if not visible and g_game.isOnline() then
    local children = leftPanel:getChildren()
    for i=1,#children do
      children[i]:setParent(gameRightPanel)
    end
  end
end

function nextViewMode()
  setupViewMode((currentViewMode + 1) % 3)
end

function setupViewMode(mode)
  if mode == currentViewMode then return end

  if currentViewMode == 2 then
    gameMapPanel:addAnchor(AnchorLeft, 'gameLeftPanel', AnchorRight)
    gameMapPanel:addAnchor(AnchorRight, 'gameRightPanel', AnchorLeft)
    gameMapPanel:addAnchor(AnchorBottom, 'gameBottomPanel', AnchorTop)
    gameRootPanel:addAnchor(AnchorTop, 'topMenu', AnchorBottom)
    gameLeftPanel:setOn(modules.client_options.getOption('showLeftPanel'))
    gameLeftPanel:setImageColor('white')
    gameRightPanel:setImageColor('white')
    gameLeftPanel:setMarginTop(0)
    gameRightPanel:setMarginTop(0)
    gameBottomPanel:setImageColor('white')
    modules.client_topmenu.getTopMenu():setImageColor('white')
    g_game.changeMapAwareRange(18, 14)
  end

  if mode == 0 then
    gameMapPanel:setKeepAspectRatio(true)
    gameMapPanel:setLimitVisibleRange(false)
    gameMapPanel:setZoom(11)
    gameMapPanel:setVisibleDimension({ width = 15, height = 11 })
  elseif mode == 1 then
    gameMapPanel:setKeepAspectRatio(false)
    gameMapPanel:setLimitVisibleRange(true)
    gameMapPanel:setZoom(11)
    gameMapPanel:setVisibleDimension({ width = 15, height = 11 })
  elseif mode == 2 then
    local limit = limitZoom and not g_game.isGM()
    gameMapPanel:setLimitVisibleRange(limit)
    gameMapPanel:setZoom(11)
    gameMapPanel:setVisibleDimension({ width = 15, height = 11 })
    gameMapPanel:fill('parent')
    gameRootPanel:fill('parent')
    gameLeftPanel:setImageColor('alpha')
    gameRightPanel:setImageColor('alpha')
    gameLeftPanel:setMarginTop(modules.client_topmenu.getTopMenu():getHeight() - gameLeftPanel:getPaddingTop())
    gameRightPanel:setMarginTop(modules.client_topmenu.getTopMenu():getHeight() - gameRightPanel:getPaddingTop())
    gameLeftPanel:setOn(true)
    gameLeftPanel:setVisible(true)
    gameRightPanel:setOn(true)
    gameMapPanel:setOn(true)
    gameBottomPanel:setImageColor('#ffffff88')
    modules.client_topmenu.getTopMenu():setImageColor('#ffffff66')
    if not limit then
      g_game.changeMapAwareRange(24, 20)
    end
  end

  currentViewMode = mode
end

function limitZoom()
  limitZoom = true
end
