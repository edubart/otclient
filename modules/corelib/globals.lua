rootWidget = g_ui.getRootWidget()

importStyle = g_ui.importStyle
importFont = g_fonts.importFont
setDefaultFont = g_fonts.setDefaultFont

-- G is used as a global table to save variables in memory between reloads
G = G or {}

function loadUI(otui, parent)
  local otuiFilePath = resolvepath(otui, 2)
  return g_ui.loadUI(otuiFilePath, parent)
end

function displayUI(otui, parent)
  parent = parent or rootWidget
  local otuiFilePath = resolvepath(otui, 2)
  return g_ui.loadUI(otuiFilePath, parent)
end

function createWidget(stylename, parent)
  if type(parent) == 'string' then
    parent = rootWidget:recursiveGetChildById(parent)
  end
  local widget = g_ui.createWidgetFromStyle(stylename, parent)
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

function cycleEvent(callback, front)
  local event = g_eventDispatcher.cycleEvent(callback, front)
  -- must hold a reference to the callback, otherwise it would be collected
  event._callback = callback
  return event
end


function periodicalEvent(eventFunc, conditionFunc, delay, autoRepeatDelay)
  delay = delay or 30
  autoRepeatDelay = autoRepeatDelay or delay

  local func
  func = function()
    if conditionFunc and not conditionFunc() then
      func = nil
      return
    end
    eventFunc()
    scheduleEvent(func, delay)
  end

  scheduleEvent(function()
    func()
  end, autoRepeatDelay)
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
