PlayerTrade = {}

-- g_game.inspectTrade(counterOffer, index)
-- g_game.acceptTrade()
-- g_game.rejectTrade()

local tradeWindow

local function createTrade()
  if tradeWindow then
    tradeWindow:destroy()
    tradeWindow = nil
  end

  tradeWindow = createWidget('TradeWindow', rootWidget)
end

local function onOwnTrade(name, items)
  local firstItem = items[1]

  local tradeItemWidget = tradeWindow:getChildById('tradeItem')
  tradeItemWidget:setItem(firstItem)
end

local function onCounterTrade(name, items)

end

local function onCloseTrade()
  tradeWindow:destroy()
  tradeWindow = nil
end

function PlayerTrade.init()
  importStyle 'tradewindow.otui'

  connect(g_game, { onOwnTrade = onGameOwnTrade,
                    onCounterTrade = onGameCounterTrade,
                    onCloseTrade = onGameCloseTrade })
end

function PlayerTrade.terminate()
  disconnect(g_game, { onOwnTrade = onGameOwnTrade,
                       onCounterTrade = onGameCounterTrade,
                       onCloseTrade = onGameCloseTrade })
end
