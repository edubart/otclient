Skills = {}

-- private variables
local skillsWindow
local skillsButton

-- private functions
local function setSkillValue(id, value)
  local skill = skillsWindow:recursiveGetChildById(id)
  local widget = skill:getChildById('value')
  widget:setText(value)
end

local function setSkillPercent(id, percent, tooltip)
  local skill = skillsWindow:recursiveGetChildById(id)
  local widget = skill:getChildById('percent')
  widget:setPercent(percent)

  if tooltip then
    widget:setTooltip(tooltip)
  end
end

-- public functions
function Skills.init()
  connect(LocalPlayer, {
    onExperienceChange = Skills.onExperienceChange,
    onLevelChange = Skills.onLevelChange,
    onHealthChange = Skills.onHealthChange,
    onManaChange = Skills.onManaChange,
    onSoulChange = Skills.onSoulChange,
    onFreeCapacityChange = Skills.onFreeCapacityChange,
    onStaminaChange = Skills.onStaminaChange,
    onMagicLevelChange = Skills.onMagicLevelChange,
    onSkillChange = Skills.onSkillChange
  })

  skillsWindow = g_ui.loadUI('skills.otui', GameInterface.getRightPanel())
  skillsButton = TopMenu.addRightGameToggleButton('skillsButton', tr('Skills') .. ' (Ctrl+S)', 'skills.png', Skills.toggle)
  skillsButton:setOn(true)
  g_keyboard.bindKeyDown('Ctrl+S', Skills.toggle)

  Skills.refresh()
end

function Skills.terminate()
  disconnect(LocalPlayer, {
    onExperienceChange = Skills.onExperienceChange,
    onLevelChange = Skills.onLevelChange,
    onHealthChange = Skills.onHealthChange,
    onManaChange = Skills.onManaChange,
    onSoulChange = Skills.onSoulChange,
    onFreeCapacityChange = Skills.onFreeCapacityChange,
    onStaminaChange = Skills.onStaminaChange,
    onMagicLevelChange = Skills.onMagicLevelChange,
    onSkillChange = Skills.onSkillChange
  })

  g_keyboard.unbindKeyDown('Ctrl+S')
  skillsButton:destroy()
  skillsButton = nil
  skillsWindow:destroy()
  skillsWindow = nil

  Skills = nil
end

function Skills.refresh()
  local player = g_game.getLocalPlayer()
  if not player then return end

  Skills.onExperienceChange(player, player:getExperience())
  Skills.onLevelChange(player, player:getLevel(), player:getLevelPercent())
  Skills.onHealthChange(player, player:getHealth(), player:getMaxHealth())
  Skills.onManaChange(player, player:getMana(), player:getMaxMana())
  Skills.onSoulChange(player, player:getSoul())
  Skills.onFreeCapacityChange(player, player:getFreeCapacity())
  Skills.onStaminaChange(player, player:getStamina())
  Skills.onMagicLevelChange(player, player:getMagicLevel(), player:getMagicLevelPercent())

  for i=0,6 do
    Skills.onSkillChange(player, i, player:getSkillLevel(i), player:getSkillLevelPercent(i))
  end
end

function Skills.toggle()
  if skillsButton:isOn() then
    skillsWindow:close()
    skillsButton:setOn(false)
  else
    skillsWindow:open()
    skillsButton:setOn(true)
  end
end

function Skills.onMiniWindowClose()
  skillsButton:setOn(false)
end

function Skills.onSkillButtonClick(button)
  local percentBar = button:getChildById('percent')
  if percentBar then
    percentBar:setVisible(not percentBar:isVisible())
    if percentBar:isVisible() then
      button:setHeight(21)
    else
      button:setHeight(21 - 6)
    end
  end
end

-- hooked events
function Skills.onExperienceChange(localPlayer, value)
  setSkillValue('experience', tr(value))
end

function Skills.onLevelChange(localPlayer, value, percent)
  setSkillValue('level', tr(value))
  setSkillPercent('level', percent, tr('You have %s percent to go', 100 - percent))
end

function Skills.onHealthChange(localPlayer, health, maxHealth)
  setSkillValue('health', tr(health))
end

function Skills.onManaChange(localPlayer, mana, maxMana)
  setSkillValue('mana', tr(mana))
end

function Skills.onSoulChange(localPlayer, soul)
  setSkillValue('soul', soul)
end

function Skills.onFreeCapacityChange(localPlayer, freeCapacity)
  setSkillValue('capacity', freeCapacity)
end

function Skills.onStaminaChange(localPlayer, stamina)
  local hours = math.floor(stamina / 60)
  local minutes = stamina % 60
  if minutes < 10 then
    minutes = '0' .. minutes
  end
  local percent = 100 * stamina / (42 * 60) -- max is 42 hours

  setSkillValue('stamina', hours .. ":" .. minutes)
  setSkillPercent('stamina', percent, tr('You have %s percent', percent))
end

function Skills.onMagicLevelChange(localPlayer, value, percent)
  setSkillValue('magiclevel', value)
  setSkillPercent('magiclevel', percent, tr('You have %s percent to go', 100 - percent))
end

function Skills.onSkillChange(localPlayer, id, level, percent)
  setSkillValue('skillId' .. id, level)
  setSkillPercent('skillId' .. id, percent, tr('You have %s percent to go', 100 - percent))
end

