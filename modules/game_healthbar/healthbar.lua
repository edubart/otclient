HealthBar = {}

-- private variables
local healthBarWindow
local healthBar
local manaBar
local healthLabel
local manaLabel

-- public functions
function HealthBar.create()
  healthBarWindow = displayUI('healthbar.otui', { parent = Game.gameRightPanel })
  healthBarButton = TopMenu.addGameButton('healthBarButton', 'Healh Bar', 'healthbar.png', HealthBar.toggle)
  healthBarButton:setOn(true)
  healthBar = healthBarWindow:getChildById('healthBar')
  manaBar = healthBarWindow:getChildById('manaBar')
  healthLabel = healthBarWindow:getChildById('healthLabel')
  manaLabel = healthBarWindow:getChildById('manaLabel')
end

function HealthBar.destroy()
  healthBarWindow:destroy()
  healthBarWindow = nil
  healthBarButton:destroy()
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
function HealthBar.onHealthChange(health, maxHealth)
  healthLabel:setText(health .. ' / ' .. maxHealth)
  healthBar:setPercent(health / maxHealth * 100)
end

function HealthBar.onManaChange(mana, maxMana)
  manaLabel:setText(mana .. ' / ' .. maxMana)

  local percent
  if maxMana == 0 then
    percent = 100
  else
    percent = (mana * 100)/maxMana
  end
  manaBar:setPercent(percent)
end

connect(Game, { onLogin = HealthBar.create,
                onLogout = HealthBar.destroy,
                onHealthChange = HealthBar.onHealthChange,
                onManaChange = HealthBar.onManaChange })
