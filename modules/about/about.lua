About = {}

-- private variables
local about

-- public functions
function About.create()
  about = UI.display('about.otui', { locked = true })
end

function About.destroy()
  about:destroy()
  about = nil
end

function About.openWebpage()
  displayErrorBox("Error", "Not implemented yet")
end
