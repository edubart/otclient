rootWidget = g_ui.getRootWidget()

function importStyle(otui)
  g_ui.importStyle(resolvepath(otui, 2))
end

function importFont(otfont)
  g_fonts.importFont(resolvepath(otfont, 2))
end

function setDefaultFont(font)
  g_fonts.setDefaultFont(font)
end

function displayUI(arg1, options)
  local widget
  local parent
  if options then parent = options.parent end
  parent = parent or rootWidget

  -- display otui files
  if type(arg1) == 'string' then
    local otuiFilePath = resolvepath(arg1, 2)
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

function loadUI(otui, parent)
  local otuiFilePath = resolvepath(otui, 2)
  return g_ui.loadUI(otuiFilePath, parent)
end

function createWidget(style, parent)
  local className = g_ui.getStyleClass(style)
  if className == "" then
    error('could not find style ' .. style)
    return
  end

  local class = _G[className]
  if not class then
    error('could not find widget class ' .. class)
    return
  end

  local widget = class.create()
  if parent then
    if type(parent) == 'string' then
      parent = rootWidget:recursiveGetChildById(parent)
    end
    parent:addChild(widget)
  end
  widget:setStyle(style)
  return widget
end

function reloadModule(name)
  local module = g_modules.getModule(name)
  if module then
    module:reload()
  end
end

function reloadModules()
  g_modules.reloadModules()
end


