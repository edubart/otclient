local eventId = 1
local events = { }
local orig = { scheduleEvent = scheduleEvent,
               addEvent = addEvent }

-- fix original scheduleEvent
function scheduleEvent(func, delay)
    eventId = eventId + 1
    local id = eventId + 1
    local function proxyFunc()
        func()
        table[id] = nil
    end
    table[id] = proxyFunc
    orig.scheduleEvent(proxyFunc, delay)
end

-- fix original addEvent
function addEvent(func)
    eventId = eventId + 1
    local id = eventId + 1
    local function proxyFunc()
        func()
        table[id] = nil
    end
    table[id] = proxyFunc
    orig.addEvent(proxyFunc)
end