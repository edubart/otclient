OfferStatistic = {}
OfferStatistic.__index = OfferStatistic

SECONDS_PER_DAY = 86400

OfferStatistic.new = function(timestamp, t, transactions, totalPrice, highestPrice, lowestPrice)
  local stat = {
    time = 0,
    type = nil,
    transactions = 0,
    totalPrice = 0,
    highestPrice = 0,
    lowestPrice = 0
  }
  stat.time = math.floor(timestamp / SECONDS_PER_DAY) * SECONDS_PER_DAY

  if t ~= MarketAction.Buy and t ~= MarketAction.Sell then
    g_logger.error('OfferStatistic.new - invalid type provided.')
  end
  stat.type = t

  stat.transactions = transactions
  stat.totalPrice = totalPrice
  stat.highestPrice = highestPrice
  stat.lowestPrice = lowestPrice

  setmetatable(stat, OfferStatistic)
  return stat
end

function OfferStatistic:isNull()
  return self.time == 0 or not self.type
end

-- Sets/Gets

function OfferStatistic:setTime(time)
  if not time or type(time) ~= 'number'  then
    g_logger.error('OfferStatistic.setTime - invalid time provided.')
  end
  self.time = time
end

function OfferStatistic:getTime()
  return self.time
end

function OfferStatistic:setType(t)
  if not t or type(t) ~= 'number'  then
    g_logger.error('OfferStatistic.setType - invalid type provided.')
  end
  self.type = t
end

function OfferStatistic:getType()
  return self.type
end

function OfferStatistic:setTransactions(transactions)
  if not transactions or type(transactions) ~= 'number'  then
    g_logger.error('OfferStatistic.setTransactions - invalid transactions provided.')
  end
  self.transactions = transactions
end

function OfferStatistic:getTransactions()
  return self.transactions
end

function OfferStatistic:setTotalPrice(amount)
  if not totalPrice or type(totalPrice) ~= 'number'  then
    g_logger.error('OfferStatistic.setTotalPrice - invalid total price provided.')
  end
  self.totalPrice = totalPrice
end

function OfferStatistic:getTotalPrice()
  return self.totalPrice
end

function OfferStatistic:setHighestPrice(highestPrice)
  if not highestPrice or type(highestPrice) ~= 'number'  then
    g_logger.error('OfferStatistic.setHighestPrice - invalid highestPrice provided.')
  end
  self.highestPrice = highestPrice
end

function OfferStatistic:getHighestPrice()
  return self.highestPrice
end

function OfferStatistic:setLowestPrice(lowestPrice)
  if not lowestPrice or type(lowestPrice) ~= 'number'  then
    g_logger.error('OfferStatistic.setLowestPrice - invalid lowestPrice provided.')
  end
  self.lowestPrice = lowestPrice
end

function OfferStatistic:getLowestPrice()
  return self.lowestPrice
end
