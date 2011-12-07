local eventId = 0
local eventList = {}

function scheduleEvent(func, delay)
  eventId = eventId + 1
  local id = eventId
  local function proxyFunc()
    if eventList[id] then
      if eventList[id].active then
        func()
      end
      eventList[id] = nil
    end
  end
  eventList[id] = { func = proxyFunc, active = true }
  if delay and delay > 0 then
    g_dispatcher.scheduleEvent(proxyFunc, delay)
  else
    g_dispatcher.addEvent(proxyFunc, false)
  end
  return id
end

function addEvent(func)
  return scheduleEvent(func, 0)
end

function removeEvent(id)
  if id and eventList[id] then
    eventList[id].active = false
    return true
  end
end
