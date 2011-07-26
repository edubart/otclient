require 'entergame'

function initializeApplication()
  mainMenu = loadUI("ui/mainmenu", rootUI)
end

function terminateApplication()
  exit()
end

-- here is where everything starts
if not initialized then
  initializeApplication()
  onClose = terminateApplication
  initialized = true
end
