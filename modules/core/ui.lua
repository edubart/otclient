UI = { }
UI.root = getRootWidget()

function UI.loadAndDisplayLocked(otuiFile)
  local widget = loadUI(otuiFile, UI.root)
  UI.root:lockChild(widget)
  return widget
end

function UI.loadAndDisplay(otuiFile)
  local widget = loadUI(otuiFile, UI.root)
  return widget
end

function UI.display(widget)
  UI.root:addChild(widget)
end

function UI.displayLocked(widget)
  UI.root:addChild(widget)
  UI.root:lockChild(widget)
end
