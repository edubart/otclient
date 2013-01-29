cooldownWindow = nil
cooldownButton = nil
contentsPanel = nil
spellCooldownPanel = nil

function init()
  connect(g_game, { onGameStart = online,
                    onGameEnd = offline,
                    onSpellGroupCooldown = onSpellGroupCooldown,
                    onSpellCooldown = onSpellCooldown })

  cooldownButton = modules.client_topmenu.addRightGameToggleButton('cooldownButton', tr('Cooldowns'), '/images/topbuttons/cooldowns', toggle)
  cooldownButton:setOn(true)
  cooldownButton:hide()

  cooldownWindow = g_ui.loadUI('cooldown', modules.game_interface.getRightPanel())
  cooldownWindow:disableResize()
  cooldownWindow:setup() 
  
  contentsPanel = cooldownWindow:getChildById('contentsPanel')
  spellCooldownPanel = contentsPanel:getChildById('spellCooldownPanel')

  if g_game.isOnline() then
    online()
  end
end

function terminate()
  disconnect(g_game, { onGameStart = online,
                       onGameEnd = offline,
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

function online()
  if g_game.getFeature(GameSpellList) then
    cooldownButton:show()
  else
    cooldownButton:hide()
    cooldownWindow:close()
  end
end

function offline()
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
  local spell, profile, spellName = Spells.getSpellByIcon(iconId)
  if not spellName then return end
  
  local ping = g_game.getPing()
  if ping > 0 then duration = duration - (ping/2) end
  
  clientIconId = Spells.getClientId(spellName)
  if not clientIconId then return end
  
  local icon = spellCooldownPanel:getChildById(spellName)
  if not icon then
    icon = g_ui.createWidget('SpellIcon', spellCooldownPanel)
    icon:setId(spellName)
    icon:setImageSource('/images/game/spells/' .. SpelllistSettings[profile].iconFile)
    icon:setImageClip(Spells.getImageClip(clientIconId, profile))
    icon.event = scheduleEvent(function() icon:destroy() icon = nil end, duration)
  
    local progressRect = g_ui.createWidget('SpellProgressRect', icon)
    progressRect:fill('parent')
    progressRect:setTooltip(spellName)
    updateProgressRect(progressRect, duration/25, true)
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
