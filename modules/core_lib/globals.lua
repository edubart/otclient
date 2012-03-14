rootWidget = g_ui.getRootWidget()

importStyle = g_ui.importStyle
importFont = g_fonts.importFont
setDefaultFont = g_fonts.setDefaultFont

loadUI = g_ui.loadUI

function displayUI(otui, parent)
  parent = parent or rootWidget
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
    error('could not find widget class ' .. className)
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

function scheduleEvent(callback, delay)
  local event = g_eventDispatcher.scheduleEvent(callback, delay)

  -- must hold a reference to the callback, otherwise it would be collected
  event._callback = callback
  return event
end

function addEvent(callback, front)
  local event = g_eventDispatcher.addEvent(callback, front)
  -- must hold a reference to the callback, otherwise it would be collected
  event._callback = callback
  return event
end

function removeEvent(event)
  if event then
    event:cancel()
    event._callback = nil
  end
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


