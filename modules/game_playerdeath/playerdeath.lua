deathWindow = nil

local deathTexts = {
  regular = {text = 'Alas! Brave adventurer, you have met a sad fate.\nBut do not despair, for the gods will bring you back\ninto this world in exchange for a small sacrifice\n\nSimply click on Ok to resume your journeys!', height = 140, width = 0},
  unfair = {text = 'Alas! Brave adventurer, you have met a sad fate.\nBut do not despair, for the gods will bring you back\ninto this world in exchange for a small sacrifice\n\nThis death penalty has been reduced by %i%%\nbecause it was an unfair fight.\n\nSimply click on Ok to resume your journeys!', height = 185, width = 0},
  blessed = {text = 'Alas! Brave adventurer, you have met a sad fate.\nBut do not despair, for the gods will bring you back into this world\n\nThis death penalty has been reduced by 100%\nbecause you are blessed with the Adventurer\'s Blessing\n\nSimply click on Ok to resume your journeys!', height = 170, width = 90}
}

function init()
  g_ui.importStyle('deathwindow')

  connect(g_game, { onDeath = display,
                    onGameEnd = reset })
end

function terminate()
  disconnect(g_game, { onDeath = display,
                       onGameEnd = reset })

  reset()
end

function reset()
  if deathWindow then
    deathWindow:destroy()
    deathWindow = nil
  end
end

function display(deathType, penalty)
  displayDeadMessage()
  openWindow(deathType, penalty)
end

function displayDeadMessage()
  local advanceLabel = modules.game_interface.getRootPanel():recursiveGetChildById('middleCenterLabel')
  if advanceLabel:isVisible() then return end

  modules.game_textmessage.displayGameMessage(tr('You are dead.'))
end

function openWindow(deathType, penalty)
  if deathWindow then
    deathWindow:destroy()
    return
  end

  deathWindow = g_ui.createWidget('DeathWindow', rootWidget)

  local textLabel = deathWindow:getChildById('labelText')
  if deathType == DeathType.Regular then
    if penalty == 100 then
      textLabel:setText(deathTexts.regular.text)
      deathWindow:setHeight(deathWindow.baseHeight + deathTexts.regular.height)
      deathWindow:setWidth(deathWindow.baseWidth + deathTexts.regular.width)
    else
      textLabel:setText(string.format(deathTexts.unfair.text, 100 - penalty))
      deathWindow:setHeight(deathWindow.baseHeight + deathTexts.unfair.height)
      deathWindow:setWidth(deathWindow.baseWidth + deathTexts.unfair.width)
    end
  elseif deathType == DeathType.Blessed then
    textLabel:setText(deathTexts.blessed.text)
    deathWindow:setHeight(deathWindow.baseHeight + deathTexts.blessed.height)
    deathWindow:setWidth(deathWindow.baseWidth + deathTexts.blessed.width)
  end

  local okButton = deathWindow:getChildById('buttonOk')
  local cancelButton = deathWindow:getChildById('buttonCancel')

  local okFunc = function()
    CharacterList.doLogin()
    okButton:getParent():destroy()
    deathWindow = nil
  end
  local cancelFunc = function()
    g_game.safeLogout()
    cancelButton:getParent():destroy()
    deathWindow = nil
  end

  deathWindow.onEnter = okFunc
  deathWindow.onEscape = cancelFunc

  okButton.onClick = okFunc
  cancelButton.onClick = cancelFunc
end
