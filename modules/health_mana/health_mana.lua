HealthMana = {}

-- private variables
local healthManaPanel = nil

-- public functions
function HealthMana.create()
  healthManaPanel = loadUI("/health_mana/health_mana.otui", Game.gameRightPanel)

  local healthLabel = UILabel.create()
  healthManaPanel:addChild(healthLabel)
  healthLabel:setId('healthLabel')
  healthLabel:setStyle('HealthLabel')
  healthLabel:setText('0 / 0')

  local manaLabel = UILabel.create()
  healthManaPanel:addChild(manaLabel)
  manaLabel:setId('manaLabel')
  manaLabel:setStyle('ManaLabel')
  manaLabel:setText('0 / 0')

  healthManaPanel:setHeight(healthLabel:getHeight() + manaLabel:getHeight())
end

function HealthMana.destroy()
  healthManaPanel:destroy()
  healthManaPanel = nil
end

-- hooked events
function Game.onHealthChange(health, maxHealth)
  local label = healthManaPanel:getChildById('healthLabel')
  label:setText(health .. ' / ' .. maxHealth)
end

function Game.onManaChange(mana, maxMana)
  local label = healthManaPanel:getChildById('manaLabel')
  label:setText(mana .. ' / ' .. maxMana)
end

connect(Game, { onLogin = HealthMana.create,
                onLogout = HealthMana.destroy })
