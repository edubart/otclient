GameInterface = {}

local WALK_AUTO_REPEAT_DELAY = 90
local gameRootPanel
local gameMapPanel
local gameRightPanel
local gameLeftPanel
local gameBottomPanel
local logoutButton
local mouseGrabberWidget

local countWindow
local logoutWindow
local exitWindow

local function onLeftPanelVisibilityChange(leftPanel, visible)
  if not visible then
    local children = leftPanel:getChildren()
    for i=1,#children do
      children[i]:setParent(gameRightPanel)
    end
  end
end

function GameInterface.init()
  g_ui.importStyle('styles/countwindow.otui')
  g_ui.importStyle('styles/logoutwindow.otui')
  g_ui.importStyle('styles/exitwindow.otui')

  connect(g_game, { onGameStart = GameInterface.show,
                    onGameEnd = GameInterface.hide }, true)

  gameRootPanel = g_ui.displayUI('gameinterface.otui')
  gameRootPanel:hide()
  gameRootPanel:lower()

  mouseGrabberWidget = gameRootPanel:getChildById('mouseGrabber')
  mouseGrabberWidget.onMouseRelease = GameInterface.onMouseGrabberRelease

  gameMapPanel = gameRootPanel:getChildById('gameMapPanel')
  gameRightPanel = gameRootPanel:getChildById('gameRightPanel')
  gameLeftPanel = gameRootPanel:getChildById('gameLeftPanel')
  gameBottomPanel = gameRootPanel:getChildById('gameBottomPanel')
  connect(gameLeftPanel, { onVisibilityChange = onLeftPanelVisibilityChange })

  logoutButton = TopMenu.addRightButton('logoutButton', 'Logout', '/images/logout.png', GameInterface.tryLogout)
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
  g_keyboard.bindKeyDown('Ctrl+Q', GameInterface.logout, gameRootPanel)
  g_keyboard.bindKeyDown('Ctrl+L', GameInterface.logout, gameRootPanel)
  g_keyboard.bindKeyDown('Ctrl+W', function() g_map.cleanTexts() TextMessage.clearMessages() end, gameRootPanel)

  g_keyboard.bindKeyDown('Ctrl+.', function()
    if gameMapPanel:isKeepAspectRatioEnabled() then
      gameMapPanel:setKeepAspectRatio(false)
    else
      gameMapPanel:setKeepAspectRatio(true)
      gameMapPanel:setVisibleDimension({ width = 15, height = 11 })
    end
  end)

  if g_game.isOnline() then
    GameInterface.show()
  end
end

function GameInterface.terminate()
  disconnect(g_game, { onGameStart = GameInterface.show,
                       onGameEnd = GameInterface.hide })
  disconnect(gameLeftPanel, { onVisibilityChange = onLeftPanelVisibilityChange })

  logoutButton:destroy()
  logoutButton = nil
  gameRootPanel:destroy()
  gameRootPanel = nil
  gameMapPanel = nil
  gameRightPanel = nil
  gameLeftPanel = nil
  gameBottomPanel = nil
  mouseGrabberWidget = nil
  countWindow = nil
  logoutWindow = nil
  exitWindow = nil
  GameInterface = nil
end

function GameInterface.show()
  connect(g_app, { onClose = GameInterface.tryExit })
  logoutButton:show()
  Background.hide()
  gameRootPanel:show()
  gameRootPanel:focus()
  gameMapPanel:followCreature(g_game.getLocalPlayer())
end

function GameInterface.hide()
  disconnect(g_app, { onClose = GameInterface.tryExit })
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

function GameInterface.exit()
  if g_game.isOnline() then
    g_game.forceLogout()
    scheduleEvent(exit, 10)
    return true
  end
end

function GameInterface.tryExit()
  if exitWindow then
    return true
  end
  exitWindow = g_ui.createWidget('ExitWindow', rootWidget)
  local exitButton = exitWindow:getChildById('buttonExit')
  local logButton = exitWindow:getChildById('buttonLogout')
  local cancelButton = exitWindow:getChildById('buttonCancel')

  local exitFunc = function()
    GameInterface.exit()
  end
  local logoutFunc = function()
    GameInterface.logout()
    logButton:getParent():destroy()
    exitWindow = nil
  end
  local cancelFunc = function()
    cancelButton:getParent():destroy()
    exitWindow = nil
  end

  exitWindow.onEscape = cancelFunc
  exitWindow.onEnter = logoutFunc

  exitButton.onClick = exitFunc
  logButton.onClick = logoutFunc
  cancelButton.onClick = cancelFunc
  return true -- signal closing
end

function GameInterface.logout()
  if g_game.isOnline() then
    g_game.safeLogout()
    return true
  end
end

function GameInterface.tryLogout()
  if logoutWindow then
    return
  end
  logoutWindow = g_ui.createWidget('LogoutWindow', rootWidget)
  local yesButton = logoutWindow:getChildById('buttonYes')
  local noButton = logoutWindow:getChildById('buttonNo')

  local logoutFunc = function()
    GameInterface.logout()
    yesButton:getParent():destroy()
    logoutWindow = nil
  end
  local cancelFunc = function()
    noButton:getParent():destroy()
    logoutWindow = nil
  end

  logoutWindow.onEnter = logoutFunc
  logoutWindow.onEscape = cancelFunc

  yesButton.onClick = logoutFunc
  noButton.onClick = cancelFunc
end

function GameInterface.onMouseGrabberRelease(self, mousePosition, mouseButton)
  if GameInterface.selectedThing == nil then return false end
  if mouseButton == MouseLeftButton then
    local clickedWidget = gameRootPanel:recursiveGetChildByPos(mousePosition, false)
    if clickedWidget then
      if GameInterface.selectedType == 'use' then
        GameInterface.onUseWith(clickedWidget, mousePosition)
      elseif GameInterface.selectedType == 'trade' then
        GameInterface.onTradeWith(clickedWidget, mousePosition)
      end
    end
  end

  GameInterface.selectedThing = nil
  g_mouse.restoreCursor()
  self:ungrabMouse()
  return true
end

function GameInterface.onUseWith(clickedWidget, mousePosition)
  if clickedWidget:getClassName() == 'UIMap' then
    local tile = clickedWidget:getTile(mousePosition)
    if tile then
      g_game.useWith(GameInterface.selectedThing, tile:getTopMultiUseThing())
    elseif clickedWidget:getClassName() == 'UIItem' and not clickedWidget:isVirtual() then
      g_game.useWith(GameInterface.selectedThing, clickedWidget:getItem())
    end
  end
end

function GameInterface.onTradeWith(clickedWidget, mousePosition)
  if clickedWidget:getClassName() == 'UIMap' then
    local tile = clickedWidget:getTile(mousePosition)
    if tile then
      g_game.requestTrade(GameInterface.selectedThing, tile:getTopCreature())
    end
  end
end

function GameInterface.startUseWith(thing)
  GameInterface.selectedType = 'use'
  GameInterface.selectedThing = thing
  mouseGrabberWidget:grabMouse()
  g_mouse.setTargetCursor()
end

function GameInterface.startTradeWith(thing)
  GameInterface.selectedType = 'trade'
  GameInterface.selectedThing = thing
  mouseGrabberWidget:grabMouse()
  g_mouse.setTargetCursor()
end

function GameInterface.createThingMenu(menuPosition, lookThing, useThing, creatureThing)
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
        menu:addOption(tr('Use with ...'), function() GameInterface.startUseWith(useThing) end)
      else
        menu:addOption(tr('Use'), function() g_game.use(useThing) end)
      end
    end

    if useThing:isRotateable() then
      menu:addOption(tr('Rotate'), function() g_game.rotate(useThing) end)
    end

  end

  if lookThing and not lookThing:asCreature() and not lookThing:isNotMoveable() and lookThing:isPickupable() then
    menu:addSeparator()
    menu:addOption(tr('Trade with ...'), function() GameInterface.startTradeWith(lookThing) end)
  end

  if lookThing then
    local parentContainer = lookThing:getParentContainer()
    if parentContainer and parentContainer:hasParent() then
      menu:addOption(tr('Move up'), function() g_game.moveToParentContainer(lookThing, lookThing:getCount()) end)
    end
  end

  if creatureThing then
    menu:addSeparator()

    if creatureThing:asLocalPlayer() then
      menu:addOption(tr('Set Outfit'), function() g_game.requestOutfit() end)

      if creatureThing:isPartyMember() --[[and not fighting]] then
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
      local localPlayer = g_game.getLocalPlayer()
      if localPlayer then
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

        if creatureThing:asPlayer() then
          menu:addSeparator()
          local creatureName = creatureThing:getName()
          menu:addOption(tr('Message to %s', creatureName), function() g_game.openPrivateChannel(creatureName) end)
          if Console.getOwnPrivateTab() then
            menu:addOption(tr('Invite to private chat'), function() g_game.inviteToOwnChannel(creatureName) end)
            menu:addOption(tr('Exclude from private chat'), function() g_game.excludeFromOwnChannel(creatureName) end) -- [TODO] must be removed after message's popup labels been implemented
          end
          if (not Player:hasVip(creatureName)) then
            menu:addOption(tr('Add to VIP list'), function() g_game.addVip(creatureName) end)
          end

          local localPlayerShield = localPlayer:asCreature():getShield()
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
    end

    if RuleViolation.hasWindowAccess() then
      menu:addSeparator()
      menu:addOption(tr('Rule Violation'), function() RuleViolation.show(creatureThing:getName()) end)
    end

    menu:addSeparator()
    menu:addOption(tr('Copy Name'), function() g_window.setClipboardText(creatureThing:getName()) end)

  end

  menu:display(menuPosition)
end

function GameInterface.processMouseAction(menuPosition, mouseButton, autoWalkPos, lookThing, useThing, creatureThing, multiUseThing)
  local keyboardModifiers = g_keyboard.getModifiers()

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
          g_game.open(useThing)
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
    if multiUseThing and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseRightButton and not g_mouse.isPressed(MouseLeftButton) then
      local player = g_game.getLocalPlayer()
      if multiUseThing:asCreature() and multiUseThing:asCreature() ~= player then
        g_game.attack(multiUseThing:asCreature())
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
        GameInterface.startUseWith(useThing)
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
      GameInterface.createThingMenu(menuPosition, lookThing, useThing, creatureThing)
      return true
    elseif creatureThing and keyboardModifiers == KeyboardAltModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.attack(creatureThing)
      return true
    end
  end

  if autoWalkPos and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseLeftButton then
    local dirs = g_map.findPath(g_game.getLocalPlayer():getPosition(), autoWalkPos, 127)
    if #dirs == 0 then
      TextMessage.displayStatus(tr('There is no way.'))
      return true
    end
    g_game.autoWalk(dirs)
    return true
  end

  return false
end

function GameInterface.moveStackableItem(item, toPos)
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
