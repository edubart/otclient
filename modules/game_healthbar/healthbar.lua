HealthBar = {}

-- private variables
local healthBarWindow
local healthBar
local manaBar
local healthLabel
local manaLabel

-- public functions
function HealthBar.init()
  connect(LocalPlayer, { onHealthChange = HealthBar.onHealthChange,
                         onManaChange = HealthBar.onManaChange })

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
                            onManaChange = HealthBar.onManaChange })

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

