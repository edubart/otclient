About = {}

-- private variables
local aboutButton
local aboutWindow

-- public functions
function About.init()
  aboutButton = TopMenu.addRightButton('aboutButton', 'About', 'about.png', About.show)
  aboutWindow = displayUI('about.otui')
  aboutWindow:hide()
end

function About.terminate()
  aboutButton:destroy()
  aboutButton = nil
  aboutWindow:destroy()
  aboutWindow = nil
  About = nil
end

function About.show()
  aboutWindow:show()
  aboutWindow:raise()
  aboutWindow:focus()
end

function About.hide()
  aboutWindow:hide()
end

function About.openWebpage()
  displayErrorBox("Error", "Not implemented yet")
end
