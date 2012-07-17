-- @docclass
ProtocolLogin = extends(Protocol)

-- set to the latest Tibia.pic signature to make otclient compatible with official tibia
local PIC_SIGNATURE = 0

LoginServerError = 10
LoginServerMotd = 20
LoginServerUpdateNeeded = 30
LoginServerCharacterList = 100


-- private functions
local function sendLoginPacket(protocol)
  local msg = OutputMessage.create()
  msg:addU8(ClientOpcodes.ClientEnterAccount)
  msg:addU16(g_game.getOsType())
  msg:addU16(g_game.getProtocolVersion())

  msg:addU32(g_things.getDatSignature())
  msg:addU32(g_sprites.getSprSignature())
  msg:addU32(PIC_SIGNATURE)

  local paddingBytes = 128
  msg:addU8(0) -- first RSA byte must be 0
  paddingBytes = paddingBytes - 1

  -- xtea key
  protocol:generateXteaKey()
  local xteaKey = protocol:getXteaKey()
  msg:addU32(xteaKey[1])
  msg:addU32(xteaKey[2])
  msg:addU32(xteaKey[3])
  msg:addU32(xteaKey[4])
  paddingBytes = paddingBytes - 16

  if g_game.getFeature(GameProtocolChecksum) then
    protocol:enableChecksum()
  end

  if g_game.getFeature(GameAccountNames) then
    msg:addString(protocol.accountName)
    msg:addString(protocol.accountPassword)
    paddingBytes = paddingBytes - (4 + string.len(protocol.accountName) + string.len(protocol.accountPassword))
  else
    msg:addU32(tonumber(protocol.accountName))
    msg:addString(protocol.accountPassword)
    paddingBytes = paddingBytes - (6 + string.len(protocol.accountPassword))
  end

  msg:addPaddingBytes(paddingBytes, 0)
  msg:encryptRsa(128, g_game.getRsa())

  protocol:send(msg)
  protocol:enableXteaEncryption()
  protocol:recv()
end

-- events
function ProtocolLogin:onConnect()
  self:connectCallback(self)
end

function ProtocolLogin:onRecv(msg)
  while not msg:eof() do
    local opcode = msg:getU8()
    if opcode == LoginServerError then
      self:parseError(msg)
    elseif opcode == LoginServerMotd then
      self:parseMotd(msg)
    elseif opcode == LoginServerUpdateNeeded then
      signalcall(self.onError, self, tr("Client needs update."))
    elseif opcode == LoginServerCharacterList then
      self:parseCharacterList(msg)
    else
      self:parseOpcode(opcode, msg)
    end
  end
  self:disconnect()
end

-- public functions
function ProtocolLogin.create()
  return ProtocolLogin.internalCreate()
end

function ProtocolLogin:login(host, port, accountName, accountPassword)
  if string.len(accountName) == 0 or string.len(accountPassword) == 0 then
    signalcall(self.onError, self, tr("You must enter an account name and password."))
    return
  end
  if string.len(host) == 0 or port == nil or port == 0 then
    signalcall(self.onError, self, tr("You must enter a valid server address and port."))
    return
  end

  self.accountName = accountName
  self.accountPassword = accountPassword
  self.connectCallback = sendLoginPacket

  self:connect(host, port)
end

function ProtocolLogin:cancelLogin()
  self:disconnect()
end

function ProtocolLogin:parseError(msg)
  local errorMessage = msg:getString()
  signalcall(self.onError, self, errorMessage)
end

function ProtocolLogin:parseMotd(msg)
  local motd = msg:getString()
  signalcall(self.onMotd, self, motd)
end

function ProtocolLogin:parseCharacterList(msg)
  local characters = {}
  local charactersCount = msg:getU8()
  for i=1,charactersCount do
    local character = {}
    character[1] = msg:getString()
    character[2] = msg:getString()
    character[3] = iptostring(msg:getU32())
    character[4] = msg:getU16()
    characters[i] = character
  end
  local premDays = msg:getU16()
  signalcall(self.onCharacterList, self, characters, premDays)
end

function ProtocolLogin:parseOpcode(opcode, msg)
  signalcall(self.onOpcode, self, opcode, msg)
end
