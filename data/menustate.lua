MainMenuController = {}
enterGameController = {}

function enterGameController.enterGameButton_clicked()
    createEnterGameWindow()
end

function mainMenuController.createMainMenu()
    mainMenuPanel = loadUI("ui/mainMenuPanel.yml")
    enterGameButton = mainMenuPanel:getElementById("enterGameButton")
    enterGameButton.setOnClick(enterGameController.enterGameButton_clicked)
    exitButton = mainMenuPanel:getElementById("exitButton")
    exitButton:setOnClick(function() exitGame() end)
end

function onEnter()
    mainMenuController:createMainMenu()
end

function onLeave()

end
