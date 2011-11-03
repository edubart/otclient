Options = {}

-- private variables
local options

-- public functions
function Options.create()
  options = UI.loadAndDisplay("/options/options.otui")
  UI.root:lockChild(options)
end

function Options.destroy()
  options:destroy()
  options = nil
end

function Options.openWebpage()
  displayErrorBox("Error", "Not implemented yet")
end