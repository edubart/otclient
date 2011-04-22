-- menu state
function onEnterMenuState()
    MainMenu_create()
end

function onLeaveMenuState()
    MainMenu_destroy()
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