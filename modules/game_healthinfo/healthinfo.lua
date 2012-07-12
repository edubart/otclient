HealthInfo = {}

-- constants
local Icons = {}
Icons[1] = { tooltip = tr('You are poisoned'), path = '/game_healthinfo/icons/poisoned.png', id = 'condition_poisoned' }
Icons[2] = { tooltip = tr('You are burning'), path = '/game_healthinfo/icons/burning.png', id = 'condition_burning' }
Icons[4] = { tooltip = tr('You are electrified'), path = '/game_healthinfo/icons/electrified.png', id = 'condition_electrified' }
Icons[8] = { tooltip = tr('You are freezing'), path = '/game_healthinfo/icons/drunk.png', id = 'condition_drunk' }
Icons[16] = { tooltip = tr('You are protected by a magic shield'), path = '/game_healthinfo/icons/magic_shield.png', id = 'condition_magic_shield' }
Icons[32] = { tooltip = tr('You are paralysed'), path = '/game_healthinfo/icons/slowed.png', id = 'condition_slowed' }
Icons[64] = { tooltip = tr('You are hasted'), path = '/game_healthinfo/icons/haste.png', id = 'condition_haste' }
Icons[128] = { tooltip = tr('You may not logout during a fight'), path = '/game_healthinfo/icons/logout_block.png', id = 'condition_logout_block' }
Icons[256] = { tooltip = tr('You are drowing'), path = '/game_healthinfo/icons/drowning.png', id = 'condition_drowning' }
Icons[512] = { tooltip = tr('You are freezing'), path = '/game_healthinfo/icons/freezing.png', id = 'condition_freezing' }
Icons[1024] = { tooltip = tr('You are dazzled'), path = '/game_healthinfo/icons/dazzled.png', id = 'condition_dazzled' }
Icons[2048] = { tooltip = tr('You are cursed'), path = '/game_healthinfo/icons/cursed.png', id = 'condition_cursed' }
Icons[4096] = { tooltip = tr('You are strengthened'), path = '/game_healthinfo/icons/strengthened.png', id = 'condition_strengthened' }
Icons[8192] = { tooltip = tr('You may not logout or enter a protection zone'), path = '/game_healthinfo/icons/protection_zone_block.png', id = 'condition_protection_zone_block' }
Icons[16384] = { tooltip = tr('You are within a protection zone'), path = '/game_healthinfo/icons/protection_zone.png', id = 'condition_protection_zone' }
Icons[32768] = { tooltip = tr('You are bleeding'), path = '/game_healthinfo/icons/bleeding.png', id = 'condition_bleeding' }
Icons[65536] = { tooltip = tr('You are hungry'), path = '/game_healthinfo/icons/hungry.png', id = 'condition_hungry' }

-- private variables
local healthInfoWindow
local healthBar
local manaBar
local soulBar
local healthLabel
local manaLabel
local soulLabel

-- public functions
function HealthInfo.init()
  connect(LocalPlayer, { onHealthChange = HealthInfo.onHealthChange,
                         onManaChange = HealthInfo.onManaChange,
                         onStatesChange = HealthInfo.onStatesChange,
                         onSoulChange = HealthInfo.onSoulChange })

  connect(g_game, { onGameEnd = HealthInfo.offline })

  healthInfoWindow = g_ui.loadUI('healthinfo.otui', GameInterface.getRightPanel())
  healthInfoButton = TopMenu.addRightGameToggleButton('healthInfoButton', tr('Health Information'), 'healthinfo.png', HealthInfo.toggle)
  healthInfoButton:setOn(true)
  healthBar = healthInfoWindow:recursiveGetChildById('healthBar')
  manaBar = healthInfoWindow:recursiveGetChildById('manaBar')
  healthLabel = healthInfoWindow:recursiveGetChildById('healthLabel')
  manaLabel = healthInfoWindow:recursiveGetChildById('manaLabel')
  soulBar = healthInfoWindow:recursiveGetChildById('soulBar')
  soulLabel = healthInfoWindow:recursiveGetChildById('soulLabel')

  if g_game.isOnline() then
    local localPlayer = g_game.getLocalPlayer()
    HealthInfo.onHealthChange(localPlayer, localPlayer:getHealth(), localPlayer:getMaxHealth())
    HealthInfo.onManaChange(localPlayer, localPlayer:getMana(), localPlayer:getMaxMana())
    HealthInfo.onStatesChange(localPlayer, localPlayer:getStates(), 0)
    HealthInfo.onSoulChange(localPlayer, localPlayer:getSoul())
  end
end

function HealthInfo.terminate()
  disconnect(LocalPlayer, { onHealthChange = HealthInfo.onHealthChange,
                            onManaChange = HealthInfo.onManaChange,
                            onStatesChange = HealthInfo.onStatesChange,
                            onSoulChange = HealthInfo.onSoulChange })

  disconnect(g_game, { onGameEnd = HealthInfo.offline })

  healthInfoWindow:destroy()
  healthInfoButton:destroy()
  healthInfoWindow = nil
  healthInfoButton = nil
  
  healthBar = nil
  manaBar = nil
  soulBar = nil
  
  healthLabel = nil
  manaLabel = nil
  soulLabel = nil

  HealthInfo = nil
end

function HealthInfo.toggle()
  if healthInfoButton:isOn() then
    healthInfoWindow:close()
    healthInfoButton:setOn(false)
  else
    healthInfoWindow:open()
    healthInfoButton:setOn(true)
  end
end

function HealthInfo.onMiniWindowClose()
  healthInfoButton:setOn(false)
end

function HealthInfo.offline()
  healthInfoWindow:recursiveGetChildById('conditionPanel'):destroyChildren()
end

-- hooked events
function HealthInfo.onHealthChange(localPlayer, health, maxHealth)
  healthLabel:setText(health .. ' / ' .. maxHealth)
  healthBar:setPercent(health / maxHealth * 100)
end

function HealthInfo.onManaChange(localPlayer, mana, maxMana)
  manaLabel:setText(mana .. ' / ' .. maxMana)

  local percent
  if maxMana == 0 then
    percent = 100
  else
    percent = (mana * 100)/maxMana
  end
  manaBar:setPercent(percent)
end

function HealthInfo.onSoulChange(localPlayer, soul)
  soulLabel:setText('Soul: ' .. soul)
end

function HealthInfo.onStatesChange(localPlayer, now, old)
  if now == old then return end

  local bitsChanged = bit32.bxor(now, old)
  for i = 1, 32 do
    local pow = math.pow(2, i-1)
    if pow > bitsChanged then break end
    local bitChanged = bit32.band(bitsChanged, pow)
    if bitChanged ~= 0 then
      HealthInfo.toggleIcon(bitChanged)
    end
  end
end

function HealthInfo.toggleIcon(bitChanged)
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

