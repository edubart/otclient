PlayerDeath = {}

-- private variables
local deathWindow

-- private functions

-- public functions
function PlayerDeath.init()
  g_ui.importStyle('deathwindow.otui')
  
  connect(g_game, { onDeath = PlayerDeath.display,
                    onGameEnd = PlayerDeath.reset })
end

function PlayerDeath.terminate()
  disconnect(g_game, { onDeath = PlayerDeath.display,
                       onGameEnd = PlayerDeath.reset })

  PlayerDeath.reset()
  PlayerDeath = nil
end

function PlayerDeath.reset()
  GameInterface.getMapPanel():recursiveGetChildById('centerAdvance'):hide()
  if deathWindow then
    deathWindow:destroy()
    deathWindow = nil
  end
end

function PlayerDeath.display()
  PlayerDeath.displayDeadMessage()
  PlayerDeath.openWindow()
end

function PlayerDeath.displayDeadMessage()
  local advanceLabel = GameInterface.getMapPanel():recursiveGetChildById('centerAdvance')
  if advanceLabel:isVisible() then
    return 
  end
  
  TextMessage.displayEventAdvance(tr('You are dead.'))
end

function PlayerDeath.openWindow()
  if deathWindow then
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
    GameInterface.logout()
    cancelButton:getParent():destroy()
    deathWindow = nil
  end

  deathWindow.onEnter = okFunc
  deathWindow.onEscape = cancelFunc
  
  okButton.onClick = okFunc
  cancelButton.onClick = cancelFunc
end