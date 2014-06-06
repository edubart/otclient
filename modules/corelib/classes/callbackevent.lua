--[[
  @Authors: Ben Dol (BeniS)
  @Details: CallbackEvent class shell for callback events
]]

CallbackEvent = newclass("CallbackEvent")

CallbackEvent.create = function(id, callback)
  local event = CallbackEvent.internalCreate()
  
  event.id = id
  event.callback = callback

  return event
end

-- gets/sets

--@RequiredBy:Queue
function CallbackEvent:getId()
  return self.id
end

--@RequiredBy:Queue
function CallbackEvent:setId(id)
  self.id = id
end

--@RequiredBy:Queue
function CallbackEvent:getCallback()
  return self.callback
end

--@RequiredBy:Queue
function CallbackEvent:setCallback(callback)
  self.callback = callback
end

-- logic

--@RequiredBy:Queue
function CallbackEvent:start()
  -- Do nothing by default
end
