-- menu state
function onEnterMenuState()
    mainMenu = UI.load("modules/mainmenu/mainmenu.yml")
end

function onLeaveMenuState()
    mainMenu:destroy()
end

function onApplicationClose()
    onLeaveMenuState()
    exitGame()
end

-- here is where everything starts
if not initialStateLoaded then
    onEnterMenuState()
    App.setOnClose(onApplicationClose)
    initialStateLoaded = true
end
