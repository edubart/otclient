Market = {}

local marketWindow

function Market.init()
  g_ui.importStyle('market.otui')

end

function Market.terminate()
  marketWindow = nil
  Market = nil
end

function Market.onMarketEnter(depotItems, offers, balance)
  -- open market window
  -- populate market?
  print('onMarketEnter')
  print(offers)
  print(balance)
  print('depotItems:')
  for k, item in pairs(depotItems) do
    print('id- '..item[1])
    print('count- '..item[2])
  end
end

function Market.onMarketLeave()
  -- close market window?
  print('onMarketLeave')
end

function Market.onMarketDetail(itemId, descriptions, purchaseStats, saleStats)
  -- populate market widget
  print('onMarketDetail')
  print(itemId)
  print('descriptions:')
  for k, desc in pairs(descriptions) do
    print('type- '..desc[1])
    print('description- '..desc[2])
  end
  print('purchaseStats:')
  for k, stat in pairs(purchaseStats) do
    print('transactions- '..stat[1])
    print('total price- '..stat[2])
    print('highest price- '..stat[3])
    print('lowest price- '..stat[4])
  end
  print('saleStats:')
  for k, stat in pairs(saleStats) do
    print('transactions- '..stat[1])
    print('total price- '..stat[2])
    print('highest price- '..stat[3])
    print('lowest price- '..stat[4])
  end
end

function Market.onMarketBrowse(offers)
  -- populate market widget
  print('onMarketBrowse')
end
