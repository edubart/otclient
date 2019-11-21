battleWindow = nil
battleButton = nil
battlePanel = nil
filterPanel = nil
toggleFilterButton = nil
lastBattleButtonSwitched = nil
battleButtonsByCreaturesList = {}
creatureAgeList = {}

mouseWidget = nil

sortTypeBox = nil
sortOrderBox = nil
hidePlayersButton = nil
hideNPCsButton = nil
hideMonstersButton = nil
hideSkullsButton = nil
hidePartyButton = nil

function init()
  g_ui.importStyle('battlebutton')
  battleButton = modules.client_topmenu.addRightGameToggleButton('battleButton', tr('Battle') .. ' (Ctrl+B)', '/images/topbuttons/battle', toggle)
  battleButton:setOn(true)
  battleWindow = g_ui.loadUI('battle', modules.game_interface.getRightPanel())
  g_keyboard.bindKeyDown('Ctrl+B', toggle)

  -- this disables scrollbar auto hiding
  local scrollbar = battleWindow:getChildById('miniwindowScrollBar')
  scrollbar:mergeStyle({ ['$!on'] = { }})

  battlePanel = battleWindow:recursiveGetChildById('battlePanel')

  filterPanel = battleWindow:recursiveGetChildById('filterPanel')
  toggleFilterButton = battleWindow:recursiveGetChildById('toggleFilterButton')

  if isHidingFilters() then
    hideFilterPanel()
  end

  sortTypeBox = battleWindow:recursiveGetChildById('sortTypeBox')
  sortOrderBox = battleWindow:recursiveGetChildById('sortOrderBox')
  hidePlayersButton = battleWindow:recursiveGetChildById('hidePlayers')
  hideNPCsButton = battleWindow:recursiveGetChildById('hideNPCs')
  hideMonstersButton = battleWindow:recursiveGetChildById('hideMonsters')
  hideSkullsButton = battleWindow:recursiveGetChildById('hideSkulls')
  hidePartyButton = battleWindow:recursiveGetChildById('hideParty')

  mouseWidget = g_ui.createWidget('UIButton')
  mouseWidget:setVisible(false)
  mouseWidget:setFocusable(false)
  mouseWidget.cancelNextRelease = false

  battleWindow:setContentMinimumHeight(80)

  sortTypeBox:addOption('Name', 'name')
  sortTypeBox:addOption('Distance', 'distance')
  sortTypeBox:addOption('Age', 'age')
  sortTypeBox:addOption('Health', 'health')
  sortTypeBox:setCurrentOptionByData(getSortType())
  sortTypeBox.onOptionChange = onChangeSortType

  sortOrderBox:addOption('Asc.', 'asc')
  sortOrderBox:addOption('Desc.', 'desc')
  sortOrderBox:setCurrentOptionByData(getSortOrder())
  sortOrderBox.onOptionChange = onChangeSortOrder

  connect(Creature, {
    onSkullChange = updateCreatureSkull,
    onEmblemChange = updateCreatureEmblem,
    onOutfitChange = onCreatureOutfitChange,
    onHealthPercentChange = onCreatureHealthPercentChange,
    onPositionChange = onCreaturePositionChange,
    onAppear = onCreatureAppear,
    onDisappear = onCreatureDisappear
  })

  connect(LocalPlayer, {
    onPositionChange = onCreaturePositionChange
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
    onOutfitChange = onCreatureOutfitChange,
    onHealthPercentChange = onCreatureHealthPercentChange,
    onPositionChange = onCreaturePositionChange,
    onAppear = onCreatureAppear,
    onDisappear = onCreatureDisappear
  })

  disconnect(LocalPlayer, {
    onPositionChange = onCreaturePositionChange
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

function getSortType()
  local settings = g_settings.getNode('BattleList')
  if not settings then
    return 'name'
  end
  return settings['sortType']
end

function setSortType(state)
  settings = {}
  settings['sortType'] = state
  g_settings.mergeNode('BattleList', settings)

  checkCreatures()
end

function getSortOrder()
  local settings = g_settings.getNode('BattleList')
  if not settings then
    return 'asc'
  end
  return settings['sortOrder']
end

function setSortOrder(state)
  settings = {}
  settings['sortOrder'] = state
  g_settings.mergeNode('BattleList', settings)

  checkCreatures()
end

function isSortAsc()
    return getSortOrder() == 'asc'
end

function isSortDesc()
    return getSortOrder() == 'desc'
end

function isHidingFilters()
  local settings = g_settings.getNode('BattleList')
  if not settings then
    return false
  end
  return settings['hidingFilters']
end

function setHidingFilters(state)
  settings = {}
  settings['hidingFilters'] = state
  g_settings.mergeNode('BattleList', settings)
end

function hideFilterPanel()
  filterPanel.originalHeight = filterPanel:getHeight()
  filterPanel:setHeight(0)
  toggleFilterButton:getParent():setMarginTop(0)
  toggleFilterButton:setImageClip(torect("0 0 21 12"))
  setHidingFilters(true)
  filterPanel:setVisible(false)
end

function showFilterPanel()
  toggleFilterButton:getParent():setMarginTop(5)
  filterPanel:setHeight(filterPanel.originalHeight)
  toggleFilterButton:setImageClip(torect("21 0 21 12"))
  setHidingFilters(false)
  filterPanel:setVisible(true)
end

function toggleFilterPanel()
  if filterPanel:isVisible() then
    hideFilterPanel()
  else
    showFilterPanel()
  end
end

function onChangeSortType(comboBox, option)
  setSortType(option:lower())
end

function onChangeSortOrder(comboBox, option)
  -- Replace dot in option name
  setSortOrder(option:lower():gsub('[.]', ''))
end

function checkCreatures()
  removeAllCreatures()

  if not g_game.isOnline() then
    return
  end

  local player = g_game.getLocalPlayer()
  local spectators = g_map.getSpectators(player:getPosition(), false)
  for _, creature in ipairs(spectators) do
    if doCreatureFitFilters(creature) then
      addCreature(creature)
    end
  end
end

function doCreatureFitFilters(creature)
  if creature:isLocalPlayer() then
    return false
  end

  local pos = creature:getPosition()
  if not pos then return false end

  local localPlayer = g_game.getLocalPlayer()
  if pos.z ~= localPlayer:getPosition().z or not creature:canBeSeen() then return false end
  return true
end

function doShowCreatureAtBattle(creature)
  if doCreatureFitFilters(creature) then
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
  return false
end

function onCreatureHealthPercentChange(creature, health)
  local battleButton = battleButtonsByCreaturesList[creature:getId()]
  if battleButton then
    if getSortType() == 'health' then
      removeCreature(creature)
      addCreature(creature)
      return
    end
    battleButton:setLifeBarPercent(creature:getHealthPercent())
  end
end

local function getDistanceBetween(p1, p2)
    return math.max(math.abs(p1.x - p2.x), math.abs(p1.y - p2.y))
end

function onCreaturePositionChange(creature, newPos, oldPos)
  if creature:isLocalPlayer() then
    if oldPos and newPos and newPos.z ~= oldPos.z then
      checkCreatures()
    else
      -- Distance will change when moving, recalculate and move to correct index
      if getSortType() == 'distance' then
        local distanceList = {}
        for _, battleButton in pairs(battleButtonsByCreaturesList) do
          table.insert(distanceList, {distance = getDistanceBetween(newPos, battleButton.creature:getPosition()), widget = battleButton})
        end

        if isSortAsc() then
          table.sort(distanceList, function(a, b) return a.distance < b.distance end)
        else
          table.sort(distanceList, function(a, b) return a.distance > b.distance end)
        end

        for i = 1, #distanceList do
          battlePanel:moveChildToIndex(distanceList[i].widget, i)
        end
      end

      for _, battleButton in pairs(battleButtonsByCreaturesList) do
        addCreature(battleButton.creature)
      end
    end
  else
    local has = hasCreature(creature)
    local fit = doCreatureFitFilters(creature)
    if has and not fit then
      removeCreature(creature)
    elseif fit then
      if has and getSortType() == 'distance' then
        removeCreature(creature)
      end
      addCreature(creature)
    end
  end
end

function onCreatureOutfitChange(creature, outfit, oldOutfit)
  if doCreatureFitFilters(creature) then
    addCreature(creature)
  else
    removeCreature(creature)
  end
end

function onCreatureAppear(creature)
  if creature:isLocalPlayer() then
    addEvent(function()
      updateStaticSquare()
    end)
  end

  if doCreatureFitFilters(creature) then
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

  -- Register when creature is added to battlelist for the first time
  if not creatureAgeList[creatureId] then
    creatureAgeList[creatureId] = os.time()
  end

  if not battleButton then
    battleButton = g_ui.createWidget('BattleButton')
    battleButton:setup(creature)

    battleButton.onHoverChange = onBattleButtonHoverChange
    battleButton.onMouseRelease = onBattleButtonMouseRelease

    battleButtonsByCreaturesList[creatureId] = battleButton

    if creature == g_game.getAttackingCreature() then
      onAttack(creature)
    end

    if creature == g_game.getFollowingCreature() then
      onFollow(creature)
    end

    local inserted = false
    local nameLower = creature:getName():lower()
    local healthPercent = creature:getHealthPercent()
    local playerPosition = g_game.getLocalPlayer():getPosition()
    local distance = getDistanceBetween(playerPosition, creature:getPosition())
    local age = creatureAgeList[creatureId]

    local childCount = battlePanel:getChildCount()
    for i = 1, childCount do
      local child = battlePanel:getChildByIndex(i)
      local childName = child:getCreature():getName():lower()
      local equal = false
      if getSortType() == 'age' then
        local childAge = creatureAgeList[child:getCreature():getId()]
        if (age < childAge and isSortAsc()) or (age > childAge and isSortDesc()) then
          battlePanel:insertChild(i, battleButton)
          inserted = true
          break
        elseif age == childAge then
          equal = true
        end
      elseif getSortType() == 'distance' then
        local childDistance = getDistanceBetween(child:getCreature():getPosition(), playerPosition)
        if (distance < childDistance and isSortAsc()) or (distance > childDistance and isSortDesc()) then
          battlePanel:insertChild(i, battleButton)
          inserted = true
          break
        elseif childDistance == distance then
          equal = true
        end
      elseif getSortType() == 'health' then
          local childHealth = child:getCreature():getHealthPercent()
          if (healthPercent < childHealth and isSortAsc()) or (healthPercent > childHealth and isSortDesc()) then
            battlePanel:insertChild(i, battleButton)
            inserted = true
            break
          elseif healthPercent == childHealth then
            equal = true
          end
      end

      -- If any other sort type is selected and values are equal, sort it by name also
      if getSortType() == 'name' or equal then
          local length = math.min(childName:len(), nameLower:len())
          for j=1,length do
            if (nameLower:byte(j) < childName:byte(j) and isSortAsc()) or (nameLower:byte(j) > childName:byte(j) and isSortDesc()) then
              battlePanel:insertChild(i, battleButton)
              inserted = true
              break
            elseif (nameLower:byte(j) > childName:byte(j) and isSortAsc()) or (nameLower:byte(j) < childName:byte(j) and isSortDesc()) then
              break
            elseif j == nameLower:len() and isSortAsc() then
              battlePanel:insertChild(i, battleButton)
              inserted = true
            elseif j == childName:len() and isSortDesc() then
              battlePanel:insertChild(i, battleButton)
              inserted = true
            end
          end
      end

      if inserted then
        break
      end
    end

    -- Insert at the end if no other place is found
    if not inserted then
      battlePanel:insertChild(childCount + 1, battleButton)
    end
  else
    battleButton:setLifeBarPercent(creature:getHealthPercent())
  end

  local localPlayer = g_game.getLocalPlayer()
  battleButton:setVisible(localPlayer:hasSight(creature:getPosition()) and creature:canBeSeen() and doShowCreatureAtBattle(creature))
end

function removeAllCreatures()
  creatureAgeList = {}
  for _, battleButton in pairs(battleButtonsByCreaturesList) do
    removeCreature(battleButton.creature)
  end
end

function removeCreature(creature)
  if hasCreature(creature) then
    local creatureId = creature:getId()

    if lastBattleButtonSwitched == battleButtonsByCreaturesList[creatureId] then
      lastBattleButtonSwitched = nil
    end

    battleButtonsByCreaturesList[creatureId].creature:hideStaticSquare()
    battleButtonsByCreaturesList[creatureId]:destroy()
    battleButtonsByCreaturesList[creatureId] = nil
  end
end

function onBattleButtonMouseRelease(self, mousePosition, mouseButton)
  if mouseWidget.cancelNextRelease then
    mouseWidget.cancelNextRelease = false
    return false
  end
  if ((g_mouse.isPressed(MouseLeftButton) and mouseButton == MouseRightButton)
    or (g_mouse.isPressed(MouseRightButton) and mouseButton == MouseLeftButton)) then
    mouseWidget.cancelNextRelease = true
    g_game.look(self.creature, true)
    return true
  elseif mouseButton == MouseLeftButton and g_keyboard.isShiftPressed() then
    g_game.look(self.creature, true)
    return true
  elseif mouseButton == MouseRightButton and not g_mouse.isPressed(MouseLeftButton) then
    modules.game_interface.createThingMenu(mousePosition, nil, nil, self.creature)
    return true
  elseif mouseButton == MouseLeftButton and not g_mouse.isPressed(MouseRightButton) then
    if self.isTarget then
      g_game.cancelAttack()
    else
      g_game.attack(self.creature)
    end
    return true
  end
  return false
end

function onBattleButtonHoverChange(battleButton, hovered)
  if battleButton.isBattleButton then
    battleButton.isHovered = hovered
    updateBattleButton(battleButton)
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

function updateStaticSquare()
  for _, battleButton in pairs(battleButtonsByCreaturesList) do
    if battleButton.isTarget then
      battleButton:update()
    end
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
