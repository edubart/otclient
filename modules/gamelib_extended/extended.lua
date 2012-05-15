Extended = {}

-- private variables
local callbacks = {}

-- hooked functions
local function onExtendedOpcode(protocol, opcode, buffer)
  local callback = callbacks[opcode]
  if callback then
    callback(opcode, buffer)
  end
end

-- public functions
function Extended.init()
  connect(ProtocolGame, { onExtendedOpcode = onExtendedOpcode } )
end

function Extended.terminate()
  disconnect(ProtocolGame, { onExtendedOpcode = onExtendedOpcode } )
  callbacks = nil
  Extended = nil
end

function Extended.register(opcode, callback)
  if not callback or type(callback) ~= 'function' then
    error('Invalid callback.')
    return false
  end

  if opcode < 0 or opcode > 255 then
    error('Invalid opcode. Range: 0-255')
    return false
  end

  if callbacks[opcode] then
    error('Opcode is already taken.')
    return false
  end

  callbacks[opcode] = callback
  return true
end

function Extended.unregister(opcode)
  if opcode < 0 or opcode > 255 then
    error('Invalid opcode. Range: 0-255')
    return false
  end

  if not callbacks[opcode] then
    error('Opcode is not registered.')
    return false
  end

  callbacks[opcode] = nil
  return true
end