Icons = {}
Icons[1] = { tooltip = tr('You are poisoned'), path = '/images/game/states/poisoned', id = 'condition_poisoned' }
Icons[2] = { tooltip = tr('You are burning'), path = '/images/game/states/burning', id = 'condition_burning' }
Icons[4] = { tooltip = tr('You are electrified'), path = '/images/game/states/electrified', id = 'condition_electrified' }
Icons[8] = { tooltip = tr('You are drunk'), path = '/images/game/states/drunk', id = 'condition_drunk' }
Icons[16] = { tooltip = tr('You are protected by a magic shield'), path = '/images/game/states/magic_shield', id = 'condition_magic_shield' }
Icons[32] = { tooltip = tr('You are paralysed'), path = '/images/game/states/slowed', id = 'condition_slowed' }
Icons[64] = { tooltip = tr('You are hasted'), path = '/images/game/states/haste', id = 'condition_haste' }
Icons[128] = { tooltip = tr('You may not logout during a fight'), path = '/images/game/states/logout_block', id = 'condition_logout_block' }
Icons[256] = { tooltip = tr('You are drowning'), path = '/images/game/states/drowning', id = 'condition_drowning' }
Icons[512] = { tooltip = tr('You are freezing'), path = '/images/game/states/freezing', id = 'condition_freezing' }
Icons[1024] = { tooltip = tr('You are dazzled'), path = '/images/game/states/dazzled', id = 'condition_dazzled' }
Icons[2048] = { tooltip = tr('You are cursed'), path = '/images/game/states/cursed', id = 'condition_cursed' }
Icons[4096] = { tooltip = tr('You are strengthened'), path = '/images/game/states/strengthened', id = 'condition_strengthened' }
Icons[8192] = { tooltip = tr('You may not logout or enter a protection zone'), path = '/images/game/states/protection_zone_block', id = 'condition_protection_zone_block' }
Icons[16384] = { tooltip = tr('You are within a protection zone'), path = '/images/game/states/protection_zone', id = 'condition_protection_zone' }
Icons[32768] = { tooltip = tr('You are bleeding'), path = '/images/game/states/bleeding', id = 'condition_bleeding' }
Icons[65536] = { tooltip = tr('You are hungry'), path = '/images/game/states/hungry', id = 'condition_hungry' }

healthInfoWindow = nil
healthBar = nil
manaBar = nil
experienceBar = nil
soulBar = nil
healthLabel = nil
manaLabel = nil
experienceLabel = nil
soulLabel = nil
capLabel = nil

function init()
  connect(LocalPlayer, { onHealthChange = onHealthChange,
                         onManaChange = onManaChange,
                         onLevelChange = onLevelChange,
                         onStatesChange = onStatesChange,
                         onSoulChange = onSoulChange,
                         onFreeCapacityChange = onFreeCapacityChange })

  connect(g_game, { onGameEnd = offline })

  healthInfoButton = modules.client_topmenu.addRightGameToggleButton('healthInfoButton', tr('Health Information'), '/images/topbuttons/healthinfo', toggle)
  healthInfoButton:setOn(true)

  healthInfoWindow = g_ui.loadUI('healthinfo', modules.game_interface.getRightPanel())
  healthInfoWindow:disableResize()
  healthBar = healthInfoWindow:recursiveGetChildById('healthBar')
  manaBar = healthInfoWindow:recursiveGetChildById('manaBar')
  experienceBar = healthInfoWindow:recursiveGetChildById('experienceBar')
  healthLabel = healthInfoWindow:recursiveGetChildById('healthLabel')
  manaLabel = healthInfoWindow:recursiveGetChildById('manaLabel')
  experienceLabel = healthInfoWindow:recursiveGetChildById('experienceLabel')
  soulBar = healthInfoWindow:recursiveGetChildById('soulBar')
  soulLabel = healthInfoWindow:recursiveGetChildById('soulLabel')
  capLabel = healthInfoWindow:recursiveGetChildById('capLabel')

  if g_game.isOnline() then
    local localPlayer = g_game.getLocalPlayer()
    onHealthChange(localPlayer, localPlayer:getHealth(), localPlayer:getMaxHealth())
    onManaChange(localPlayer, localPlayer:getMana(), localPlayer:getMaxMana())
    onLevelChange(localPlayer, localPlayer:getLevel(), localPlayer:getLevelPercent())
    onStatesChange(localPlayer, localPlayer:getStates(), 0)
    onSoulChange(localPlayer, localPlayer:getSoul())
    onFreeCapacityChange(localPlayer, localPlayer:getFreeCapacity())
  end

  healthInfoWindow:setup()
end

function terminate()
  disconnect(LocalPlayer, { onHealthChange = onHealthChange,
                            onManaChange = onManaChange,
                            onLevelChange = onLevelChange,
                            onStatesChange = onStatesChange,
                            onSoulChange = onSoulChange,
                            onFreeCapacityChange = onFreeCapacityChange })

  disconnect(g_game, { onGameEnd = offline })

  healthInfoWindow:destroy()
  healthInfoButton:destroy()
end

function toggle()
  if healthInfoButton:isOn() then
    healthInfoWindow:close()
    healthInfoButton:setOn(false)
  else
    healthInfoWindow:open()
    healthInfoButton:setOn(true)
  end
end

function hideLabels()
  capLabel:hide()
  soulLabel:hide()
  local removeHeight = capLabel:getHeight() + capLabel:getMarginTop() + capLabel:getMarginBottom()
  healthInfoWindow:setHeight(math.max(healthInfoWindow.minimizedHeight, healthInfoWindow:getHeight() - removeHeight))
end

function hideExperience()
  experienceBar:hide()
  experienceLabel:hide()
  local removeHeight = experienceBar:getHeight() + experienceBar:getMarginTop() + experienceBar:getMarginBottom()
  healthInfoWindow:setHeight(math.max(healthInfoWindow.minimizedHeight, healthInfoWindow:getHeight() - removeHeight))
end

function onMiniWindowClose()
  healthInfoButton:setOn(false)
end

function offline()
  healthInfoWindow:recursiveGetChildById('conditionPanel'):destroyChildren()
end

-- hooked events
function onHealthChange(localPlayer, health, maxHealth)
  healthLabel:setText(health .. ' / ' .. maxHealth)
  healthBar:setPercent(health / maxHealth * 100)
end

function onManaChange(localPlayer, mana, maxMana)
  manaLabel:setText(mana .. ' / ' .. maxMana)

  local percent
  if maxMana == 0 then
    percent = 100
  else
    percent = (mana * 100)/maxMana
  end
  manaBar:setPercent(percent)
end

function onLevelChange(localPlayer, value, percent)
  experienceLabel:setText(percent .. "%")
  experienceBar:setPercent(percent)
end

function onSoulChange(localPlayer, soul)
  soulLabel:setText(tr('Soul') .. ': ' .. soul)
end

function onFreeCapacityChange(player, freeCapacity)
  capLabel:setText(tr('Cap') .. ': ' .. freeCapacity)
end


function onStatesChange(localPlayer, now, old)
  if now == old then return end

  local bitsChanged = bit32.bxor(now, old)
  for i = 1, 32 do
    local pow = math.pow(2, i-1)
    if pow > bitsChanged then break end
    local bitChanged = bit32.band(bitsChanged, pow)
    if bitChanged ~= 0 then
      toggleIcon(bitChanged)
    end
  end
end

function toggleIcon(bitChanged)
  local content = healthInfoWindow:recursiveGetChildById('conditionPanel')

  local icon = content:getChildById(Icons[bitChanged].id)
  if icon then
    icon:destroy()
  else
    icon = g_ui.createWidget('ConditionWidget', content)
    icon:setId(Icons[bitChanged].id)
    icon:setImageSource(Icons[bitChanged].path)
    icon:setTooltip(Icons[bitChanged].tooltip)
  end
end