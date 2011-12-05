HealthBar = {}

-- private variables
local healthManaPanel = nil

-- public functions
function HealthBar.create()
  healthManaPanel = UI.display('healthbar.otui', { parent = Game.gameRightPanel })

  local healthBar = UIProgressBar.create()
  healthManaPanel:addChild(healthBar)
  healthBar:setId('healthBar')
  healthBar:setStyle('HealthBar')

  local healthLabel = UILabel.create()
  healthManaPanel:addChild(healthLabel)
  healthLabel:setId('healthLabel')
  healthLabel:setStyle('HealthLabel')
  healthLabel:setText('0 / 0')

  local manaBar = UIProgressBar.create()
  healthManaPanel:addChild(manaBar)
  manaBar:setId('manaBar')
  manaBar:setStyle('ManaBar')

  local manaLabel = UILabel.create()
  healthManaPanel:addChild(manaLabel)
  manaLabel:setId('manaLabel')
  manaLabel:setStyle('ManaLabel')
  manaLabel:setText('0 / 0')

  healthManaPanel:setHeight(healthBar:getHeight() + manaBar:getHeight() + 4)
end

function HealthBar.destroy()
  healthManaPanel:destroy()
  healthManaPanel = nil
end

-- hooked events
function Game.onHealthChange(health, maxHealth)
  local label = healthManaPanel:getChildById('healthLabel')
  label:setText(health .. ' / ' .. maxHealth)

  local healthBar = healthManaPanel:getChildById('healthBar')
  healthBar:setPercent(health / maxHealth * 100)
end

function Game.onManaChange(mana, maxMana)
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
                onLogout = HealthBar.destroy })
