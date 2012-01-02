UI = { }
UI.root = getRootWidget()

-- public functions
function UI.display(arg1, options)
  local widget
  local parent
  if options then parent = options.parent end
  parent = parent or UI.root

  -- display otui files
  if type(arg1) == 'string' then
    local otuiFilePath = resolveFileFullPath(arg1, 2)
    widget = loadUI(otuiFilePath, parent)
  -- display already loaded widgets
  else
    widget = arg1
    if parent:hasChild(widget) then
      widget:focus()
      widget:show()
    else
      parent:addChild(widget)
      widget:show()
    end
  end

  -- apply display options
  if widget and options then
    for option,value in pairs(options) do
      if option == 'locked' and value then
        widget:lock()
      elseif option == 'visible' then
        widget:setVisible(value)
      elseif option == 'x' then
        widget:setX(value)
      elseif option == 'y' then
        widget:setY(value)
      end
    end
  end
  return widget
end
