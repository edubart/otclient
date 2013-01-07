Exit = {}

local exitWindow
local exitButton

function Exit.init()
  if not g_game.isOnline() then
    exitButton = TopMenu.addRightButton('exitButton', tr('Exit Client'), 'exit.png', Exit.tryExit)
  end

  connect(g_game, {
    onGameStart = Exit.hide,
    onGameEnd = Exit.show
  })
end

function Exit.terminate()
  disconnect(g_game, {
    onGameStart = Exit.hide,
    onGameEnd = Exit.show
  })

  if exitWindow then
    exitWindow:destroy()
    exitWindow = nil
  end
  
  if exitButton then
    exitButton:destroy()
    exitButton = nil
  end

  Exit = nil
end

function Exit.hide()
  if exitWindow then
    exitWindow:destroy()
  end
  exitButton:hide()
end

function Exit.show()
  exitButton:show()
end

function Exit.tryExit()
  if exitWindow then
    return true
  end

  local yesFunc = function() scheduleEvent(exit, 10) end
  local noFunc = function() exitWindow:destroy() exitWindow = nil end

  exitWindow = displayGeneralBox('Exit', tr("Do you really want to exit?"),
  { { text='Yes', callback=yesFunc },
    { text='No', callback=noFunc },
    anchor=AnchorHorizontalCenter }, yesFunc, noFunc)

  return true
end
