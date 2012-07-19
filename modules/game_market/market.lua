Market = {}

g_ui.importStyle('market.otui')
g_ui.importStyle('ui/general/markettabs.otui')
g_ui.importStyle('ui/general/marketbuttons.otui')

local marketWindow
local mainTabBar

local marketOffersPanel
local selectionTabBar
local browsePanel
local searchPanel

local displaysTabBar
local itemOffersPanel
local itemDetailsPanel
local itemStatsPanel

local myOffersPanel
local offersTabBar
local currentOffersPanel
local offerHistoryPanel

local marketOffers = {}
local depot = {}
local information ={}
local selectedItem
local nameLabel

local itemsPanel
local radioItems

local function clearSelectedItem()
  if selectedItem then
    nameLabel:clearText()
    radioItems:selectWidget(nil)
    selectedItem.setItem(nil)
  end
end

local function loadMarketItems()
  itemsPanel = marketWindow:recursiveGetChildById('itemsPanel')

  local layout = itemsPanel:getLayout()
  layout:disableUpdates()

  clearSelectedItem()
  itemsPanel:destroyChildren()

  if radioItemSet then
    radioItemSet:destroy()
  end
  radioItemSet = UIRadioGroup.create()

  -- TODO: populate with dat items
  

  layout:enableUpdates()
  layout:update()
end

local function loadDepotItems(depotItems)
  information.depotItems = {}
  for _, data in pairs(depotItems) do
    local item = Item.create(data[1])
    if not item then
      break
    end
    item:setCount(data[2])
    local marketData = item:getMarketData()

    if not table.empty(marketData) then
      local newItem = {
        ptr = item,
        marketData = marketData
      }
      table.insert(information.depotItems, newItem)
    end
  end
end

function Market.init()
  marketWindow = g_ui.createWidget('MarketWindow', rootWidget)
  marketWindow:hide()

  nameLabel = marketWindow:recursiveGetChildById('nameLabel')

  -- TODO: clean this up into functions
  -- setup main tabs
  mainTabBar = marketWindow:getChildById('mainTabBar')
  mainTabBar:setContentWidget(marketWindow:getChildById('mainTabContent'))

  -- setup 'Market Offer' section tabs
  marketOffersPanel = g_ui.loadUI('ui/marketoffers.otui')
  mainTabBar:addTab(tr('Market Offers'), marketOffersPanel)

    selectionTabBar = marketOffersPanel:getChildById('leftTabBar')
    selectionTabBar:setContentWidget(marketOffersPanel:getChildById('leftTabContent'))

      browsePanel = g_ui.loadUI('ui/marketoffers/browse.otui')
      selectionTabBar:addTab(tr('Browse'), browsePanel)

      searchPanel = g_ui.loadUI('ui/marketoffers/search.otui')
      selectionTabBar:addTab(tr('Search'), searchPanel)

    displaysTabBar = marketOffersPanel:getChildById('rightTabBar')
    displaysTabBar:setContentWidget(marketOffersPanel:getChildById('rightTabContent'))

      itemOffersPanel = g_ui.loadUI('ui/marketoffers/itemoffers.otui')
      displaysTabBar:addTab(tr('Offers'), itemOffersPanel)

      itemDetailsPanel = g_ui.loadUI('ui/marketoffers/itemdetails.otui')
      displaysTabBar:addTab(tr('Details'), itemDetailsPanel)

      itemStatsPanel = g_ui.loadUI('ui/marketoffers/itemstats.otui')
      displaysTabBar:addTab(tr('Statistics'), itemStatsPanel)

  -- setup 'My Offer' section tabs
  myOffersPanel = g_ui.loadUI('ui/myoffers.otui')
  mainTabBar:addTab(tr('My Offers'), myOffersPanel)
    
    offersTabBar = myOffersPanel:getChildById('offersTabBar')
    offersTabBar:setContentWidget(myOffersPanel:getChildById('offersTabContent'))

      currentOffersPanel = g_ui.loadUI('ui/myoffers/currentoffers.otui')
      offersTabBar:addTab(tr('Current Offers'), currentOffersPanel)

      offerHistoryPanel = g_ui.loadUI('ui/myoffers/offerhistory.otui')
      offersTabBar:addTab(tr('Offer History'), offerHistoryPanel)
end

function Market.terminate()
  if marketWindow then
    marketWindow:destroy()
    marketWindow = nil
  end

  mainTabBar = nil
  marketOffersPanel = nil
  selectionTabBar = nil
  browsePanel = nil
  searchPanel = nil
  displaysTabBar = nil
  itemOffersPanel = nil
  itemDetailsPanel = nil
  itemStatsPanel = nil
  myOffersPanel = nil
  offersTabBar = nil
  currentOffersPanel = nil
  offerHistoryPanel = nil
  marketOffers = {}
  depotItems = {}
  information = {}
  itemsPanel = nil
  nameLabel = nil
  radioItems = nil
  selectedItem = nil

  Market = nil
end

function Market.updateOffers(offers)
  for k, offer in pairs(offers) do
    if offer and offer:getAction() == MarketAction.Buy then
      table.insert(marketOffers[MarketAction.Buy], offer)
    else
      table.insert(marketOffers[MarketAction.Sell], offer)
    end
  end

  for _, offers in pairs(marketOffers) do
    for _, offer in pairs(offers) do
      print('  counter: '..offer:getCounter()..' | timestamp: '..offer:getTimeStamp()..' | item: '..offer:getItem():getId()..' | action: '..offer:getAction()..' | amount: '..offer:getAmount()..' | price: '..offer:getPrice()..' | player: '..offer:getPlayer()..' | state: '..offer:getState())
    end
  end
  -- TODO: refresh all widget windows
end

function Market.updateDetails(itemId, descriptions, purchaseStats, saleStats)
  -- TODO: refresh all widget windows

end

function Market.updateSelectedItem(newItem)
  local itemDisplay = marketWindow:recursiveGetChildById('selectedItem')
  local itemName = marketWindow:recursiveGetChildById('nameLabel')
  selectedItem = newItem
  if not table.empty(selectedItem) then
    if selectedItem.ptr then
      itemDisplay:setItem(selectedItem.ptr)
      itemName:setText(tr(selectedItem.name))
      MarketProtocol.sendMarketBrowse(selectedItem.ptr:getId()) -- send sprite id browsed
    end
  else
    itemDisplay:setItem(nil)
    itemName:setText(tr('No item selected.'))
  end
end

function Market.onMarketEnter(depotItems, offers, balance)
  -- TODO: populate market?
  if marketWindow:isVisible() then
    return
  end
  marketOffers[MarketAction.Buy] = {}
  marketOffers[MarketAction.Sell] = {}

  information.balance = balance
  information.totalOffers = offers

  loadMarketItems()
  loadDepotItems(depotItems)

  -- TODO: if you are already viewing an item on market enter it must recheck the current item
  if selectedItem and selectedItem:isChecked() then
    selectedItem:setChecked(false)
    selectedItem:setChecked(true)
  end
  --MarketProtocol.sendMarketBrowse(645)
  marketWindow:show()
end

function Market.onMarketLeave()
  marketWindow:hide()
end

function Market.onMarketDetail(itemId, descriptions, purchaseStats, saleStats)
  Market.updateDetails(itemId, descriptions, purchaseStats, saleStats)

  print('')
  print('[onMarketDetail]')
  print('itemId: '..itemId)
  print('descriptions:')
  for k, desc in pairs(descriptions) do
    print('  type: '..desc[1]..' | description: '..desc[2])
  end
  print('purchaseStats:')
  for k, stat in pairs(purchaseStats) do
    print('  transactions: '..stat[1])
    print('  total price: '..stat[2])
    print('  highest price: '..stat[3])
    print('  lowest price: '..stat[4])
  end
  print('saleStats:')
  for k, stat in pairs(saleStats) do
    print('  transactions: '..stat[1])
    print('  total price: '..stat[2])
    print('  highest price: '..stat[3])
    print('  lowest price: '..stat[4])
  end
end

function Market.onMarketBrowse(offers)
  Market.updateOffers(offers)
end

function Market.onItemBoxChecked(widget)
  if widget:isChecked() then
    Market.updateSelectedItem(widget.item)
  end
end