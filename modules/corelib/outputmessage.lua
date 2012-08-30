function OutputMessage:addData(data)
  if type(data) == 'boolean' then
    self:addU8(NetworkMessageTypes.Boolean)
    self:addU8(booleantonumber(data))
  elseif type(data) == 'number' then
    self:addU8(NetworkMessageTypes.Number)
    self:addU64(data)
  elseif type(data) == 'string' then
    self:addU8(NetworkMessageTypes.String)
    self:addString(data)
  elseif type(data) == 'table' then
    self:addU8(NetworkMessageTypes.Table)
    self:addTable(data)
  else
    perror('Invalid data type '  .. type(data))
  end
end

function OutputMessage:addTable(data)
  local size = 0

  -- reserve for size
  local sizePos = self:getWritePos()
  self:addU32(size)
  local sizeSize = self:getWritePos() - sizePos

  -- add values
  for key,value in pairs(data) do
    self:addData(key)
    self:addData(value)
    size = size + 1
  end

  -- write size
  local currentPos = self:getWritePos()
  self:setWritePos(sizePos)
  self:addU32(size)

  -- fix msg size and go back to end
  self:setMessageSize(self:getMessageSize() - sizeSize)
  self:setWritePos(currentPos)
end