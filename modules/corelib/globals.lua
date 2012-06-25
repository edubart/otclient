-- @docvars @{

-- root widget
rootWidget = g_ui.getRootWidget()

-- G is used as a global table to save variables in memory between reloads
G = G or {}

-- @}

-- @docfuncs @{

function scheduleEvent(callback, delay)
  local event = g_dispatcher.scheduleEvent(callback, delay)
  -- must hold a reference to the callback, otherwise it would be collected
  event._callback = callback
  return event
end

function addEvent(callback, front)
  local event = g_dispatcher.addEvent(callback, front)
  -- must hold a reference to the callback, otherwise it would be collected
  event._callback = callback
  return event
end

function cycleEvent(callback, front)
  local event = g_dispatcher.cycleEvent(callback, front)
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

-- @}