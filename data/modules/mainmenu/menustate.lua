-- menu state
function onEnterMenuState()
    mainMenu = loadUI("modules/mainmenu/mainmenu.yml")
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
    setOnApplicationClose(onApplicationClose)
    initialStateLoaded = true
end