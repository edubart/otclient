-- globals
rootWidget = g_ui.getRootWidget()

-- public functions
function importStyle(otui)
  g_ui.importStyle(resolveFileFullPath(otui, 2))
end

function displayUI(arg1, options)
  local widget
  local parent
  if options then parent = options.parent end
  parent = parent or rootWidget

  -- display otui files
  if type(arg1) == 'string' then
    local otuiFilePath = resolveFileFullPath(arg1, 2)
    widget = g_ui.loadUI(otuiFilePath, parent)
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

function createWidget(style, parent)
  local className = g_ui.getStyleClass(style)
  local class = _G[className]
  local widget = class.create()
  if parent then
    parent:addChild(widget)
  end
  widget:setStyle(style)
  return widget
end
