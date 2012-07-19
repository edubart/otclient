function getMessageTypes(version)
  if version >= 960 then
    perror("TODO: message types for 9.6")
    return {}
  elseif version >= 861 then
    return {
      [13] = 'ConsoleOrange',
      [14] = 'ConsoleOrange',
      [15] = 'Warning',
      [16] = 'EventAdvance',
      [17] = 'EventDefault',
      [18] = 'StatusDefault',
      [19] = 'Info',
      [20] = 'StatusSmall',
      [21] = 'ConsoleBlue',
      [22] = 'ConsoleRed'
    }
  elseif version >= 854 then
    return {
      [18] = 'ConsoleRed',
      [19] = 'ConsoleOrange',
      [20] = 'ConsoleOrange',
      [21] = 'Warning',
      [22] = 'EventAdvance',
      [23] = 'EventDefault',
      [24] = 'StatusDefault',
      [25] = 'Info',
      [26] = 'StatusSmall',
      [27] = 'ConsoleBlue'
    }
  else
    return {
      [18] = 'Warning',
      [19] = 'EventAdvance',
      [20] = 'EventDefault',
      [21] = 'StatusDefault',
      [22] = 'Info',
      [23] = 'StatusSmall',
      [24] = 'ConsoleBlue',
      [25] = 'ConsoleRed',
      [26] = 'ConsoleOrange',
      [27] = 'ConsoleOrange',
    }
   end
end

function parseTextMessage(msg)
  local msgtype = msg:getU8()
  local text = msg:getString()
  msgtype = getMessageTypes(g_game.getProtocolVersion())[msgtype]
  signalcall(g_game.onTextMessage, msgtype, text)
end

function registerProtocol()
  ProtocolGame.registerOpcode(GameServerOpcodes.GameServerTextMessage, parseTextMessage)
end

function unregisterProtocol()
  ProtocolGame.unregisterOpcode(GameServerOpcodes.GameServerTextMessage)
end
