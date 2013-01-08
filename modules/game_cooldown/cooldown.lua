cooldownWindow = nil
cooldownButton = nil
contentsPanel = nil
spellCooldownPanel = nil

function init()
  connect(g_game, { onGameStart = show,
                    onGameEnd = hide,
                    onSpellGroupCooldown = onSpellGroupCooldown,
                    onSpellCooldown = onSpellCooldown })

  cooldownButton = TopMenu.addRightGameToggleButton('cooldownButton', tr('Cooldowns'), 'cooldown.png', toggle)
  cooldownButton:setOn(true)
  cooldownButton:hide()

  cooldownWindow = g_ui.loadUI('cooldown.otui', modules.game_interface.getRightPanel())
  cooldownWindow:disableResize()
  cooldownWindow:setup() 
  
  contentsPanel = cooldownWindow:getChildById('contentsPanel')
  spellCooldownPanel = contentsPanel:getChildById('spellCooldownPanel')
  
  if g_game.isOnline() then
    show()
  end
end

function terminate()
  disconnect(g_game, { onGameStart = show,
                       onGameEnd = hide,
                       onSpellGroupCooldown = onSpellGroupCooldown,
                       onSpellCooldown = onSpellCooldown })

  cooldownButton:destroy()
  cooldownWindow:destroy()
end

function onMiniWindowClose()
  cooldownButton:setOn(false)
end

function toggle()
  if cooldownButton:isOn() then
    cooldownWindow:close()
    cooldownButton:setOn(false)
  else
    cooldownWindow:open()
    cooldownButton:setOn(true)
  end
end

function show()
  if g_game.getFeature(GameSpellList) then
    cooldownWindow:show()
    cooldownButton:show()
  end
end

function hide()
  cooldownWindow:hide()
  cooldownButton:hide()
end

function updateProgressRect(progressRect, interval, init)
  if init then
    progressRect:setPercent(0)
  else
    progressRect:setPercent(progressRect:getPercent() + 5)
  end
  
  if progressRect:getPercent() < 100 then
     removeEvent(progressRect.event)
     progressRect.event = scheduleEvent(function() updateProgressRect(progressRect, interval) end, interval)
  end
end

function onSpellCooldown(iconId, duration)
  local spellName = SpelllistSettings[modules.game_spelllist.getSpelllistProfile()].spellIcons[iconId]
  if not spellName then return end
  
  local ping = g_game.getPing()
  if ping > 0 then duration = duration - (ping/2) end
  local otcIconId = tonumber(SpellInfo[modules.game_spelllist.getSpelllistProfile()][spellName].icon)
  if not otcIconId and SpellIcons[SpellInfo[modules.game_spelllist.getSpelllistProfile()][spellName].icon] then
    otcIconId = SpellIcons[SpellInfo[modules.game_spelllist.getSpelllistProfile()][spellName].icon][1]
  end
  
  if not otcIconId then return end
  
  local icon = spellCooldownPanel:getChildById(spellName)
  if not icon then
    icon = g_ui.createWidget('SpellIcon', spellCooldownPanel)
    icon:setId(spellName)
    icon:setImageSource('/game_cooldown/icons/' .. SpelllistSettings[modules.game_spelllist.getSpelllistProfile()].iconFile)
    icon:setImageClip(modules.game_spelllist.getIconImageClip(otcIconId))
    icon.event = scheduleEvent(function() icon:destroy() icon = nil end, duration)
  
    local progressRect = g_ui.createWidget('SpellProgressRect')
    updateProgressRect(progressRect, duration/25, true)
    progressRect:setTooltip(spellName)
  end
end

function onSpellGroupCooldown(groupId, duration)
  if not SpellGroups[groupId] then return end
  
  local ping = g_game.getPing()
  if ping > 0 then duration = duration - (ping/2) end
  local icon = contentsPanel:getChildById('groupIcon' .. SpellGroups[groupId])
  local progressRect = contentsPanel:getChildById('progressRect' .. SpellGroups[groupId])
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
