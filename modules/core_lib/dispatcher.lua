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

function removeEvent(event)
  if event then
    event:cancel()
  end
end
