-- main menu methods
function MainMenu_create()
    mainMenu = loadUI("modules/mainmenu/mainmenu.yml")
end

function MainMenu_destroy()
    mainMenu:destroy()
end

function MainMenu_enterGameClicked()
    enterGameWindow = loadUI("modules/mainmenu/entergamewindow.yml")
    button = enterGameWindow:getChildByID("okButton")
end

function MainMenu_optionsClicked()
    optionsWindow = loadUI("modules/mainmenu/optionswindow.yml")
end

function MainMenu_infoClicked()
    infoWindow = loadUI("modules/mainmenu/infowindow.yml")
end

function MainMenu_exitClicked()
    onApplicationClose()
end
