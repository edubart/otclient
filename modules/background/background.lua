Background = { }

-- private variables
local background

-- public functions
function Background.create()
  background = UI.loadAndDisplay('/background/background.otui')
end

function Background.destroy()
  background:destroy()
  background = nil
end

function Background.hide()
  background:hide()
end

function Background.show()
  background:show()
end
