HealthBar = {}

-- private variables
local healthManaPanel = nil

-- public functions
function HealthBar.create()
  healthManaPanel = displayUI('healthbar.otui', { parent = Game.gameRightPanel })

  local healthBar = createWidget('HealthBar', healthManaPanel)
  healthBar:setId('healthBar')

  local healthLabel = createWidget('HealthLabel', healthManaPanel)
  healthLabel:setId('healthLabel')
  healthLabel:setText('0 / 0')

  local manaBar = createWidget('ManaBar', healthManaPanel)
  manaBar:setId('manaBar')

  local manaLabel = createWidget('ManaLabel', healthManaPanel)
  manaLabel:setId('manaLabel')
  manaLabel:setText('0 / 0')

  healthManaPanel:setHeight(healthBar:getHeight() + manaBar:getHeight() + 4)
end

function HealthBar.destroy()
  healthManaPanel:destroy()
  healthManaPanel = nil
end

-- hooked events
function HealthBar.onHealthChange(health, maxHealth)
  local label = healthManaPanel:getChildById('healthLabel')
  label:setText(health .. ' / ' .. maxHealth)

  local healthBar = healthManaPanel:getChildById('healthBar')
  healthBar:setPercent(health / maxHealth * 100)
end

function HealthBar.onManaChange(mana, maxMana)
  local label = healthManaPanel:getChildById('manaLabel')
  label:setText(mana .. ' / ' .. maxMana)

  local manaBar = healthManaPanel:getChildById('manaBar')

  local percent
  if maxMana == 0 then
    percent = 100
  else
    percent = mana / maxMana * 100
  end
  manaBar:setPercent(percent)
end

connect(Game, { onLogin = HealthBar.create,
                onLogout = HealthBar.destroy,
                onHealthChange = HealthBar.onHealthChange,
                onManaChange = HealthBar.onManaChange })
