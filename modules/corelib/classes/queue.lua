--[[
  @Authors: Ben Dol (BeniS)
  @Details: Queue class for event queuing.
]]

Queue = {}
Queue.__index = Queue

Queue.__class = "Queue"

Queue.new = function(callback)
  que = {
    queue = {},
    callback = nil
  }

  que.callback = callback

  setmetatable(que, Queue)
  return que
end

-- gets/sets

function Queue:getQueue()
  return self.queue
end

function Queue:setQueue(queue)
  self.queue = queue
end

function Queue:getCallback()
  return self.callback
end

function Queue:setCallback(callback)
  self.callback = callback
end

-- logic

function Queue:add(item)
  table.insert(self.queue, item)
end

function Queue:start()
  local compiled = function() self:finished() end
  for k,event in pairs(self.queue) do
    print("Compiling " .. tostring(event:getId()))
    local snip = compiled
    compiled = function()
      local realCallback = event:getCallback()
      event:setCallback(function()
        print("Event compiled callback called")
        realCallback()
        -- next queue item
        snip()
      end)
      print("Event started from queue chain")
      event:start()
    end
  end
  -- Start the queue chain
  print("Start compiled chain")
  compiled()
end

function Queue:finished()
  print("Queue:finished")
  local done = function()
    local callback = self:getCallback()
    if callback then callback() end
  end
  done()
end