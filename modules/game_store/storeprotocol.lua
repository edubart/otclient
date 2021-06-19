StoreProtocol = {}

-- PUBLIC ENUMS

StoreProtocol.OfferTypes = {
  OFFER_TYPE_NONE = 0, -- (this will disable offer)
  OFFER_TYPE_ITEM = 1,
  OFFER_TYPE_STACKABLE = 2,
  OFFER_TYPE_OUTFIT = 3,
  OFFER_TYPE_OUTFIT_ADDON = 4,
  OFFER_TYPE_MOUNT = 5,
  OFFER_TYPE_NAMECHANGE = 6,
  OFFER_TYPE_SEXCHANGE = 7,
  OFFER_TYPE_PROMOTION = 8,
  OFFER_TYPE_HOUSE = 9,
  OFFER_TYPE_EXPBOOST = 10,
  OFFER_TYPE_PREYSLOT = 11,
  OFFER_TYPE_PREYBONUS = 12,
  OFFER_TYPE_TEMPLE = 13,
  OFFER_TYPE_BLESSINGS = 14,
  OFFER_TYPE_PREMIUM = 15,
  OFFER_TYPE_POUNCH = 16,
  OFFER_TYPE_ALLBLESSINGS = 17
}

StoreProtocol.ClientOfferTypes = {
  CLIENT_STORE_OFFER_OTHER = 0,
  CLIENT_STORE_OFFER_NAMECHANGE = 1
}

StoreProtocol.HistoryTypes = {
  HISTORY_TYPE_NONE = 0,
  HISTORY_TYPE_GIFT = 1,
  HISTORY_TYPE_REFUND = 2
}

StoreProtocol.States = {
  STATE_NONE = 0,
  STATE_NEW = 1,
  STATE_SALE = 2,
  STATE_TIMED = 3
}

StoreProtocol.StoreErrors = {
  STORE_ERROR_PURCHASE = 0,
  STORE_ERROR_NETWORK = 1,
  STORE_ERROR_HISTORY = 2,
  STORE_ERROR_TRANSFER = 3,
  STORE_ERROR_INFORMATION = 4
}

StoreProtocol.ServiceTypes = {
  SERVICE_STANDARD = 0,
  SERVICE_OUTFITS = 3,
  SERVICE_MOUNTS = 4,
  SERVICE_BLESSINGS = 5
}

local protocol
local isSilent

local function send(msg)
  if protocol and not isSilent then
    --print('sent', msg)
    protocol:send(msg)
  end
end

function initProtocol()
  connect(g_game, { onGameStart = StoreProtocol.registerProtocol,
                    onGameEnd = StoreProtocol.unregisterProtocol })

  -- reloading module
  if g_game.isOnline() then
    StoreProtocol.registerProtocol()
  end

  StoreProtocol.setIsSilent(false)
end

function terminateProtocol()
  disconnect(g_game, { onGameStart = StoreProtocol.registerProtocol,
                       onGameEnd = StoreProtocol.unregisterProtocol })

  -- reloading module
  StoreProtocol.unregisterProtocol()
  StoreProtocol = nil
end

-- PRIVATE

local function parseCoinBalance(protocol, msg)
  msg:getU8()
  local balance = msg:getU32()
  msg:getU32() -- duplicate of balance

  signalcall(Store.onCoinBalance, balance)
  return true
end

local function parseCoinBalanceUpdating(protocol, msg)
  msg:getU8() -- 0x00 is updating

  signalcall(Store.onCoinBalanceUpdating)
  return true
end

local function parseStoreError(protocol, msg)
  --[[
    STORE_ERROR_PURCHASE=0,
    STORE_ERROR_NETWORK,
    STORE_ERROR_HISTORY,
    STORE_ERROR_TRANSFER,
    STORE_ERROR_INFORMATION
  ]]


  local error = msg:getU8()
  local message = msg:getString()

  signalcall(Store.onStoreError, error, message)
  return true
end

local function parseRequestPurchaseData(protocol, msg)
  local offerId = msg:getU32()
  local clientOfferType = msg:getU8()

  signalcall(Store.onRequestPurchaseData, offerId, clientOfferType)
  return true
end

local function parseOpenStore(protocol, msg)
  local categories = {}
  msg:getU8()
  local categoriesCount = msg:getU16()

  for i=1, categoriesCount do
    local category = {}

    category.name = msg:getString()
    category.description = msg:getString()
    category.state = msg:getU8()
    category.icons = {}

    local iconsCount = msg:getU8()
    for j=1, iconsCount do
      category.icons[j] = msg:getString()
    end

    category.parentCategory = msg:getString()

    categories[i] = category
  end

  signalcall(Store.onOpenStore, categories)
  return true
end

local function parseStoreOffers(protocol, msg)
  local category = {}
  category.name = msg:getString()
  category.offers = {}

  local offerCount = msg:getU16()

  for i=1, offerCount do
    local offer = {}

    offer.id = msg:getU32()
    offer.name = msg:getString() -- this is item amount and item name

    -- create a pseudo amount field
    local match = string.gmatch(offer.name, "(%d+)x ")
    local amount = match(1)
    if amount then
      offer.amount = amount
    else
      offer.amount = 1
    end

    offer.description = msg:getString()
    offer.price = msg:getU32()
    offer.state = msg:getU8()

    offer.basePrice = nil
    offer.validUntil = nil
    if offer.state == StoreProtocol.States.STATE_SALE then
      offer.validUntil = msg:getU32()
      offer.basePrice = msg:getU32()
    end

    offer.type = msg:getU8()

    offer.disableReason = ""
    if offer.type == 0 then
      offer.disableReason = msg:getString()
    end

    if offer.type == StoreProtocol.OfferTypes.OFFER_TYPE_PREMIUM then
      local daysCount = string.gmatch(offer.name, "(%d+) Days")
      offer.amount = daysCount(1)
    end

    offer.icons = {}
    local iconCount = msg:getU8()
    for j=1, iconCount do
      offer.icons[j] = msg:getString()
    end

    msg:getU16() -- 0

    category.offers[i] = offer
  end

  signalcall(Store.onStoreOffers, category)
  return true
end

local function parseOpenTransactionHistory(protocol, msg)
  local history = {}
  local pageNumber = msg:getU32()
  local isLastPage = msg:getU32() -- 0,1 last page
  local historyEntryCount = msg:getU8()

  for i=1, historyEntryCount do
    local historyEntry = {}

    historyEntry.time = msg:getU32()
    historyEntry.mode = msg:getU8()
    historyEntry.amount = msg:getU32()
    historyEntry.description = msg:getString()

    history[i] = historyEntry
  end

  signalcall(Store.onOpenTransactionHistory, pageNumber, isLastPage, history)
  return true
end

local function parseCompletePurchase(protocol, msg)
  msg:getU8()
  local message = msg:getString()
  local balance = msg:getU32()
  msg:getU32() -- duplicate of balance

  signalcall(Store.onCompletePurchase, message, balance)
  return true
end

function StoreProtocol.updateProtocol(_protocol)
  protocol = _protocol
end

local function logUnsupportedError(mthdname)
  return g_logger.error(string.format('StoreProtocol.%s does not support the current protocol.', mthdname))
end
-- PUBLIC

function StoreProtocol.setIsSilent(_isSilent)
  isSilent = _isSilent
end

function StoreProtocol.registerProtocol()
  if g_game.getFeature(GameIngameStore) then
    ProtocolGame.registerOpcode(GameServerOpcodes.GameServerCoinBalance, parseCoinBalance)
    ProtocolGame.registerOpcode(GameServerOpcodes.GameServerStoreError, parseStoreError)
    ProtocolGame.registerOpcode(GameServerOpcodes.GameServerRequestPurchaseData, parseRequestPurchaseData)
    ProtocolGame.registerOpcode(GameServerOpcodes.GameServerCoinBalanceUpdating, parseCoinBalanceUpdating)
    ProtocolGame.registerOpcode(GameServerOpcodes.GameServerStore, parseOpenStore) -- GameServerOpenStore
    ProtocolGame.registerOpcode(GameServerOpcodes.GameServerStoreOffers, parseStoreOffers)
    ProtocolGame.registerOpcode(GameServerOpcodes.GameServerStoreTransactionHistory, parseOpenTransactionHistory) -- GameServerStoreTransactionHistory
    ProtocolGame.registerOpcode(GameServerOpcodes.GameServerStoreCompletePurchase, parseCompletePurchase) -- GameServerCompletePurchase
  end

  StoreProtocol.updateProtocol(g_game.getProtocolGame())
end

function StoreProtocol.unregisterProtocol()
  if g_game.getFeature(GameIngameStore) then
    ProtocolGame.unregisterOpcode(GameServerOpcodes.GameServerCoinBalance, parseCoinBalance)
    ProtocolGame.unregisterOpcode(GameServerOpcodes.GameServerStoreError, parseStoreError)
    ProtocolGame.unregisterOpcode(GameServerOpcodes.GameServerRequestPurchaseData, parseRequestPurchaseData)
    ProtocolGame.unregisterOpcode(GameServerOpcodes.GameServerCoinBalanceUpdating, parseCoinBalanceUpdating)
    ProtocolGame.unregisterOpcode(GameServerOpcodes.GameServerStore, parseOpenStore)
    ProtocolGame.unregisterOpcode(GameServerOpcodes.GameServerStoreOffers, parseStoreOffers)
    ProtocolGame.unregisterOpcode(GameServerOpcodes.GameServerStoreTransactionHistory, parseOpenTransactionHistory)
    ProtocolGame.unregisterOpcode(GameServerOpcodes.GameServerStoreCompletePurchase, parseCompletePurchase)
  end

  StoreProtocol.updateProtocol(nil)
end

function StoreProtocol.sendStoreEvent()
  if not g_game.getFeature(GameIngameStore) then logUnsupportedError('sendStoreEvent') end

  local msg = OutputMessage.create()
  -- not used
  send(msg)
end

function StoreProtocol.sendTransferCoins(transferReceiver, amount)
  if not g_game.getFeature(GameIngameStore) then logUnsupportedError('sendTransferCoins') end

  local player = g_game.getLocalPlayer()

  local msg = OutputMessage.create()
  msg:addU8(ClientOpcodes.ClientTransferCoins)
  msg:addString(transferReceiver)
  msg:addU32(amount)
  send(msg)
end

function StoreProtocol.sendOpenStore()
  if not g_game.getFeature(GameIngameStore) then logUnsupportedError('sendOpenStore') end


  --[[
  GameStore.ServiceTypes = {
    SERVICE_STANDERD = 0,
    SERVICE_OUTFITS = 3,
    SERVICE_MOUNTS = 4,
    SERVICE_BLESSINGS = 5
  }
  ]]

  local msg = OutputMessage.create()
  msg:addU8(ClientOpcodes.ClientOpenStore)
  --msg:addU8(StoreProtocol.ServiceTypes.SERVICE_OUTFITS) -- TODO: add an option to set service type
  msg:addU8(0) -- TODO: add an option to set service type
  send(msg)
end

function StoreProtocol.sendRequestStoreOffers(categoryName)
  if not g_game.getFeature(GameIngameStore) then logUnsupportedError('sendRequestStoreOffers') end

  local msg = OutputMessage.create()
  msg:addU8(ClientOpcodes.ClientRequestStoreOffers)

  if g_game.getClientVersion() >= 1092 then
    msg:addU8(StoreProtocol.ServiceTypes.SERVICE_STANDARD)  -- TODO: add an option to set service type
  end

  msg:addString(categoryName)
  send(msg)
end

function StoreProtocol.sendBuyStoreOffer(offerId, offerType, newName)
  if not g_game.getFeature(GameIngameStore) then logUnsupportedError('sendBuyStoreOffer') end

  local msg = OutputMessage.create()
  msg:addU8(ClientOpcodes.ClientBuyStoreOffer)
  msg:addU32(offerId)
  msg:addU8(offerType)

  if offerType == StoreProtocol.ClientOfferTypes.CLIENT_STORE_OFFER_NAMECHANGE then
    msg:addString(newName)
  end
  send(msg)
end

function StoreProtocol.sendOpenTransactionHistory(entriesPerPageCount)
  if not g_game.getFeature(GameIngameStore) then logUnsupportedError('sendOpenTransactionHistory') end

  local msg = OutputMessage.create()
  msg:addU8(ClientOpcodes.ClientOpenTransactionHistory)
  msg:addU8(entriesPerPageCount)
  send(msg)
end

function StoreProtocol.sendRequestTransactionHistory(page)
  if not g_game.getFeature(GameIngameStore) then logUnsupportedError('sendRequestTransactionHistory') end

  local msg = OutputMessage.create(page)
  msg:addU8(ClientOpcodes.ClientRequestTransactionHistory)
  msg:addU32(page)
  send(msg)
end