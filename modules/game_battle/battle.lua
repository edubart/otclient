--TODO
--onCreatureAppears onCreatureHealthChange onCreatureDisappears
--reloadable/disconnects

battleWindow = nil
battleButton = nil
battlePanel = nil
lastBattleButtonSwitched = nil
checkCreaturesEvent = nil
battleButtonsByCreaturesList = {}

mouseWidget = nil

hidePlayersButton = nil
hideNPCsButton = nil
hideMonstersButton = nil
hideSkullsButton = nil
hidePartyButton = nil

BattleButtonColors = {
  onIdle = {notHovered = '#888888', hovered = '#FFFFFF' },
  onTargeted = {notHovered = '#FF0000', hovered = '#FF8888' },
  onFollowed = {notHovered = '#00FF00', hovered = '#88FF88' }
}

LifeBarColors = {} --Must be sorted by percentAbose
table.insert(LifeBarColors, {percentAbove = 92, color = '#00BC00' } )
table.insert(LifeBarColors, {percentAbove = 60, color = '#50A150' } )
table.insert(LifeBarColors, {percentAbove = 30, color = '#A1A100' } )
table.insert(LifeBarColors, {percentAbove = 8, color = '#3C2727' } )
table.insert(LifeBarColors, {percentAbove = 3, color = '#3C0000' } )
table.insert(LifeBarColors, {percentAbove = -1, color = '#4F0000' } )

function init()
  g_ui.importStyle('battlebutton.otui')
  battleWindow = g_ui.loadUI('battle.otui', modules.game_interface.getRightPanel())
  battleButton = TopMenu.addRightGameToggleButton('battleButton', tr('Battle') .. ' (Ctrl+B)', 'battle.png', toggle)
  battleButton:setOn(true)
  g_keyboard.bindKeyDown('Ctrl+B', toggle)

  battlePanel = battleWindow:recursiveGetChildById('battlePanel')

  hidePlayersButton = battleWindow:recursiveGetChildById('hidePlayers')
  hideNPCsButton = battleWindow:recursiveGetChildById('hideNPCs')
  hideMonstersButton = battleWindow:recursiveGetChildById('hideMonsters')
  hideSkullsButton = battleWindow:recursiveGetChildById('hideSkulls')
  hidePartyButton = battleWindow:recursiveGetChildById('hideParty')

  mouseWidget = g_ui.createWidget('UIButton')
  mouseWidget:setVisible(false)
  mouseWidget:setFocusable(false)

  connect(Creature, { onSkullChange = checkCreatureSkull,
                      onEmblemChange = checkCreatureEmblem } )

  connect(g_game, { onAttackingCreatureChange = onAttack,
                    onFollowingCreatureChange = onFollow,
                    onGameEnd = removeAllCreatures } )

  addEvent(addAllCreatures)
  checkCreaturesEvent = scheduleEvent(checkCreatures, 200)
end

function terminate()
  g_keyboard.unbindKeyDown('Ctrl+B')
  battleButtonsByCreaturesList = {}
  removeEvent(checkCreaturesEvent)
  battleButton:destroy()
  battleWindow:destroy()
  mouseWidget:destroy()

  disconnect(Creature, { onSkullChange = checkCreatureSkull,
                          onEmblemChange = checkCreatureEmblem } )

  disconnect(g_game, { onAttackingCreatureChange = onAttack } )
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

function addAllCreatures()
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
  local hidePlayers = hidePlayersButton:isChecked()
  local hideNPCs = hideNPCsButton:isChecked()
  local hideMonsters = hideMonstersButton:isChecked()
  local hideSkulls = hideSkullsButton:isChecked()
  local hideParty = hidePartyButton:isChecked()

  if hidePlayers and creature:asPlayer() then
    return false
  elseif hideNPCs and creature:asNpc() then
    return false
  elseif hideMonsters and creature:asMonster() then
    return false
  elseif hideSkulls and creature:asPlayer() and creature:getSkull() == SkullNone then
    return false
  elseif hideParty and creature:getShield() > ShieldWhiteBlue then
    return false
  end

  return true
end

function checkCreatures(forceRecheck)
  local player = g_game.getLocalPlayer()
  if g_game.isOnline() then
    local spectators = {}

    -- reloading list of spectators
    local creaturesAppeared = {}
    creatures = g_map.getSpectators(player:getPosition(), false)
    for i, creature in ipairs(creatures) do
      if creature ~= player and doCreatureFitFilters(creature) then
        -- searching for creatures that appeared on battle list
        local battleButton = battleButtonsByCreaturesList[creature:getId()]
        if battleButton == nil then
          table.insert(creaturesAppeared, creature)
        else
          setLifeBarPercent(battleButton, creature:getHealthPercent())
        end
        spectators[creature:getId()] = creature
      end
    end

    for i, v in pairs(creaturesAppeared) do
      addCreature(v)
    end

    -- searching for creatures that disappeared from battle list
    local creaturesDisappeared = {}
    for i, creature in pairs(battleButtonsByCreaturesList) do
      if spectators[creature.creatureId] == nil then
        table.insert(creaturesDisappeared, creature.creature)
      end
    end

    for i, v in pairs(creaturesDisappeared) do
      removeCreature(v)
    end
  end
  if not forceRecheck then
    checkCreaturesEvent = scheduleEvent(checkCreatures, 500)
  end
end

function addCreature(creature)
  local creatureId = creature:getId()

  if battleButtonsByCreaturesList[creatureId] == nil then
    local battleButton = g_ui.createWidget('BattleButton', battlePanel)
    local creatureWidget = battleButton:getChildById('creature')
    local labelWidget = battleButton:getChildById('label')
    local lifeBarWidget = battleButton:getChildById('lifeBar')
    battleButton.onHoverChange = onbattleButtonHoverChange
    battleButton.onMouseRelease = onMouseRelease

    battleButton:setId('BattleButton_' .. creature:getName():gsub('%s','_'))
    battleButton.creatureId = creatureId
    battleButton.creature = creature
    battleButton.isHovered = false
    battleButton.isTarget = false
    battleButton.isFollowed = false

    labelWidget:setText(creature:getName())
    creatureWidget:setCreature(creature)
    setLifeBarPercent(battleButton, creature:getHealthPercent())

    battleButtonsByCreaturesList[creatureId] = battleButton

    checkCreatureSkull(battleButton.creature)
    checkCreatureEmblem(battleButton.creature)
  end
end

function checkCreatureSkull(creature, skullId)
    local battleButton = battleButtonsByCreaturesList[creature:getId()]
    if battleButton then
      local skullWidget = battleButton:getChildById('skull')
      local labelWidget = battleButton:getChildById('label')
      local creature = battleButton.creature

      if creature:getSkull() ~= SkullNone then
        skullWidget:setWidth(skullWidget:getHeight())
        local imagePath = getSkullImagePath(creature:getSkull())
        skullWidget:setImageSource(imagePath)
        labelWidget:setMarginLeft(5)
      else
        skullWidget:setWidth(0)
        if creature:getEmblem() == EmblemNone then
          labelWidget:setMarginLeft(2)
        end
      end
    end
end

function checkCreatureEmblem(creature, emblemId)
    local battleButton = battleButtonsByCreaturesList[creature:getId()]
    if battleButton then
      local emblemId = emblemId or creature:getEmblem()
      local emblemWidget = battleButton:getChildById('emblem')
      local labelWidget = battleButton:getChildById('label')
      local creature = battleButton.creature

      if emblemId ~= EmblemNone then
        emblemWidget:setWidth(emblemWidget:getHeight())
        local imagePath = getEmblemImagePath(emblemId)
        emblemWidget:setImageSource(imagePath)
        emblemWidget:setMarginLeft(5)
        labelWidget:setMarginLeft(5)
      else
        emblemWidget:setWidth(0)
        emblemWidget:setMarginLeft(0)
        if creature:getSkull() == SkullNone then
          labelWidget:setMarginLeft(2)
        end
      end
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

function setLifeBarPercent(battleButton, percent)
  local lifeBarWidget = battleButton:getChildById('lifeBar')
  lifeBarWidget:setPercent(percent)

  local color
  for i, v in pairs(LifeBarColors) do
    if percent > v.percentAbove then
      color = v.color
      break
    end
  end

  lifeBarWidget:setBackgroundColor(color)
end

function onbattleButtonHoverChange(widget, hovered)
  if widget.isBattleButton then
    widget.isHovered = hovered
    checkBattleButton(widget)
  end
end

function onAttack(creature)
  checkCreatures(true) --Force recheck
  local battleButton = creature and battleButtonsByCreaturesList[creature:getId()] or lastBattleButtonSwitched
  if battleButton then
    battleButton.isTarget = creature and true or false
    checkBattleButton(battleButton)
  end
end

function onFollow(creature)
  checkCreatures(true) --Force recheck
  local battleButton = creature and battleButtonsByCreaturesList[creature:getId()] or lastBattleButtonSwitched
  if battleButton then
    battleButton.isFollowed = creature and true or false
    checkBattleButton(battleButton)
  end
end

function checkBattleButton(battleButton)
  local color = BattleButtonColors.onIdle
  if battleButton.isTarget then
    color = BattleButtonColors.onTargeted
  elseif battleButton.isFollowed then
    color = BattleButtonColors.onFollowed
  end

  color = battleButton.isHovered and color.hovered or color.notHovered

  if battleButton.isHovered or battleButton.isTarget or battleButton.isFollowed then
    battleButton.creature:showStaticSquare(color)
    battleButton:getChildById('creature'):setBorderWidth(1)
    battleButton:getChildById('creature'):setBorderColor(color)
    battleButton:getChildById('label'):setColor(color)
  else
    battleButton.creature:hideStaticSquare()
    battleButton:getChildById('creature'):setBorderWidth(0)
    battleButton:getChildById('label'):setColor(color)
  end

  if battleButton.isTarget or battleButton.isFollowed then
    -- set new last battle button switched
    if lastBattleButtonSwitched and lastBattleButtonSwitched ~= battleButton then
      lastBattleButtonSwitched.isTarget = false
      lastBattleButtonSwitched.isFollowed = false
      checkBattleButton(lastBattleButtonSwitched)
    end
    lastBattleButtonSwitched = battleButton
  end
end
