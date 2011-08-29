function Game.createMainInterface()
  gameUi = loadUI('/game/ui/gameinterface.otui', UI.root)
end

function Game.onLogin()

end

function Game.onLogout()
  gameUi:destroy()
  gameUi = nil
  mainMenu:show()
end
