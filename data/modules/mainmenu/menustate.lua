-- menu state
function onEnterMenuState()
    mainMenu = UI.load("mainmenu.yml")
end

function onLeaveMenuState()

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
