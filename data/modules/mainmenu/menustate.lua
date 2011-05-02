-- menu state
function onEnterMenuState()
    mainMenu = UI.load("modules/mainmenu/mainmenu.yml")
end

function onLeaveMenuState()
    mainMenu:destroy()
    mainMenu = nil
end

function onApplicationClose()
    onLeaveMenuState()
    App.exit()
end

-- here is where everything starts
if not initialStateLoaded then
    onEnterMenuState()
    App.onClose = onApplicationClose
    initialStateLoaded = true
end
