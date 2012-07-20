Market = {}

g_ui.importStyle('market.otui')
g_ui.importStyle('ui/general/markettabs.otui')
g_ui.importStyle('ui/general/marketbuttons.otui')
g_ui.importStyle('ui/general/marketcombobox.otui')

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
local marketItems = {}
local depot = {}
local information ={}
local selectedItem
local nameLabel

local currentItems = {}
local itemsPanel
local radioItemSet
local filterBox

local function getMarketCategoryName(category)
  if table.hasKey(MarketCategoryStrings, category) then
    return MarketCategoryStrings[category]
  end
end

local function getMarketCategoryId(name)
  local id = table.find(MarketCategoryStrings, name)
  if id then
    return id
  end
end

local function clearSelectedItem()
  if selectedItem and selectedItem.item.ptr then
    Market.updateOffers({})
    radioItemSet:selectWidget(nil)
    nameLabel:clearText()
    selectedItem:setItem(nil)
    selectedItem.item = {}
  end
end

local function initMarketItems()
  -- populate all market items
  marketItems = {}
  local types = g_things.findThingTypeByAttr(ThingAttrMarket)
  for idx = 1, #types do
    local t = types[idx]
    local newItem = Item.create(t:getId())
    if newItem then
      local item = {
        ptr = newItem,
        marketData = t:getMarketData()
      }
      table.insert(marketItems, item)
    end
  end
end

local function updateItemsWidget()
  if table.empty(currentItems) then
    return
  end

  itemsPanel = marketWindow:recursiveGetChildById('itemsPanel')
  local layout = itemsPanel:getLayout()
  layout:disableUpdates()

  clearSelectedItem()
  itemsPanel:destroyChildren()

  if radioItemSet then
    radioItemSet:destroy()
  end
  radioItemSet = UIRadioGroup.create()

  for _, item in pairs(currentItems) do
    local itemBox = g_ui.createWidget('MarketItemBox', itemsPanel)
    local itemWidget = itemBox:getChildById('item')

    itemBox.item = item
    itemWidget:setItem(item.ptr)

    radioItemSet:addWidget(itemBox)
  end

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

  initMarketItems()

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

  nameLabel = marketWindow:recursiveGetChildById('nameLabel')
  selectedItem = marketWindow:recursiveGetChildById('selectedItem')
  selectedItem.item = {}

  filterBox = browsePanel:getChildById('filterComboBox')
  for i = MarketCategory.First, MarketCategory.Last do
    filterBox:addOption(getMarketCategoryName(i))
  end
  filterBox:setCurrentOption(getMarketCategoryName(MarketCategory.First))

  filterBox.onOptionChange = function(combobox, option)
    Market.loadMarketItems(getMarketCategoryId(option))
  end
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
  marketItems = {}
  depotItems = {}
  information = {}
  currentItems = {}
  itemsPanel = nil
  nameLabel = nil
  radioItemSet = nil
  selectedItem = nil
  filterBox = nil

  Market = nil
end

function Market.loadMarketItems(_category)
  if table.empty(marketItems) then
    initMarketItems()
  end

  currentItems = {}
  for _, item in pairs(marketItems) do
    -- filter items here
    local category = item.marketData.category
    if category == _category or _category == MarketCategory[0] then
      table.insert(currentItems, item)
    end
  end

  updateItemsWidget()
end

function Market.updateOffers(offers)
  marketOffers[MarketAction.Buy] = {}
  marketOffers[MarketAction.Sell] = {}

  local buyOfferList = marketWindow:recursiveGetChildById('buyingList')
  buyOfferList:destroyChildren()

  local sellOfferList = marketWindow:recursiveGetChildById('sellingList')
  sellOfferList:destroyChildren()

  for k, offer in pairs(offers) do
    if offer and offer:getAction() == MarketAction.Buy then
      local label = g_ui.createWidget('OfferListLabel', buyOfferList)
      label:setText(offer:getPlayer()..'                  '..offer:getAmount()..'                  '..(offer:getPrice()*offer:getAmount())..'                  '..offer:getPrice()..'                  '..offer:getTimeStamp())
      table.insert(marketOffers[MarketAction.Buy], offer)
    else
      local label = g_ui.createWidget('OfferListLabel', sellOfferList)
      label:setText(offer:getPlayer()..'                  '..offer:getAmount()..'                  '..(offer:getPrice()*offer:getAmount())..'                  '..offer:getPrice()..'                  '..offer:getTimeStamp())
      table.insert(marketOffers[MarketAction.Sell], offer)
    end
  end
end

function Market.updateDetails(itemId, descriptions, purchaseStats, saleStats)
  if not selectedItem then
    return
  end
  selectedItem.item.details = {
    serverItemId = itemId,
    descriptions = descriptions,
    purchaseStats = purchaseStats,
    saleStats = saleStats
  }

  -- TODO: refresh all widget windows
end

function Market.updateSelectedItem(newItem)
  selectedItem.item = newItem
  if selectedItem and not table.empty(selectedItem.item) then
    if selectedItem.item.ptr then
      selectedItem:setItem(selectedItem.item.ptr)
      nameLabel:setText(selectedItem.item.marketData.name)
      MarketProtocol.sendMarketBrowse(selectedItem.item.ptr:getId()) -- send sprite id browsed
    end
  else
    selectedItem:setItem(nil)
    nameLabel:setText(tr('No item selected.'))
  end
end

function Market.onMarketEnter(depotItems, offers, balance)
  if marketWindow:isVisible() then
    return
  end
  marketWindow:lock()
  marketOffers[MarketAction.Buy] = {}
  marketOffers[MarketAction.Sell] = {}

  information.balance = balance
  information.totalOffers = offers

  if table.empty(currentItems) then
    Market.loadMarketItems(MarketCategory.First)
  end
  loadDepotItems(depotItems)

  -- TODO: if you are already viewing an item on market enter it must recheck the current item
  if selectedItem and selectedItem:isChecked() then
    selectedItem:setChecked(false)
    selectedItem:setChecked(true)
  end
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
