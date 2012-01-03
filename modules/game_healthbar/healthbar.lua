HealthBar = {}

-- private variables
local healthManaPanel
local healthBar
local manaBar
local healthLabel
local manaLabel

-- public functions
function HealthBar.create()
  healthManaPanel = displayUI('healthbar.otui', { parent = Game.gameRightPanel })
  healthBar = healthManaPanel:getChildById('healthBar')
  manaBar = healthManaPanel:getChildById('manaBar')
  healthLabel = healthManaPanel:getChildById('healthLabel')
  manaLabel = healthManaPanel:getChildById('manaLabel')
end

function HealthBar.destroy()
  healthManaPanel:destroy()
  healthManaPanel = nil
  healthBar = nil
  manaBar = nil
  healthLabel = nil
  manaLabel = nil
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
