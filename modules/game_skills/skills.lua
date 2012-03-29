Skills = {}

-- private variables
local skillsWindow
local skillsButton

-- private functions
local function getNumberString(number)
  local out = ''
  number = tostring(number):reverse()
  for i=1,#number do
    out = out .. number:sub(i, i)
    if i % 3 == 0 and i ~= #number then
      out = out .. ','
    end
  end
  out = out:reverse()
  return out
end

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

  skillsWindow = displayUI('skills.otui', GameInterface.getLeftPanel())
  skillsButton = TopMenu.addGameToggleButton('skillsButton', 'Skills (Ctrl+S)', 'skills.png', Skills.toggle)
  skillsButton:setOn(true)
  Keyboard.bindKeyDown('Ctrl+S', Skills.toggle)
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

  Keyboard.unbindKeyDown('Ctrl+S')
  skillsButton:destroy()
  skillsButton = nil
  skillsWindow:destroy()
  skillsWindow = nil
end

function Skills.toggle()
  local visible = not skillsWindow:isExplicitlyVisible()
  skillsWindow:setVisible(visible)
  skillsButton:setOn(visible)
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
  setSkillValue('experience', getNumberString(value))
end

function Skills.onLevelChange(localPlayer, value, percent)
  setSkillValue('level', getNumberString(value))
  setSkillPercent('level', percent, 'You have ' .. (100 - percent) .. ' percent to go')
end

function Skills.onHealthChange(localPlayer, health, maxHealth)
  setSkillValue('health', getNumberString(health))
end

function Skills.onManaChange(localPlayer, mana, maxMana)
  setSkillValue('mana', getNumberString(mana))
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
  setSkillPercent('stamina', percent, 'You have ' .. percent .. ' percent')
end

function Skills.onMagicLevelChange(localPlayer, value, percent)
  setSkillValue('magiclevel', value)
  setSkillPercent('magiclevel', percent, 'You have ' .. (100 - percent) .. ' percent to go')
end

function Skills.onSkillChange(localPlayer, id, level, percent)
  setSkillValue('skillId' .. id, level)
  setSkillPercent('skillId' .. id, percent, 'You have ' .. (100 - percent) .. ' percent to go')
end

