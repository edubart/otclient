-- events
function MainMenu_enterGameClicked()
    enterGameWindow = loadUI("modules/mainmenu/entergamewindow.yml")
end

function MainMenu_optionsClicked()
    optionsWindow = loadUI("modules/mainmenu/optionswindow.yml")
end

function MainMenu_infoClicked()
    infoWindow = loadUI("modules/mainmenu/infowindow.yml")
end

function MainMenu_exitClicked()
    exitGame()
end

-- create main menu
function MainMenu_create()
    menuPanel = loadUI("modules/mainmenu/mainmenu.yml")
end

MainMenu_create()