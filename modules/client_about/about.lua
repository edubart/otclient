About = {}

-- private variables
local aboutWindow

-- public functions
function About.create()
  aboutWindow = displayUI('about.otui', { locked = true })
end

function About.destroy()
  aboutWindow:destroy()
  aboutWindow = nil
end

function About.openWebpage()
  displayErrorBox("Error", "Not implemented yet")
end
