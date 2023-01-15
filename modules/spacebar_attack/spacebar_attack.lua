
spacebarAttackButton = nil

function init()
  spacebarAttackButton = modules.client_topmenu.addRightGameToggleButton('spacebarAttackButton', tr('Spacebar Attack'), '/spacebar_attack/spacebar', toggleSpacebarBind)
  spacebarAttackButton:setOn(false)
  if spacebarAttackButton:isOn() then
    bindSpacebar()
  else
    unbindSpacebar()
  end
end

function terminate()
  unbindSpacebar()
  spacebarAttackButton:destroy()
end

-------------------------------------------------
--Scripts----------------------------------------
-------------------------------------------------

function getSortedBattleList()
  local unsortedList = table.copy(modules.game_battle.battleButtonsByCreaturesList)

  local tableCount = table.size(unsortedList)

  local sortedTable = {}

  for i=1,tableCount do
    local helpValue = nil
    
    for a,b in pairs(unsortedList) do
      if helpValue == nil then
        helpValue = {creatureId = a, creatureData = b}
      else
        if b:getY() < helpValue.creatureData:getY() then
          helpValue = {creatureId = a, creatureData = b}
        end
      end
    end

    sortedTable[i] = helpValue
    unsortedList[helpValue.creatureId] = nil
  end

  return sortedTable
end

local battleListCounter = 1
local lastAttackedMonster = 0

function chooseAimFromBattleList()
  local battleListTable = getSortedBattleList()
  local tableCount = table.size(battleListTable)

  if battleListCounter > tableCount then
    battleListCounter = 1
  end

  for i=1,tableCount do
    if battleListCounter > 1 and g_game.isAttacking() == false and lastAttackedMonster ~= battleListTable[battleListCounter - 1].creatureId then
      battleListCounter = battleListCounter - 1
    end

    if isNpcOrSafeFight(battleListTable[battleListCounter].creatureId) then
      g_game.attack(g_map.getCreatureById(battleListTable[battleListCounter].creatureId))
      lastAttackedMonster = battleListTable[battleListCounter].creatureId
      battleListCounter = battleListCounter + 1
      return
    end
    battleListCounter = battleListCounter + 1

    if battleListCounter > tableCount then
      battleListCounter = 1
    end
  end
end

function isNpcOrSafeFight(creatureId)
  local creatureData = g_map.getCreatureById(creatureId)

  if creatureData:isMonster() then
    return true
  elseif creatureData:isPlayer() then
    if g_game.isSafeFight() then
      return false
    else
      return true
    end
  elseif creatureData:isNpc() then
    return false
  else
    return false
  end
end

function bindSpacebar()
  g_keyboard.bindKeyPress('Space', chooseAimFromBattleList)
end

function unbindSpacebar()
  g_keyboard.unbindKeyPress('Space', chooseAimFromBattleList)
end

function toggleSpacebarBind()
  if spacebarAttackButton:isOn() then
    unbindSpacebar()
    spacebarAttackButton:setOn(false)
  else
    bindSpacebar()
    spacebarAttackButton:setOn(true)
  end
end

-------------------------------------------------
--Scripts END------------------------------------
-------------------------------------------------