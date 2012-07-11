HealthBar = {}

-- constants
local Icons = {}
Icons[1] = { tooltip = tr('You are poisoned'), path = '/game_healthbar/icons/poisoned.png', id = 'condition_poisoned' }
Icons[2] = { tooltip = tr('You are burning'), path = '/game_healthbar/icons/burning.png', id = 'condition_burning' }
Icons[4] = { tooltip = tr('You are electrified'), path = '/game_healthbar/icons/electrified.png', id = 'condition_electrified' }
Icons[8] = { tooltip = tr('You are freezing'), path = '/game_healthbar/icons/drunk.png', id = 'condition_drunk' }
Icons[16] = { tooltip = tr('You are protected by a magic shield'), path = '/game_healthbar/icons/magic_shield.png', id = 'condition_magic_shield' }
Icons[32] = { tooltip = tr('You are paralysed'), path = '/game_healthbar/icons/slowed.png', id = 'condition_slowed' }
Icons[64] = { tooltip = tr('You are hasted'), path = '/game_healthbar/icons/haste.png', id = 'condition_haste' }
Icons[128] = { tooltip = tr('You may not logout during a fight'), path = '/game_healthbar/icons/logout_block.png', id = 'condition_logout_block' }
Icons[256] = { tooltip = tr('You are drowing'), path = '/game_healthbar/icons/drowning.png', id = 'condition_drowning' }
Icons[512] = { tooltip = tr('You are freezing'), path = '/game_healthbar/icons/freezing.png', id = 'condition_freezing' }
Icons[1024] = { tooltip = tr('You are dazzled'), path = '/game_healthbar/icons/dazzled.png', id = 'condition_dazzled' }
Icons[2048] = { tooltip = tr('You are cursed'), path = '/game_healthbar/icons/cursed.png', id = 'condition_cursed' }
Icons[4096] = { tooltip = tr('You are strengthened'), path = '/game_healthbar/icons/strengthened.png', id = 'condition_strengthened' }
Icons[8192] = { tooltip = tr('You may not logout or enter a protection zone'), path = '/game_healthbar/icons/protection_zone_block.png', id = 'condition_protection_zone_block' }
Icons[16384] = { tooltip = tr('You are within a protection zone'), path = '/game_healthbar/icons/protection_zone.png', id = 'condition_protection_zone' }
Icons[32768] = { tooltip = tr('You are bleeding'), path = '/game_healthbar/icons/bleeding.png', id = 'condition_bleeding' }
Icons[65536] = { tooltip = tr('You are hungry'), path = '/game_healthbar/icons/hungry.png', id = 'condition_hungry' }

-- private variables
local healthBarWindow
local healthBar
local manaBar
local healthLabel
local manaLabel

local soulLabel
local soulPoints

-- public functions
function HealthBar.init()
  connect(LocalPlayer, { onHealthChange = HealthBar.onHealthChange,
                         onManaChange = HealthBar.onManaChange,
                         onStatesChange = HealthBar.onStatesChange,
                         onSoulChange = HealthBar.onSoulChange })

  connect(g_game, { onGameEnd = HealthBar.offline })

  healthBarWindow = g_ui.loadUI('healthbar.otui', GameInterface.getRightPanel())
  healthBarButton = TopMenu.addGameToggleButton('healthBarButton', tr('Health Bar'), 'healthbar.png', HealthBar.toggle)
  healthBarButton:setOn(true)
  healthBar = healthBarWindow:recursiveGetChildById('healthBar')
  manaBar = healthBarWindow:recursiveGetChildById('manaBar')
  healthLabel = healthBarWindow:recursiveGetChildById('healthLabel')
  manaLabel = healthBarWindow:recursiveGetChildById('manaLabel')

  soulLabel = healthBarWindow:recursiveGetChildById('soulLabel')

  if g_game.isOnline() then
    local localPlayer = g_game.getLocalPlayer()
    HealthBar.onHealthChange(localPlayer, localPlayer:getHealth(), localPlayer:getMaxHealth())
    HealthBar.onManaChange(localPlayer, localPlayer:getMana(), localPlayer:getMaxMana())
    HealthBar.onStatesChange(localPlayer, localPlayer:getStates(), 0)
    HealthBar.onSoulChange(localPlayer, localPlayer:getSoul())
  end
end

function HealthBar.terminate()
  disconnect(LocalPlayer, { onHealthChange = HealthBar.onHealthChange,
                            onManaChange = HealthBar.onManaChange,
                            onStatesChange = HealthBar.onStatesChange,
                            onSoulChange = HealthBar.onSoulChange })

  disconnect(g_game, { onGameEnd = HealthBar.offline })

  healthBarWindow:destroy()
  healthBarButton:destroy()
  healthBarWindow = nil
  healthBarButton = nil
  healthBar = nil
  manaBar = nil
  healthLabel = nil
  manaLabel = nil
  
  soulLabel = nil

  HealthBar = nil
end

function HealthBar.toggle()
  if healthBarButton:isOn() then
    healthBarWindow:close()
    healthBarButton:setOn(false)
  else
    healthBarWindow:open()
    healthBarButton:setOn(true)
  end
end

function HealthBar.onMiniWindowClose()
  healthBarButton:setOn(false)
end

function HealthBar.offline()
  healthBarWindow:recursiveGetChildById('conditionPanel'):destroyChildren()
end

-- hooked events
function HealthBar.onHealthChange(localPlayer, health, maxHealth)
  healthLabel:setText(health .. ' / ' .. maxHealth)
  healthBar:setPercent(health / maxHealth * 100)
end

function HealthBar.onManaChange(localPlayer, mana, maxMana)
  manaLabel:setText(mana .. ' / ' .. maxMana)

  local percent
  if maxMana == 0 then
    percent = 100
  else
    percent = (mana * 100)/maxMana
  end
  manaBar:setPercent(percent)
end

function HealthBar.onStatesChange(localPlayer, now, old)
  if now == old then return end

  local bitsChanged = bit32.bxor(now, old)
  for i = 1, 32 do
    local pow = math.pow(2, i-1)
    if pow > bitsChanged then break end
    local bitChanged = bit32.band(bitsChanged, pow)
    if bitChanged ~= 0 then
      HealthBar.toggleIcon(bitChanged)
    end
  end
end

function HealthBar.onSoulChange(localPlayer, soul)
  soulLabel:setText('Soul: '.. soul)
end

function HealthBar.toggleIcon(bitChanged)
  local content = healthBarWindow:recursiveGetChildById('conditionPanel')

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

