function InputMessage:getData()
  local dataType = self:getU8()
  if dataType == NetworkMessageTypes.Boolean then
    return numbertoboolean(self:getU8())
  elseif dataType == NetworkMessageTypes.Number then
    return self:getU64()
  elseif dataType == NetworkMessageTypes.String then
    return self:getString()
  elseif dataType == NetworkMessageTypes.Table then
    return self:getTable()
  else
    perror('Unknown data type ' .. dataType)
  end
  return nil
end

function InputMessage:getTable()
  local ret = {}
  local size = self:getU32()
  for i=1,size do
    local index = self:getData()
    local value = self:getData()
    ret[index] = value
  end
  return ret
end