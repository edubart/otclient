combatControlsButton = nil
combatControlsWindow = nil
fightOffensiveBox = nil
fightBalancedBox = nil
fightDefensiveBox = nil
chaseModeButton = nil
safeFightButton = nil
whiteDoveBox = nil
whiteHandBox = nil
yellowHandBox = nil
redFistBox = nil
mountButton = nil
pvpModesPanel = nil
fightModeRadioGroup = nil
pvpModeRadioGroup = nil

function init()
  combatControlsButton = modules.client_topmenu.addRightGameToggleButton('combatControlsButton', 
    tr('Combat Controls'), '/images/topbuttons/combatcontrols', toggle)
  combatControlsButton:setOn(true)
  combatControlsWindow = g_ui.loadUI('combatcontrols', modules.game_interface.getRightPanel())
  combatControlsWindow:disableResize()

  fightOffensiveBox = combatControlsWindow:recursiveGetChildById('fightOffensiveBox')
  fightBalancedBox = combatControlsWindow:recursiveGetChildById('fightBalancedBox')
  fightDefensiveBox = combatControlsWindow:recursiveGetChildById('fightDefensiveBox')

  chaseModeButton = combatControlsWindow:recursiveGetChildById('chaseModeBox')
  safeFightButton = combatControlsWindow:recursiveGetChildById('safeFightBox')

  mountButton = combatControlsWindow:recursiveGetChildById('mountButton')
  mountButton.onClick = onMountButtonClick

  pvpModesPanel = combatControlsWindow:recursiveGetChildById('pvpModesPanel')

  whiteDoveBox = combatControlsWindow:recursiveGetChildById('whiteDoveBox')
  whiteHandBox = combatControlsWindow:recursiveGetChildById('whiteHandBox')
  yellowHandBox = combatControlsWindow:recursiveGetChildById('yellowHandBox')
  redFistBox = combatControlsWindow:recursiveGetChildById('redFistBox')

  fightModeRadioGroup = UIRadioGroup.create()
  fightModeRadioGroup:addWidget(fightOffensiveBox)
  fightModeRadioGroup:addWidget(fightBalancedBox)
  fightModeRadioGroup:addWidget(fightDefensiveBox)

  pvpModeRadioGroup = UIRadioGroup.create()
  pvpModeRadioGroup:addWidget(whiteDoveBox)
  pvpModeRadioGroup:addWidget(whiteHandBox)
  pvpModeRadioGroup:addWidget(yellowHandBox)
  pvpModeRadioGroup:addWidget(redFistBox)

  connect(fightModeRadioGroup, { onSelectionChange = onSetFightMode })
  connect(pvpModeRadioGroup, { onSelectionChange = onSetPVPMode })
  connect(chaseModeButton, { onCheckChange = onSetChaseMode })
  connect(safeFightButton, { onCheckChange = onSetSafeFight })
  connect(g_game, {
    onGameStart = online,
    onGameEnd = offline,
    onFightModeChange = update,
    onChaseModeChange = update,
    onSafeFightChange = update,
    onPVPModeChange   = update,
    onWalk = check,
    onAutoWalk = check
  })

  connect(LocalPlayer, { onOutfitChange = onOutfitChange })

  if g_game.isOnline() then
    online()
  end

  combatControlsWindow:setup()
end

function terminate()
  if g_game.isOnline() then
    offline()
  end

  fightModeRadioGroup:destroy()
  pvpModeRadioGroup:destroy()
  combatControlsWindow:destroy()
  combatControlsButton:destroy()

  disconnect(g_game, {
    onGameStart = online,
    onGameEnd = offline,
    onFightModeChange = update,
    onChaseModeChange = update,
    onSafeFightChange = update,
    onPVPModeChange   = update,
    onWalk = check,
    onAutoWalk = check
  })

  disconnect(LocalPlayer, { onOutfitChange = onOutfitChange })
end

function update()
  local fightMode = g_game.getFightMode()
  if fightMode == FightOffensive then
    fightModeRadioGroup:selectWidget(fightOffensiveBox)
  elseif fightMode == FightBalanced then
    fightModeRadioGroup:selectWidget(fightBalancedBox)
  else
    fightModeRadioGroup:selectWidget(fightDefensiveBox)
  end

  local chaseMode = g_game.getChaseMode()
  chaseModeButton:setChecked(chaseMode == ChaseOpponent)

  local safeFight = g_game.isSafeFight()
  safeFightButton:setChecked(not safeFight)

  if g_game.getFeature(GamePVPMode) then
    local pvpMode = g_game.getPVPMode()
    local pvpWidget = getPVPBoxByMode(pvpMode)
    if pvpWidget then
      pvpModeRadioGroup:selectWidget(pvpWidget)
    end
  end
end

function check()
  if modules.client_options.getOption('autoChaseOverride') then
    if g_game.isAttacking() and g_game.getChaseMode() == ChaseOpponent then
      g_game.setChaseMode(DontChase)
    end
  end
end

function online()
  local player = g_game.getLocalPlayer()
  if player then
    local char = g_game.getCharacterName()

    local lastCombatControls = g_settings.getNode('LastCombatControls')

    if not table.empty(lastCombatControls) then
      if lastCombatControls[char] then
        g_game.setFightMode(lastCombatControls[char].fightMode)
        g_game.setChaseMode(lastCombatControls[char].chaseMode)
        g_game.setSafeFight(lastCombatControls[char].safeFight)
        if lastCombatControls[char].pvpMode then
          g_game.setPVPMode(lastCombatControls[char].pvpMode)
        end
      end
    end

    if g_game.getFeature(GamePlayerMounts) then
      mountButton:setVisible(true)
      mountButton:setChecked(player:isMounted())
    else
      mountButton:setVisible(false)
    end

    if g_game.getFeature(GamePVPMode) then
      pvpModesPanel:setVisible(true)
      combatControlsWindow:setHeight(combatControlsWindow.extendedControlsHeight)
    else
      pvpModesPanel:setVisible(false)
      combatControlsWindow:setHeight(combatControlsWindow.simpleControlsHeight)
    end
  end

  update()
end

function offline()
  local lastCombatControls = g_settings.getNode('LastCombatControls')
  if not lastCombatControls then
    lastCombatControls = {}
  end

  local player = g_game.getLocalPlayer()
  if player then
    local char = g_game.getCharacterName()
    lastCombatControls[char] = {
      fightMode = g_game.getFightMode(),
      chaseMode = g_game.getChaseMode(),
      safeFight = g_game.isSafeFight()
    }

    if g_game.getFeature(GamePVPMode) then
      lastCombatControls[char].pvpMode = g_game.getPVPMode()
    end

    -- save last combat control settings
    g_settings.setNode('LastCombatControls', lastCombatControls)
  end
end

function toggle()
  if combatControlsButton:isOn() then
    combatControlsWindow:close()
    combatControlsButton:setOn(false)
  else
    combatControlsWindow:open()
    combatControlsButton:setOn(true)
  end
end

function onSetFightMode(self, selectedFightButton)
  if selectedFightButton == nil then return end
  local buttonId = selectedFightButton:getId()
  local fightMode
  if buttonId == 'fightOffensiveBox' then
    fightMode = FightOffensive
  elseif buttonId == 'fightBalancedBox' then
    fightMode = FightBalanced
  else
    fightMode = FightDefensive
  end
  g_game.setFightMode(fightMode)
end

function onSetChaseMode(self, checked)
  local chaseMode
  if checked then
    chaseMode = ChaseOpponent
  else
    chaseMode = DontChase
  end
  g_game.setChaseMode(chaseMode)
end

function onSetSafeFight(self, checked)
  g_game.setSafeFight(not checked)
end

function onSetPVPMode(self, selectedPVPButton)
  if selectedPVPButton == nil then
    return
  end

  local buttonId = selectedPVPButton:getId()
  local pvpMode = PVPWhiteDove
  if buttonId == 'whiteDoveBox' then
    pvpMode = PVPWhiteDove
  elseif buttonId == 'whiteHandBox' then
    pvpMode = PVPWhiteHand
  elseif buttonId == 'yellowHandBox' then
    pvpMode = PVPYellowHand
  elseif buttonId == 'redFistBox' then
    pvpMode = PVPRedFist
  end

  g_game.setPVPMode(pvpMode)
end

function onMiniWindowClose()
  combatControlsButton:setOn(false)
end

function onMountButtonClick(self, mousePos)
  local player = g_game.getLocalPlayer()
  if player then
    player:toggleMount()
  end
end

function onOutfitChange(localPlayer, outfit, oldOutfit)
  if outfit.mount == oldOutfit.mount then
    return
  end

  mountButton:setChecked(outfit.mount ~= nil and outfit.mount > 0)
end

function getPVPBoxByMode(mode)
  local widget = nil
  if mode == PVPWhiteDove then
    widget = whiteDoveBox
  elseif mode == PVPWhiteHand then
    widget = whiteHandBox
  elseif mode == PVPYellowHand then
    widget = yellowHandBox
  elseif mode == PVPRedFist then
    widget = redFistBox
  end
  return widget
end
