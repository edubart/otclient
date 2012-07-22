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
local buyOfferTable
local sellOfferTable
local detailsTable
local buyStatsTable
local sellStatsTable

local currentItems = {}
local itemsPanel
local radioItemSet
local filterBox

local offerTableHeader = {
    {['text'] = 'Player Name', ['width'] = 100},
    {['text'] = 'Amount', ['width'] = 60},
    {['text'] = 'Total Price', ['width'] = 90},
    {['text'] = 'Peice Price', ['width'] = 80},
    {['text'] = 'Ends at', ['width'] = 120}
  }

local function getMarketCategoryName(id)
  if table.hasKey(MarketCategoryStrings, id) then
    return MarketCategoryStrings[id]
  end
end

local function getMarketCategoryId(name)
  local id = table.find(MarketCategoryStrings, name)
  if id then
    return id
  end
end

local function getMarketDescriptionName(id)
  if table.hasKey(MarketItemDescriptionStrings, id) then
    return MarketItemDescriptionStrings[id]
  end
end

local function getMarketDescriptionId(name)
  local id = table.find(MarketItemDescriptionStrings, name)
  if id then
    return id
  end
end

local function clearSelectedItem()
  if selectedItem and selectedItem.item.ptr then
    Market.updateOffers({})
    radioItemSet:selectWidget(nil)
    nameLabel:setText('No item selected.')

    selectedItem:setItem(nil)
    selectedItem.item = {}

    detailsTable:clearData()
    buyStatsTable:clearData()
    sellStatsTable:clearData()
  end
end

local function initMarketItems()
  -- populate all market items
  marketItems = {}
  local types = g_things.findThingTypeByAttr(ThingAttrMarket)
  for i = 1, #types do
    local t = types[i]
    local newItem = Item.create(t:getId())
    if newItem then
      local item = {
        ptr = newItem,
        marketData = t:getMarketData()
      }
      marketItems[#marketItems+1] = item
    end
  end
end

local function updateItemsWidget()
  if table.empty(currentItems) then
    return
  end

  itemsPanel = browsePanel:recursiveGetChildById('itemsPanel')
  local layout = itemsPanel:getLayout()
  layout:disableUpdates()

  clearSelectedItem()
  itemsPanel:destroyChildren()

  if radioItemSet then
    radioItemSet:destroy()
  end
  radioItemSet = UIRadioGroup.create()

  for i = 1, #currentItems do
    local item = currentItems[i]
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
  for i = 1, #depotItems do
    local data = depotItems[i]
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

local function initInterface()
  -- TODO: clean this up
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

  -- setup selected item
  nameLabel = marketOffersPanel:recursiveGetChildById('nameLabel')
  selectedItem = marketOffersPanel:recursiveGetChildById('selectedItem')
  selectedItem.item = {}

  -- populate filter combo box
  filterBox = browsePanel:getChildById('filterComboBox')
  for i = MarketCategory.First, MarketCategory.Last do
    filterBox:addOption(getMarketCategoryName(i))
  end
  filterBox:setCurrentOption(getMarketCategoryName(MarketCategory.First))

  filterBox.onOptionChange = function(combobox, option)
    Market.loadMarketItems(getMarketCategoryId(option))
  end

  -- get tables
  buyOfferTable = itemOffersPanel:recursiveGetChildById('buyingTable')
  sellOfferTable = itemOffersPanel:recursiveGetChildById('sellingTable')
  detailsTable = itemDetailsPanel:recursiveGetChildById('detailsTable')
  buyStatsTable = itemStatsPanel:recursiveGetChildById('buyStatsTable')
  sellStatsTable = itemStatsPanel:recursiveGetChildById('sellStatsTable')
end

function Market.init()
  marketWindow = g_ui.createWidget('MarketWindow', rootWidget)
  marketWindow:hide()

  initInterface() -- build interface
  initMarketItems()
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
  buyOfferTable = nil
  sellOfferTable = nil
  detailsTable = nil
  buyStatsTable = nil
  sellStatsTable = nil
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
  for i = 1, #marketItems do
    -- filter items here
    local item = marketItems[i]
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
  if not buyOfferTable or not sellOfferTable then
    return
  end
  buyOfferTable:clearData()
  sellOfferTable:clearData()

  for k, offer in pairs(offers) do
    if offer and offer:getAction() == MarketAction.Buy then
      local data = {
        {['text'] = offer:getPlayer(), ['width'] = 100},
        {['text'] = offer:getAmount(), ['width'] = 60},
        {['text'] = offer:getPrice()*offer:getAmount(), ['width'] = 90},
        {['text'] = offer:getPrice(), ['width'] = 80},
        {['text'] = offer:getTimeStamp(), ['width'] = 120}
      }
      buyOfferTable:addRow(data)
      table.insert(marketOffers[MarketAction.Buy], offer)
    else
      local data = {
        {['text'] = offer:getPlayer(), ['width'] = 100},
        {['text'] = offer:getAmount(), ['width'] = 60},
        {['text'] = offer:getPrice()*offer:getAmount(), ['width'] = 90},
        {['text'] = offer:getPrice(), ['width'] = 80},
        {['text'] = offer:getTimeStamp(), ['width'] = 120}
      }
      sellOfferTable:addRow(data)
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

  -- update item details
  detailsTable:clearData()
  for k, desc in pairs(descriptions) do
    local columns = {
      {['text'] = getMarketDescriptionName(desc[1])..':', ['width'] = 100},
      {['text'] = desc[2], ['width'] = 330}
    }
    detailsTable:addRow(columns)
  end

  -- update sale item statistics
  sellStatsTable:clearData()
  if table.empty(saleStats) then
    sellStatsTable:addRow({{['text'] = 'No information', ['width'] = 110}})
  else
    for k, stat in pairs(saleStats) do
      if not table.empty(stat) then
        sellStatsTable:addRow({{['text'] = 'Total Transations:', ['width'] = 110}, 
          {['text'] = stat[1], ['width'] = 270}})

        sellStatsTable:addRow({{['text'] = 'Highest Price:', ['width'] = 110}, 
          {['text'] = stat[3], ['width'] = 270}})

        print(stat[2] .. '/' ..stat[1])
        sellStatsTable:addRow({{['text'] = 'Average Price:', ['width'] = 110}, 
          {['text'] = math.floor(stat[2]/stat[1]), ['width'] = 270}})

        sellStatsTable:addRow({{['text'] = 'Lowest Price:', ['width'] = 110}, 
          {['text'] = stat[4], ['width'] = 270}})
      end
    end
  end

  -- update buy item statistics
  buyStatsTable:clearData()
  if table.empty(purchaseStats) then
    buyStatsTable:addRow({{['text'] = 'No information', ['width'] = 110}})
  else
    for k, stat in pairs(purchaseStats) do
      if not table.empty(stat) then
        buyStatsTable:addRow({{['text'] = 'Total Transations:', ['width'] = 110}, 
          {['text'] = stat[1], ['width'] = 270}})

        buyStatsTable:addRow({{['text'] = 'Highest Price:', ['width'] = 110}, 
          {['text'] = stat[3], ['width'] = 270}})

        print(stat[2] .. '/' ..stat[1])
        buyStatsTable:addRow({{['text'] = 'Average Price:', ['width'] = 110}, 
          {['text'] = math.floor(stat[2]/stat[1]), ['width'] = 270}})

        buyStatsTable:addRow({{['text'] = 'Lowest Price:', ['width'] = 110}, 
          {['text'] = stat[4], ['width'] = 270}})
      end
    end
  end
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

  -- build offer table header
  if buyOfferTable and not buyOfferTable:hasHeader() then
    buyOfferTable:addHeaderRow(offerTableHeader)
  end
  if sellOfferTable and not sellOfferTable:hasHeader() then
    sellOfferTable:addHeaderRow(offerTableHeader)
  end

  local currentItem = radioItemSet:getSelectedWidget()
  if currentItem then
     -- Uncheck selected item, cannot make protocol calls to resend marketBrowsing
    clearSelectedItem()
  end
  marketWindow:show()
end

function Market.onMarketLeave()
  marketWindow:hide()
end

function Market.onMarketDetail(itemId, descriptions, purchaseStats, saleStats)
  Market.updateDetails(itemId, descriptions, purchaseStats, saleStats)
end

function Market.onMarketBrowse(offers)
  Market.updateOffers(offers)
end

function Market.onItemBoxChecked(widget)
  if widget:isChecked() then
    Market.updateSelectedItem(widget.item)
  end
end
