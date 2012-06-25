PlayerTrade = {}

local tradeWindow

local function createTrade()
  tradeWindow = g_ui.createWidget('TradeWindow', GameInterface.getRightPanel())
  tradeWindow.onClose = function()
    g_game.rejectTrade()
    tradeWindow:hide()
  end
end

local function fillTrade(name, items, counter)
  if not tradeWindow then
    createTrade()
  end

  local tradeItemWidget = tradeWindow:getChildById('tradeItem')
  tradeItemWidget:setItemId(items[1]:getId())

  local tradeContainer
  local label
  if counter then
    tradeContainer = tradeWindow:recursiveGetChildById('counterTradeContainer')
    label = tradeWindow:recursiveGetChildById('counterTradeLabel')

    tradeWindow:recursiveGetChildById('acceptButton'):enable()
  else
    tradeContainer = tradeWindow:recursiveGetChildById('ownTradeContainer')
    label = tradeWindow:recursiveGetChildById('ownTradeLabel')
  end
  label:setText(name)

  for index,item in ipairs(items) do
    local itemWidget = g_ui.createWidget('Item', tradeContainer)
    itemWidget:setItem(item)
    itemWidget:setVirtual(true)
    itemWidget:setMargin(1)
    itemWidget.onClick = function()
      g_game.inspectTrade(counter, index-1)
    end
  end
end

local function onGameOwnTrade(name, items)
  fillTrade(name, items, false)
end

local function onGameCounterTrade(name, items)
  fillTrade(name, items, true)
end

local function onGameCloseTrade()
  if not tradeWindow then return end
  tradeWindow:destroy()
  tradeWindow = nil
end

function PlayerTrade.init()
  g_ui.importStyle('tradewindow.otui')

  connect(g_game, { onOwnTrade = onGameOwnTrade,
                    onCounterTrade = onGameCounterTrade,
                    onCloseTrade = onGameCloseTrade,
                    onGameEnd = onGameCloseTrade })
end

function PlayerTrade.terminate()
  disconnect(g_game, { onOwnTrade = onGameOwnTrade,
                       onCounterTrade = onGameCounterTrade,
                       onCloseTrade = onGameCloseTrade,
                       onGameEnd = onGameCloseTrade })

  if tradeWindow then
    tradeWindow:destroy()
    tradeWindow = nil
  end
end
