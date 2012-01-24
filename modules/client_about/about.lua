About = {}

-- private variables
local aboutButton

-- public functions
function About.init()
  aboutButton = TopMenu.addRightButton('aboutButton', 'About', 'about.png', About.display)
end

function About.display()
  aboutWindow = displayUI('about.otui', { locked = true })
end

function About.terminate()
  aboutButton:destroy()
  aboutButton = nil
end

function About.openWebpage()
  displayErrorBox("Error", "Not implemented yet")
end
