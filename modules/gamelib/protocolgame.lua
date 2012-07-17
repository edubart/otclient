local opcodeCallbacks = {}

function ProtocolGame:onOpcode(opcode, msg)
  for i, callback in pairs(opcodeCallbacks) do
    if i == opcode then
      callback(msg)
      return true
    end
  end
  return false
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
