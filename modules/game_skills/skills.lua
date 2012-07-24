skillsWindow = nil
skillsButton = nil

function init()
  connect(LocalPlayer, {
    onExperienceChange = onExperienceChange,
    onLevelChange = onLevelChange,
    onHealthChange = onHealthChange,
    onManaChange = onManaChange,
    onSoulChange = onSoulChange,
    onFreeCapacityChange = onFreeCapacityChange,
    onStaminaChange = onStaminaChange,
    onMagicLevelChange = onMagicLevelChange,
    onSkillChange = onSkillChange
  })

  skillsWindow = g_ui.loadUI('skills.otui', modules.game_interface.getRightPanel())
  skillsButton = TopMenu.addRightGameToggleButton('skillsButton', tr('Skills') .. ' (Ctrl+S)', 'skills.png', toggle)
  skillsButton:setOn(true)
  g_keyboard.bindKeyDown('Ctrl+S', toggle)

  refresh()
end

function terminate()
  disconnect(LocalPlayer, {
    onExperienceChange = onExperienceChange,
    onLevelChange = onLevelChange,
    onHealthChange = onHealthChange,
    onManaChange = onManaChange,
    onSoulChange = onSoulChange,
    onFreeCapacityChange = onFreeCapacityChange,
    onStaminaChange = onStaminaChange,
    onMagicLevelChange = onMagicLevelChange,
    onSkillChange = onSkillChange
  })

  g_keyboard.unbindKeyDown('Ctrl+S')
  skillsButton:destroy()
  skillsWindow:destroy()
end

function setSkillValue(id, value)
  local skill = skillsWindow:recursiveGetChildById(id)
  local widget = skill:getChildById('value')
  widget:setText(value)
end

function setSkillPercent(id, percent, tooltip)
  local skill = skillsWindow:recursiveGetChildById(id)
  local widget = skill:getChildById('percent')
  widget:setPercent(percent)

  if tooltip then
    widget:setTooltip(tooltip)
  end
end

function refresh()
  local player = g_game.getLocalPlayer()
  if not player then return end

  onExperienceChange(player, player:getExperience())
  onLevelChange(player, player:getLevel(), player:getLevelPercent())
  onHealthChange(player, player:getHealth(), player:getMaxHealth())
  onManaChange(player, player:getMana(), player:getMaxMana())
  onSoulChange(player, player:getSoul())
  onFreeCapacityChange(player, player:getFreeCapacity())
  onStaminaChange(player, player:getStamina())
  onMagicLevelChange(player, player:getMagicLevel(), player:getMagicLevelPercent())

  for i=0,6 do
    onSkillChange(player, i, player:getSkillLevel(i), player:getSkillLevelPercent(i))
  end
end

function toggle()
  if skillsButton:isOn() then
    skillsWindow:close()
    skillsButton:setOn(false)
  else
    skillsWindow:open()
    skillsButton:setOn(true)
  end
end

function onMiniWindowClose()
  skillsButton:setOn(false)
end

function onSkillButtonClick(button)
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

function onExperienceChange(localPlayer, value)
  setSkillValue('experience', tr(value))
end

function onLevelChange(localPlayer, value, percent)
  setSkillValue('level', tr(value))
  setSkillPercent('level', percent, tr('You have %s percent to go', 100 - percent))
end

function onHealthChange(localPlayer, health, maxHealth)
  setSkillValue('health', tr(health))
end

function onManaChange(localPlayer, mana, maxMana)
  setSkillValue('mana', tr(mana))
end

function onSoulChange(localPlayer, soul)
  setSkillValue('soul', soul)
end

function onFreeCapacityChange(localPlayer, freeCapacity)
  setSkillValue('capacity', freeCapacity)
end

function onStaminaChange(localPlayer, stamina)
  local hours = math.floor(stamina / 60)
  local minutes = stamina % 60
  if minutes < 10 then
    minutes = '0' .. minutes
  end
  local percent = 100 * stamina / (42 * 60) -- max is 42 hours

  setSkillValue('stamina', hours .. ":" .. minutes)
  setSkillPercent('stamina', percent, tr('You have %s percent', percent))
end

function onMagicLevelChange(localPlayer, value, percent)
  setSkillValue('magiclevel', value)
  setSkillPercent('magiclevel', percent, tr('You have %s percent to go', 100 - percent))
end

function onSkillChange(localPlayer, id, level, percent)
  setSkillValue('skillId' .. id, level)
  setSkillPercent('skillId' .. id, percent, tr('You have %s percent to go', 100 - percent))
end

