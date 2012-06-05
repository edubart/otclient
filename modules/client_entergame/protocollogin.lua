ProtocolLogin = extends(Protocol)

function ProtocolLogin.create()
  local protocolLogin = ProtocolLogin.internalCreate()
  return protocolLogin
end

LoginServerError = 10
LoginServerMotd = 20
LoginServerUpdateNeeded = 30
LoginServerCharacterList = 100

GameExtendedOpcode = 0
GameProtocolChecksum = 1
GameAccountNames = 2
GameChallangeOnLogin = 3
GameStackposOnTileAddThing = 4
GamePenalityOnDeath = 5
GameNameOnNpcTrade = 6
GameDoubleFreeCapacity = 7
GameDoubleExperience = 8
GameTotalCapacity = 9
GameSkillsBase = 10
GameAdditionalPlayerStats = 11
GameIdOnCancelAttack = 12
GameChannelPlayerList = 13
GamePlayerMounts = 14
GameEnvironmentEffect = 15
GameCreatureType = 16
GameCreatureAdditionalInfo = 17
GameCreaturePassableInfo = 18
GameItemAnimationPhase = 19
GameTrucatedPingOpcode = 20
GameReverseCreatureStack = 21

RSA = "109120132967399429278860960508995541528237502902798129123468757937266291492576446330739696001110603907230888610072655818825358503429057592827629436413108566029093628212635953836686562675849720620786279431090218017681061521755056710823876476444260558147179707119674283982419152118103759076030616683978566631413"

function ProtocolLogin:parseError(msg)
  local errorMessage = msg:getString()
  signalcall(self.onError, self, errorMessage, false)
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

function ProtocolLogin:onOpcode(opcode, msg)
  if opcode == LoginServerError then
    self:parseError(msg)
  elseif opcode == LoginServerMotd then
    self:parseMotd(msg)
  elseif opcode == LoginServerUpdateNeeded then
    return false
	elseif opcode == LoginServerCharacterList then
    self:parseCharacterList(msg)
  else
    return false
  end
  return true
end

function ProtocolLogin:sendLoginPacket()
  local msg = OutputMessage.create()
  msg:addU8(1) -- ClientEnterAccount
  msg:addU16(1) -- ClientOs
  msg:addU16(g_game.getClientVersion())

  msg:addU32(g_thingsType.getSignature())
  msg:addU32(g_sprites.getSignature())
  msg:addU32(0) -- pic sig

  local paddingBytes = 128
  msg:addU8(0) -- first RSA byte must be 0
  paddingBytes = paddingBytes - 1

  -- xtea key
  self:generateXteaKey()
  local xteaKey = self:getXteaKey()
  msg:addU32(xteaKey[1])
  msg:addU32(xteaKey[2])
  msg:addU32(xteaKey[3])
  msg:addU32(xteaKey[4])
  paddingBytes = paddingBytes - 16

  if g_game.getFeature(GameProtocolChecksum) then
    self:enableChecksum()
  end

  if g_game.getFeature(GameAccountNames) then
    msg:addString(self.accountName)
    msg:addString(self.accountPassword)
    paddingBytes = paddingBytes - (4 + string.len(self.accountName) + string.len(self.accountPassword))
  else
    msg:addU32(tonumber(self.accountName))
    msg:addString(self.accountPassword)
    paddingBytes = paddingBytes - (6 + string.len(self.accountPassword))
  end

  msg:addPaddingBytes(paddingBytes) -- this crashes without 0
  msg:encryptRSA(128, RSA)

  self:send(msg)
  self:enableChecksum()
  self:recv()
end

-- void ProtocolLogin::sendLoginPacket()
-- {
--     OutputMessagePtr msg(new OutputMessage);

--     msg->addU8(Proto::ClientEnterAccount);
--     msg->addU16(Proto::ClientOs);
--     msg->addU16(Proto::ClientVersion);

--     msg->addU32(g_thingsType.getSignature()); // data signature
--     msg->addU32(g_sprites.getSignature()); // sprite signature
--     msg->addU32(Proto::PicSignature); // pic signature

--     int paddingBytes = 128;
--     msg->addU8(0); // first RSA byte must be 0
--     paddingBytes -= 1;

--     // xtea key
--     generateXteaKey();

--     msg->addU32(m_xteaKey[0]);
--     msg->addU32(m_xteaKey[1]);
--     msg->addU32(m_xteaKey[2]);
--     msg->addU32(m_xteaKey[3]);
--     paddingBytes -= 16;

--     if(g_game.getFeature(Otc::GameProtocolChecksum))
--         enableChecksum();

--     if(g_game.getFeature(Otc::GameAccountNames)) {
--         msg->addString(m_accountName);
--         msg->addString(m_accountPassword);
--         paddingBytes -= 4 + m_accountName.length() + m_accountPassword.length();
--     } else {
--         msg->addU32(stdext::from_string<uint32>(m_accountName));
--         msg->addString(m_accountPassword);
--         paddingBytes -= 6 + m_accountPassword.length();
--     }

--     msg->addPaddingBytes(paddingBytes); // complete the 128 bytes for rsa encryption with zeros
--     msg->encryptRSA(128, Proto::RSA);

--     send(msg);
--     enableXteaEncryption();
--     recv();
-- }

-- events
function ProtocolLogin:onConnect()
  self:sendLoginPacket()
end

-- public functions
function ProtocolLogin:login(host, port, accountName, accountPassword)
  if string.len(accountName) == 0 or string.len(accountPassword) == 0 then
    signalcall(self.onError, self, "You must enter an account name and password.", false)
  end

  self.accountName = accountName
  self.accountPassword = accountPassword

  self:connect(host, port)
end
