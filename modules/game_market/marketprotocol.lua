MarketProtocol = {}

local market

-- private functions

local function parseOpcode(protocol, opcode, msg)
  if not g_game.getFeature(GamePlayerMarket) then
    return false
  end

  -- process msg
  if opcode == GameServerOpcodes.GameServerMarketEnter then
    parseMarketEnter(msg)
  elseif opcode == GameServerOpcodes.GameServerMarketLeave then
    parseMarketLeave(msg)
  elseif opcode == GameServerOpcodes.GameServerMarketDetail then
    parseMarketDetail(msg)
  elseif opcode == GameServerOpcodes.GameServerMarketBrowse then
    parseMarketBrowse(msg)
  else
    return false
  end
  return true
end

local function send(msg)
  print(msg:getMessageSize())
  g_game.getProtocolGame():safeSend(msg)
end

local function readMarketOffer(msg, action, var)
  local timestamp = msg:getU32()
  local counter = msg:getU16()

  local itemId = 0
  if var == MarketRequest.MyOffers or var == MarketRequest.MyHistory then
    itemId = msg:getU16()
  else
    itemId = var
  end

  local amount = msg:getU16()
  local price = msg:getU32()
  local playerName
  local state = MarketOfferState.Active
  if var == MarketRequest.MyHistory then
    state = msg:getU8()
  else
    playerName = msg:getString()
  end
  
  return MarketOffer.new({timestamp, counter}, action, itemId, amount, price, playerName, state)
end

-- parsing protocols

local function parseMarketEnter(msg)
  local balance = msg:getU32()
  local offers = msg:getU8()
  local depotItems = {}

  local depotCount = (msg:getU16() - 1)
  for i = 0, depotCount do
    local itemId = msg:getU16() -- item id
    local itemCount = msg:getU16() -- item count

    table.insert(depotItems, {itemId, itemCount})
  end

  Market.onMarketEnter(depotItems, offers, balance)
end

local function parseMarketLeave(msg)
  Market.onMarketLeave()
end

local function parseMarketDetail(msg)
  local itemId = msg:getU16()

  local descriptions = {}
  for i = MarketItemDescription.First, MarketItemDescription.Last do
    if msg:peekU16() ~= 0x00 then
      table.insert(descriptions, {i, msg:getString()})
    else
      msg:getU16()
    end
  end

  local purchaseStats = {}
  if msg:getU8() == 0x01 then
    local transactions = msg:getU32() -- transaction count
    local totalPrice = msg:getU32() -- total price
    local highestPrice = msg:getU32() -- highest price
    local lowestPrice = msg: getU32() -- lowest price

    table.insert(purchaseStats, {transaction, totalPrice, highestPrice, lowestPrice})
  end

  local saleStats = {}
  if msg:getU8() == 0x01 then
    local transactions = msg:getU32() -- transaction count
    local totalPrice = msg:getU32() -- total price
    local highestPrice = msg:getU32() -- highest price
    local lowestPrice = msg: getU32() -- lowest price

    table.insert(saleStats, {transaction, totalPrice, highestPrice, lowestPrice})
  end

  Market.onMarketDetail(itemId, descriptions, purchaseStats, saleStats)
end

local function parseMarketBrowse(msg)
  local var = msg:getU16()
  local offers = {}

  local buyOfferCount = (msg:getU32() - 1)
  for i = 0, buyOfferCount do
    table.insert(offers, readMarketOffer(msg, MarketAction.Buy, var))
  end

  local sellOfferCount = (msg:getU32() - 1)
  for i = 0, sellOfferCount do
    table.insert(offers, readMarketOffer(msg, MarketAction.Sell, var))
  end

  Market.onMarketBrowse(offers)
end

-- public functions

function MarketProtocol.init()
  connect(ProtocolGame, { onOpcode = parseOpcode } )

end

function MarketProtocol.terminate()
  disconnect(ProtocolGame, { onOpcode = parseOpcode } )

  market = nil
  MarketProtocol = nil
end

-- sending protocols

function MarketProtocol.sendMarketLeave()
  if g_game.getFeature(GamePlayerMarket) then
    local msg = OutputMessage.create()
    msg:addU8(ClientOpcodes.ClientMarketLeave)
    send(msg)
  else
    g_logger.error('MarketProtocol.sendMarketLeave does not support the current protocol.')
  end
end

function MarketProtocol.sendMarketBrowse(browseId)
  if g_game.getFeature(GamePlayerMarket) then
    local msg = OutputMessage.create()
    msg:addU8(ClientOpcodes.ClientMarketBrowse)
    msg:addU16(browseId)
    send(msg)
  else
    g_logger.error('MarketProtocol.sendMarketBrowse does not support the current protocol.')
  end
end

function MarketProtocol.sendMarketCreateOffer(type, spriteId, amount, price, anonymous)
  if g_game.getFeature(GamePlayerMarket) then
    local msg = OutputMessage.create()
    msg:addU8(ClientOpcodes.ClientMarketCreate)
    msg:addU8(type)
    msg:addU16(spriteId)
    msg:addU16(amount)
    msg:addU32(price)
    msg:addU8(anonymous)
    send(msg)
  else
    g_logger.error('MarketProtocol.sendMarketCreateOffer does not support the current protocol.')
  end
end

function MarketProtocol.sendMarketCancelOffer(counter)
  if g_game.getFeature(GamePlayerMarket) then
    local msg = OutputMessage.create()
    msg:addU8(ClientOpcodes.ClientMarketCancel)
    msg:addU32(os.time())
    msg:addU16(counter)
    send(msg)
  else
    g_logger.error('MarketProtocol.sendMarketCancelOffer does not support the current protocol.')
  end
end

function MarketProtocol.sendMarketAcceptOffer(counter)
  if g_game.getFeature(GamePlayerMarket) then
    local msg = OutputMessage.create()
    msg:addU8(ClientOpcodes.ClientMarketAccept)
    msg:addU32(os.time())
    msg:addU16(counter)
    send(msg)
  else
    g_logger.error('MarketProtocol.sendMarketAcceptOffer does not support the current protocol.')
  end
end
