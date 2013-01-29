local ProgressCallback = {
  update = 1,
  finish = 2
}

cooldownWindow = nil
cooldownButton = nil
contentsPanel = nil
spellCooldownPanel = nil

function init()
  connect(g_game, { onGameStart = online,
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

function removeCooldown(progressRect)
  removeEvent(progressRect.event)
  if progressRect.icon then
    progressRect.icon:destroy()
    progressRect.icon = nil
  end
  progressRect = nil
end

function turnOffCooldown(progressRect)
  removeEvent(progressRect.event)
  if progressRect.icon then
    progressRect.icon:setOn(false)
    progressRect.icon = nil
  end
  
  -- create particles
  local particle = g_ui.createWidget('GroupCooldownParticles', progressRect)
  particle:fill('parent')

  progressRect = nil
end

function initCooldown(progressRect, updateCallback, finishCallback)
  progressRect:setPercent(0)

  progressRect.callback = {}
  progressRect.callback[ProgressCallback.update] = updateCallback
  progressRect.callback[ProgressCallback.finish] = finishCallback

  updateCallback()
end

function updateCooldown(progressRect, interval)
  progressRect:setPercent(progressRect:getPercent() + 5)
  
  if progressRect:getPercent() < 100 then
    removeEvent(progressRect.event)
    progressRect.event = scheduleEvent(function() progressRect.callback[ProgressCallback.update]() end, interval)
  else
    progressRect.callback[ProgressCallback.finish]()
  end
end

function onSpellCooldown(iconId, duration)
  local spell, profile, spellName = Spells.getSpellByIcon(iconId)
  if not spellName then return end
  
  --local ping = g_game.getPing()
  --if ping > 0 then duration = duration + ping end
  
  clientIconId = Spells.getClientId(spellName)
  if not clientIconId then return end
  
  local icon = spellCooldownPanel:getChildById(spellName)
  if not icon then
    icon = g_ui.createWidget('SpellIcon', spellCooldownPanel)
    icon:setId(spellName)
    icon:setImageSource('/images/game/spells/' .. SpelllistSettings[profile].iconFile)
    icon:setImageClip(Spells.getImageClip(clientIconId, profile))
  
    local progressRect = g_ui.createWidget('SpellProgressRect', icon)
    progressRect.icon = icon
    progressRect:fill('parent')
    progressRect:setTooltip(spellName)

    local updateFunc = function()
      updateCooldown(progressRect, duration/25)
    end
    local finishFunc = function()
      removeCooldown(progressRect)
    end
    initCooldown(progressRect, updateFunc, finishFunc)
  end
end

function onSpellGroupCooldown(groupId, duration)
  if not SpellGroups[groupId] then return end
  
  --local ping = g_game.getPing()
  --if ping > 0 then duration = duration + ping end

  local icon = contentsPanel:getChildById('groupIcon' .. SpellGroups[groupId])
  local progressRect = contentsPanel:getChildById('progressRect' .. SpellGroups[groupId])
  if icon then
    icon:setOn(true)
    removeEvent(icon.event)
  end

  progressRect.icon = icon
  if progressRect then
    removeEvent(progressRect.event)
    local updateFunc = function()
      updateCooldown(progressRect, duration/25)
    end
    local finishFunc = function()
      turnOffCooldown(progressRect)
    end
    initCooldown(progressRect, updateFunc, finishFunc)
  end
end
