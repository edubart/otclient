cooldownPanel = nil
spellCooldownPanel = nil

function init()
  connect(g_game, { onGameStart = show,
                    onGameEnd = hide,
                    onSpellGroupCooldown = onSpellGroupCooldown,
                    onSpellCooldown = onSpellCooldown })

  cooldownPanel = g_ui.displayUI('cooldown.otui')
  cooldownPanel:hide()
  
  spellCooldownPanel = cooldownPanel:getChildById('spellCooldownPanel')

  if g_game.isOnline() then
    show()
  end
end

function terminate()
  disconnect(g_game, { onGameStart = show,
                       onGameEnd = hide,
                       onSpellGroupCooldown = onSpellGroupCooldown,
                       onSpellCooldown = onSpellCooldown })

  spellCooldownPanel:destroy()
  cooldownPanel:destroy()
end

function show()
  if g_game.getFeature(GameSpellList) then
    cooldownPanel:show()
  end
end

function hide()
  cooldownPanel:hide()
end

function updateProgressRect(progressRect, interval, init)
  if init then
    progressRect:setPercent(0)
  else
    progressRect:setPercent(progressRect:getPercent() + 4)
  end
  
  if progressRect:getPercent() < 100 then
     removeEvent(progressRect.event)
     progressRect.event = scheduleEvent(function() updateProgressRect(progressRect, interval) end, interval)
  end
end

function onSpellCooldown(iconId, duration)
  local spellName = SpelllistSettings[modules.game_spelllist.getSpelllistProfile()].spellIcons[iconId]
  if not spellName then return end
  
  local otcIconId = tonumber(SpellInfo[modules.game_spelllist.getSpelllistProfile()][spellName].icon)
  if not otcIconId and SpellIcons[SpellInfo[modules.game_spelllist.getSpelllistProfile()][spellName].icon] then
    otcIconId = SpellIcons[SpellInfo[modules.game_spelllist.getSpelllistProfile()][spellName].icon][1]
  end
  
  if not otcIconId then return end
  
  local icon = cooldownPanel:getChildById(spellName)
  if not icon then
    icon = g_ui.createWidget('SpellIcon', spellCooldownPanel)
    icon:setId(spellName)
    icon:setImageSource('/game_cooldown/icons/' .. SpelllistSettings[modules.game_spelllist.getSpelllistProfile()].iconFile)
    icon:setImageClip(modules.game_spelllist.getIconImageClip(otcIconId))
    icon.event = scheduleEvent(function() icon:destroy() end, duration)
  
    local progressRect = g_ui.createWidget('SpellProgressRect', icon)
    updateProgressRect(progressRect, duration/25, true)
    progressRect:setTooltip(spellName)
  end
end

function onSpellGroupCooldown(groupId, duration)
  if not SpellGroups[groupId] then return end
  
  local icon = cooldownPanel:getChildById('groupIcon' .. SpellGroups[groupId])
  local progressRect = cooldownPanel:getChildById('progressRect' .. SpellGroups[groupId])
  if icon then
    icon:setOn(true)
    removeEvent(icon.event)
    icon.event = scheduleEvent(function() icon:setOn(false) end, duration)
  end
  
  if progressRect then
    removeEvent(progressRect.event)
    updateProgressRect(progressRect, duration/25, true)
  end
end
