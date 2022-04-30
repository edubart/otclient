EventController = {
    events = nil,
    connected = false
}

function EventController:new(actor, events)
    local obj = {
        actor = actor,
        events = events,
        connected = false
    }
    setmetatable(obj, self)
    self.__index = self
    return obj
end

function EventController:connect()
    if self.connected then return end
    self.connected = true
    connect(self.actor, self.events)
end

function EventController:disconnect()
    if not self.connected then return end
    self.connected = false
    disconnect(self.actor, self.events)
end

function EventController:execute(name, ...) self.events[name](...) end
