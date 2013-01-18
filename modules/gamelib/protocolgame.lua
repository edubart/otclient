local opcodeCallbacks = {}
local extendedCallbacks = {}

function ProtocolGame:onOpcode(opcode, msg)
  for i, callback in pairs(opcodeCallbacks) do
    if i == opcode then
      callback(self, msg)
      return true
    end
  end
  return false
end

function ProtocolGame:onExtendedOpcode(opcode, buffer)
  local callback = extendedCallbacks[opcode]
  if callback then
    callback(self, opcode, buffer)
  end
end

function ProtocolGame.registerOpcode(opcode, callback)
  if opcodeCallbacks[opcode] then
    error('opcode ' .. opcode .. ' already registered will be overriden')
  end

  opcodeCallbacks[opcode] = callback
end

function ProtocolGame.unregisterOpcode(opcode)
  opcodeCallbacks[opcode] = nil
end

function ProtocolGame.registerExtendedOpcode(opcode, callback)
  if not callback or type(callback) ~= 'function' then
    error('Invalid callback.')
  end

  if opcode < 0 or opcode > 255 then
    error('Invalid opcode. Range: 0-255')
  end

  if extendedCallbacks[opcode] then
    error('Opcode is already taken.')
  end

  extendedCallbacks[opcode] = callback
end

function ProtocolGame.unregisterExtendedOpcode(opcode)
  if opcode < 0 or opcode > 255 then
    error('Invalid opcode. Range: 0-255')
  end

  if not extendedCallbacks[opcode] then
    error('Opcode is not registered.')
  end

  extendedCallbacks[opcode] = nil
end
