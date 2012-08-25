battleWindow = nil
battleButton = nil
battlePanel = nil
lastBattleButtonSwitched = nil
battleButtonsByCreaturesList = {}

mouseWidget = nil

hidePlayersButton = nil
hideNPCsButton = nil
hideMonstersButton = nil
hideSkullsButton = nil
hidePartyButton = nil

function init()
  g_ui.importStyle('battlebutton.otui')
  battleButton = TopMenu.addRightGameToggleButton('battleButton', tr('Battle') .. ' (Ctrl+B)', 'battle.png', toggle)
  battleButton:setOn(true)
  battleWindow = g_ui.loadUI('battle.otui', modules.game_interface.getRightPanel())
  g_keyboard.bindKeyDown('Ctrl+B', toggle)

  -- this disables scrollbar auto hiding
  local scrollbar = battleWindow:getChildById('miniwindowScrollBar')
  scrollbar:mergeStyle({ ['$!on'] = { }})

  battlePanel = battleWindow:recursiveGetChildById('battlePanel')

  hidePlayersButton = battleWindow:recursiveGetChildById('hidePlayers')
  hideNPCsButton = battleWindow:recursiveGetChildById('hideNPCs')
  hideMonstersButton = battleWindow:recursiveGetChildById('hideMonsters')
  hideSkullsButton = battleWindow:recursiveGetChildById('hideSkulls')
  hidePartyButton = battleWindow:recursiveGetChildById('hideParty')

  mouseWidget = g_ui.createWidget('UIButton')
  mouseWidget:setVisible(false)
  mouseWidget:setFocusable(false)

  battleWindow:setContentMinimumHeight(80)
  --battleWindow:setContentMaximumHeight(384)

  connect(Creature, {
    onSkullChange = updateCreatureSkull,
    onEmblemChange = updateCreatureEmblem,
    onHealthPercentChange = onCreatureHealthPercentChange,
    onPositionChange = onCreaturePositionChange,
    onAppear = onCreatureAppear,
    onDisappear = onCreatureDisappear
  })

  connect(g_game, {
    onAttackingCreatureChange = onAttack,
    onFollowingCreatureChange = onFollow,
    onGameEnd = removeAllCreatures
  })

  checkCreatures()
  battleWindow:setup()
end

function terminate()
  g_keyboard.unbindKeyDown('Ctrl+B')
  battleButtonsByCreaturesList = {}
  battleButton:destroy()
  battleWindow:destroy()
  mouseWidget:destroy()

  disconnect(Creature, {  
    onSkullChange = updateCreatureSkull,
    onEmblemChange = updateCreatureEmblem,
    onHealthPercentChange = onCreatureHealthPercentChange,
    onPositionChange = onCreaturePositionChange,
    onAppear = onCreatureAppear,
    onDisappear = onCreatureDisappear
  })

  disconnect(g_game, {
    onAttackingCreatureChange = onAttack,
    onFollowingCreatureChange = onFollow,
    onGameEnd = removeAllCreatures
  })
end

function toggle()
  if battleButton:isOn() then
    battleWindow:close()
    battleButton:setOn(false)
  else
    battleWindow:open()
    battleButton:setOn(true)
  end
end

function onMiniWindowClose()
  battleButton:setOn(false)
end

function checkCreatures()
  removeAllCreatures()

  local spectators = {}
  local player = g_game.getLocalPlayer()
  if g_game.isOnline() then
    creatures = g_map.getSpectators(player:getPosition(), false)
    for i, creature in ipairs(creatures) do
      if creature ~= player and doCreatureFitFilters(creature) then
        table.insert(spectators, creature)
      end
    end
  end

  for i, v in pairs(spectators) do
    addCreature(v)
  end
end

function doCreatureFitFilters(creature)
  local localPlayer = g_game.getLocalPlayer()
  if creature == localPlayer then
    return false
  end

  local pos = creature:getPosition()
  if not pos then return false end

  if pos.z ~= localPlayer:getPosition().z or not localPlayer:hasSight(pos) then
    return false
  end

  local hidePlayers = hidePlayersButton:isChecked()
  local hideNPCs = hideNPCsButton:isChecked()
  local hideMonsters = hideMonstersButton:isChecked()
  local hideSkulls = hideSkullsButton:isChecked()
  local hideParty = hidePartyButton:isChecked()

  if hidePlayers and creature:isPlayer() then
    return false
  elseif hideNPCs and creature:isNpc() then
    return false
  elseif hideMonsters and creature:isMonster() then
    return false
  elseif hideSkulls and creature:isPlayer() and creature:getSkull() == SkullNone then
    return false
  elseif hideParty and creature:getShield() > ShieldWhiteBlue then
    return false
  end

  return true
end

function onCreatureHealthPercentChange(creature, health)
  local battleButton = battleButtonsByCreaturesList[creature:getId()]
  if battleButton then
    battleButton:setLifeBarPercent(creature:getHealthPercent())
  end
end

function onCreaturePositionChange(creature, newPos, oldPos)
  if creature:isLocalPlayer() then
    checkCreatures()
  else
    local has = hasCreature(creature)
    local fit = doCreatureFitFilters(creature)
    if has and not fit then
      removeCreature(creature)
    elseif not has and fit then
      addCreature(creature)
    end
  end
end

function onCreatureAppear(creature)
  local player = g_game.getLocalPlayer()
  if creature ~= player and creature:getPosition().z == player:getPosition().z and doCreatureFitFilters(creature) then
    addCreature(creature)
  end
end

function onCreatureDisappear(creature)
  removeCreature(creature)
end

function hasCreature(creature)
  return battleButtonsByCreaturesList[creature:getId()] ~= nil
end

function addCreature(creature)
  local creatureId = creature:getId()
  local battleButton = battleButtonsByCreaturesList[creatureId]
  if not battleButton then
    battleButton = g_ui.createWidget('BattleButton', battlePanel)
    battleButton:setup(creature)

    battleButton.onHoverChange = onBattleButtonHoverChange
    battleButton.onMouseRelease = onMouseRelease

    battleButtonsByCreaturesList[creatureId] = battleButton

    if creature == g_game.getAttackingCreature() then
      onAttack(creature)
    end

    if creature == g_game.getFollowingCreature() then
      onFollow(creature)
    end
  else
    battleButton:setLifeBarPercent(creature:getHealthPercent())
  end
end

function onMouseRelease(self, mousePosition, mouseButton)
  if mouseButton == MouseRightButton then
    modules.game_interface.createThingMenu(mousePosition, nil, nil, self.creature)
    return true
  elseif mouseButton == MouseLeftButton then
    if g_keyboard.isShiftPressed() then
      g_game.look(self.creature)
    else
      if self.isTarget then
        g_game.cancelAttack()
      else
        g_game.attack(self.creature)
      end
    end
    return true
  end
end

function removeAllCreatures()
  for i, v in pairs(battleButtonsByCreaturesList) do
    removeCreature(v.creature)
  end
end

function removeCreature(creature)
  local creatureId = creature:getId()

  if battleButtonsByCreaturesList[creatureId] ~= nil then
    if lastBattleButtonSwitched == battleButtonsByCreaturesList[creatureId] then
      lastBattleButtonSwitched = nil
    end

    battleButtonsByCreaturesList[creatureId].creature:hideStaticSquare()
    battleButtonsByCreaturesList[creatureId]:destroy()
    battleButtonsByCreaturesList[creatureId] = nil
  end
end

function onBattleButtonHoverChange(widget, hovered)
  if widget.isBattleButton then
    widget.isHovered = hovered
    updateBattleButton(widget)
  end
end

function onAttack(creature)
  local battleButton = creature and battleButtonsByCreaturesList[creature:getId()] or lastBattleButtonSwitched
  if battleButton then
    battleButton.isTarget = creature and true or false
    updateBattleButton(battleButton)
  end
end

function onFollow(creature)
  local battleButton = creature and battleButtonsByCreaturesList[creature:getId()] or lastBattleButtonSwitched
  if battleButton then
    battleButton.isFollowed = creature and true or false
    updateBattleButton(battleButton)
  end
end

function updateCreatureSkull(creature, skullId)
  local battleButton = battleButtonsByCreaturesList[creature:getId()]
  if battleButton then
    battleButton:updateSkull(skullId)
  end
end

function updateCreatureEmblem(creature, emblemId)
  local battleButton = battleButtonsByCreaturesList[creature:getId()]
  if battleButton then
    battleButton:updateSkull(emblemId)
  end
end

function updateBattleButton(battleButton)
  battleButton:update()
  if battleButton.isTarget or battleButton.isFollowed then
    -- set new last battle button switched
    if lastBattleButtonSwitched and lastBattleButtonSwitched ~= battleButton then
      lastBattleButtonSwitched.isTarget = false
      lastBattleButtonSwitched.isFollowed = false
      updateBattleButton(lastBattleButtonSwitched)
    end
    lastBattleButtonSwitched = battleButton
  end
end
