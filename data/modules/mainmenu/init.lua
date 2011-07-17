require 'entergame'

function initializeApplication()
  mainMenu = loadUI("ui/mainmenu")
end

function terminateApplication()
  App.exit()
end

-- here is where everything starts
if not initialized then
  initializeApplication()
  App.onClose = terminateApplication
  initialized = true
end
