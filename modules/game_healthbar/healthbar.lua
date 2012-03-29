HealthBar = {}

-- constants
local Icons = {}
Icons[1] = { tooltip = 'You are poisoned', path = '/game_healthbar/icons/poisoned.png', id = 'condition_poisoned' }
Icons[2] = { tooltip = 'You are burning', path = '/game_healthbar/icons/burning.png', id = 'condition_burning' }
Icons[4] = { tooltip = 'You are electrified', path = '/game_healthbar/icons/electrified.png', id = 'condition_electrified' }
Icons[8] = { tooltip = 'You are freezing', path = '/game_healthbar/icons/drunk.png', id = 'condition_drunk' }
Icons[16] = { tooltip = 'You are protected by a magic shield', path = '/game_healthbar/icons/magic_shield.png', id = 'condition_magic_shield' }
Icons[32] = { tooltip = 'You are paralysed', path = '/game_healthbar/icons/slowed.png', id = 'condition_slowed' }
Icons[64] = { tooltip = 'You are hasted', path = '/game_healthbar/icons/haste.png', id = 'condition_haste' }
Icons[128] = { tooltip = 'You may not logout during a fight', path = '/game_healthbar/icons/logout_block.png', id = 'condition_logout_block' }
Icons[256] = { tooltip = 'You are drowing', path = '/game_healthbar/icons/drowning.png', id = 'condition_drowning' }
Icons[512] = { tooltip = 'You are freezing', path = '/game_healthbar/icons/freezing.png', id = 'condition_freezing' }
Icons[1024] = { tooltip = 'You are dazzled', path = '/game_healthbar/icons/dazzled.png', id = 'condition_dazzled' }
Icons[2048] = { tooltip = 'You are cursed', path = '/game_healthbar/icons/cursed.png', id = 'condition_cursed' }
Icons[4096] = { tooltip = 'You are strenghtened', path = '/game_healthbar/icons/strengthened.png', id = 'condition_strengthened' }
Icons[8192] = { tooltip = 'You may not logout or enter a protection zone', path = '/game_healthbar/icons/protection_zone_block.png', id = 'condition_protection_zone_block' }
Icons[16384] = { tooltip = 'You are within a protection zone', path = '/game_healthbar/icons/protection_zone.png', id = 'condition_protection_zone' }

-- private variables
local healthBarWindow
local healthBar
local manaBar
local healthLabel
local manaLabel

-- public functions
function HealthBar.init()
  connect(LocalPlayer, { onHealthChange = HealthBar.onHealthChange,
                         onManaChange = HealthBar.onManaChange,
                         onStatesChange = HealthBar.onStatesChange })
                         
  connect(g_game, { onGameEnd = HealthBar.offline })

  healthBarWindow = displayUI('healthbar.otui', GameInterface.getLeftPanel())
  healthBarButton = TopMenu.addGameToggleButton('healthBarButton', 'Healh Bar', 'healthbar.png', HealthBar.toggle)
  healthBarButton:setOn(true)
  healthBar = healthBarWindow:recursiveGetChildById('healthBar')
  manaBar = healthBarWindow:recursiveGetChildById('manaBar')
  healthLabel = healthBarWindow:recursiveGetChildById('healthLabel')
  manaLabel = healthBarWindow:recursiveGetChildById('manaLabel')

  if g_game.isOnline() then
    local localPlayer = g_game.getLocalPlayer()
    HealthBar.onHealthChange(localPlayer, localPlayer:getHealth(), localPlayer:getMaxHealth())
    HealthBar.onManaChange(localPlayer, localPlayer:getMana(), localPlayer:getMaxMana())
  end
end

function HealthBar.terminate()
  disconnect(LocalPlayer, { onHealthChange = HealthBar.onHealthChange,
                            onManaChange = HealthBar.onManaChange,
                            onStatesChange = HealthBar.onStatesChange })
                            
  disconnect(g_game, { onGameEnd = HealthBar.offline })

  healthBarWindow:destroy()
  healthBarButton:destroy()
  healthBarWindow = nil
  healthBarButton = nil
  healthBar = nil
  manaBar = nil
  healthLabel = nil
  manaLabel = nil
end

function HealthBar.toggle()
  local visible = not healthBarWindow:isExplicitlyVisible()
  healthBarWindow:setVisible(visible)
  healthBarButton:setOn(visible)
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
  local bitsChanged = bit32.bxor(now, old)
  for i = 1, 32 do
    if i > bitsChanged then break end
    local bitChanged = bit32.band(bitsChanged, math.pow(2, i))
    if bitChanged ~= 0 then
      HealthBar.toggleIcon(bitChanged)
    end
  end
end

function HealthBar.toggleIcon(bitChanged)
  local content = healthBarWindow:recursiveGetChildById('conditions_content')
  
  local icon = content:getChildById(Icons[bitChanged].id)
  if icon then
    icon:destroy()
  else
    icon = createWidget('ConditionWidget', content)
    icon:setId(Icons[bitChanged].id)
    icon:setImageSource(Icons[bitChanged].path)
    icon:setTooltip(Icons[bitChanged].tooltip)
  end
end

function HealthBar.offline()
  local conditionsContent = healthBarWindow:recursiveGetChildById('conditions_content')
  local count = conditionsContent:getChildCount()  
  for i = count, 1, -1 do conditionsContent:getChildByIndex(i):destroy() end
end

