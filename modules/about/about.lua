About = {}

-- private variables
local about

-- public functions
function About.create()
  about = UI.loadAndDisplay("/about/about.otui")
  UI.root:lockChild(about)
end

function About.destroy()
  about:destroy()
  about = nil
end

function About.openWebpage()
  displayErrorBox("Error", "Not implemented yet")
end