MainMenu = { }

-- private variables
local mainMenu

-- public functions
function MainMenu.create()
  mainMenu = UI.loadAndDisplay("/mainmenu/ui/mainmenu.otui")
end

function MainMenu.destroy()
  mainMenu:destroy()
  mainMenu = nil
end

function MainMenu.hide()
  mainMenu:hide()
end

function MainMenu.show()
  mainMenu:show()
end
