deathWindow = nil

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
  end
end

function display()
  displayDeadMessage()
  openWindow()
end

function displayDeadMessage()
  local advanceLabel = modules.game_interface.getRootPanel():recursiveGetChildById('middleCenterLabel')
  if advanceLabel:isVisible() then return end

  modules.game_textmessage.displayGameMessage(tr('You are dead.'))
end

function openWindow()
  if deathWindow then
    deathWindow:destroy()
    return
  end
  deathWindow = g_ui.createWidget('DeathWindow', rootWidget)
  local okButton = deathWindow:getChildById('buttonOk')
  local cancelButton = deathWindow:getChildById('buttonCancel')

  local okFunc = function()
    CharacterList.doLogin()
    okButton:getParent():destroy()
    deathWindow = nil
  end
  local cancelFunc = function()
    modules.game_interface.logout()
    cancelButton:getParent():destroy()
    deathWindow = nil
  end

  deathWindow.onEnter = okFunc
  deathWindow.onEscape = cancelFunc

  okButton.onClick = okFunc
  cancelButton.onClick = cancelFunc
end
