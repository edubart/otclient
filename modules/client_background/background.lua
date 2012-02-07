Background = { }

-- private variables
local background

-- public functions
function Background.init()
  background = displayUI('background.otui')
  background:lower()
end

function Background.terminate()
  background:destroy()
  background = nil
  Background = nil
end

function Background.hide()
  background:hide()
end

function Background.show()
  background:show()
end
