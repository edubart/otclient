Market = {}

g_ui.importStyle('market.otui')
g_ui.importStyle('ui/general/markettabs.otui')
g_ui.importStyle('ui/general/marketbuttons.otui')
g_ui.importStyle('ui/general/marketcombobox.otui')

local marketWindow
local mainTabBar

local marketOffersPanel
local selectionTabBar
local displaysTabBar
local offersTabBar

local itemsPanel
local browsePanel
local searchPanel
local itemOffersPanel
local itemDetailsPanel
local itemStatsPanel
local myOffersPanel
local currentOffersPanel
local offerHistoryPanel

local selectedItem
local nameLabel
local radioItemSet
local categoryList
local subCategoryList
local slotFilterList
local filterButtons = {}

local buyOfferTable
local sellOfferTable
local detailsTable
local buyStatsTable
local sellStatsTable

local marketOffers = {}
local marketItems = {}
local depot = {}
local information = {}
local currentItems = {}

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

local function getMarketSlotFilterId(name)
  local id = table.find(MarketSlotFilters, name)
  if id then
    return id
  end
end

local function getMarketSlotFilterName(id)
  if table.hasKey(MarketSlotFilters, id) then
    return MarketSlotFilters[id]
  end
end

local function isValidItem(item, category)
  if item.marketData.category ~= category and category ~= MarketCategory[0] then
    return false
  end

  -- filter item
  local slotFilter = false
  if slotFilterList:isEnabled() then
    slotFilter = getMarketSlotFilterId(slotFilterList:getCurrentOption().text)
  end
  local marketData = item.marketData

  local filterVocation = filterButtons[MarketFilters.Vocation]:isChecked()
  local filterLevel = filterButtons[MarketFilters.Level]:isChecked()
  local filterDepot = filterButtons[MarketFilters.Depot]:isChecked()

  if slotFilter then
    if slotFilter ~= 255 and item.ptr:getClothSlot() ~= slotFilter then
      return false
    end
  end
  local player = g_game.getLocalPlayer()
  if filterLevel and marketData.requiredLevel and player:getLevel() < marketData.requiredLevel then
    return false
  end
  if filterVocation and marketData.restrictVocation > 0 then
    local voc = Bit.bit(information.vocation)
    if not Bit.hasBit(marketData.restrictVocation, voc) then
      return false
    end
  end
  if filterDepot and not Market.depotContains(item.ptr:getId()) then
    return false
  end
  return true
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
      local marketData = t:getMarketData()
      if not table.empty(marketData) then
        local item = {
          ptr = newItem,
          marketData = marketData
        }
        marketItems[#marketItems+1] = item
      end
    end
  end
end

local function updateItemsWidget()
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

local function onUpdateCategory(combobox, option)
  local id = getMarketCategoryId(option)
  if id == MarketCategory.MetaWeapons then
    -- enable and load weapons filter/items
    subCategoryList:setEnabled(true)
    slotFilterList:setEnabled(true)
    local subId = getMarketCategoryId(subCategoryList:getCurrentOption().text)
    Market.loadMarketItems(subId)
  else
    subCategoryList:setEnabled(false)
    slotFilterList:setEnabled(false)
    Market.loadMarketItems(id) -- load standard filter
  end
end

local function onUpdateSubCategory(combobox, option)
  local id = getMarketCategoryId(option)
  Market.loadMarketItems(id)
  -- setup slot filter
  slotFilterList:clearOptions()
  local subId = getMarketCategoryId(subCategoryList:getCurrentOption().text)
  local slots = MarketCategoryWeapons[subId].slots
  for _, slot in pairs(slots) do
    if table.hasKey(MarketSlotFilters, slot) then
      slotFilterList:addOption(MarketSlotFilters[slot])
    end
  end
  slotFilterList:setEnabled(true)
end

local function onUpdateSlotFilter(combobox, option)
  Market.updateCurrentItems()
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

  -- setup filters
  filterButtons[MarketFilters.Vocation] = browsePanel:getChildById('filterVocation')
  filterButtons[MarketFilters.Level] = browsePanel:getChildById('filterLevel')
  filterButtons[MarketFilters.Depot] = browsePanel:getChildById('filterDepot')

  categoryList = browsePanel:getChildById('categoryComboBox')
  subCategoryList = browsePanel:getChildById('subCategoryComboBox')
  slotFilterList = browsePanel:getChildById('typeComboBox')

  slotFilterList:addOption(MarketSlotFilters[255])
  slotFilterList:setEnabled(false)

  for i = MarketCategory.First, MarketCategory.Last do
    if i >= MarketCategory.Ammunition and i <= MarketCategory.WandsRods then
      subCategoryList:addOption(getMarketCategoryName(i))
    else
      categoryList:addOption(getMarketCategoryName(i))
    end
  end
  categoryList:addOption(getMarketCategoryName(255)) -- meta weapons
  categoryList:setCurrentOption(getMarketCategoryName(MarketCategory.First))
  subCategoryList:setEnabled(false)
  
  -- hook item filters
  categoryList.onOptionChange = onUpdateCategory
  subCategoryList.onOptionChange = onUpdateSubCategory
  slotFilterList.onOptionChange = onUpdateSlotFilter

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
  displaysTabBar = nil
  offersTabBar = nil
  selectionTabBar = nil

  marketOffersPanel = nil
  browsePanel = nil
  searchPanel = nil
  itemOffersPanel = nil
  itemDetailsPanel = nil
  itemStatsPanel = nil
  myOffersPanel = nil
  currentOffersPanel = nil
  offerHistoryPanel = nil
  itemsPanel = nil

  nameLabel = nil
  radioItemSet = nil
  selectedItem = nil
  categoryList = nil
  subCategoryList = nil
  slotFilterList = nil
  filterButtons = {}

  buyOfferTable = nil
  sellOfferTable = nil
  detailsTable = nil
  buyStatsTable = nil
  sellStatsTable = nil

  marketOffers = {}
  marketItems = {}
  information = {}
  currentItems = {}
  
  Market = nil
end

function Market.depotContains(itemId)
  for i = 1, #information.depotItems do
    local item = information.depotItems[i]
    if item.ptr:getId() == itemId then
      return true
    end
  end
  return false
end

function Market.loadMarketItems(category)
  if table.empty(marketItems) then
    initMarketItems()
  end

  currentItems = {}
  for i = 1, #marketItems do
    local item = marketItems[i]
    if isValidItem(item, category) then
      table.insert(currentItems, item)
    end
  end

  updateItemsWidget()
end

function Market.loadDepotItems(depotItems)
  information.depotItems = {}
  for i = 1, #depotItems do
    local data = depotItems[i]
    local newItem = Item.create(data[1])
    if not newItem then
      break
    end
    newItem:setCount(data[2])
    local marketData = newItem:getMarketData()

    if not table.empty(marketData) then
      local item = {
        ptr = newItem,
        marketData = marketData
      }
      table.insert(information.depotItems, item)
    end
  end
end

function Market.updateCurrentItems()
  -- get market category
  local id = getMarketCategoryId(categoryList:getCurrentOption().text)
  if id == MarketCategory.MetaWeapons then
    id = getMarketCategoryId(subCategoryList:getCurrentOption().text)
  end
  Market.loadMarketItems(id)
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
    descriptions = descriptions,
    purchaseStats = purchaseStats,
    saleStats = saleStats
  }

  -- update item details
  detailsTable:clearData()
  for k, desc in pairs(descriptions) do
    local columns = {
      {['text'] = getMarketDescriptionName(desc[1])..':'},
      {['text'] = desc[2], ['width'] = 330}
    }
    detailsTable:addRow(columns)
  end

  -- update sale item statistics
  sellStatsTable:clearData()
  if table.empty(saleStats) then
    sellStatsTable:addRow({{['text'] = 'No information'}})
  else
    for k, stat in pairs(saleStats) do
      if not table.empty(stat) then
        sellStatsTable:addRow({{['text'] = 'Total Transations:'}, 
          {['text'] = stat[1], ['width'] = 270}})

        sellStatsTable:addRow({{['text'] = 'Highest Price:'}, 
          {['text'] = stat[3], ['width'] = 270}})

        sellStatsTable:addRow({{['text'] = 'Average Price:'}, 
          {['text'] = math.floor(stat[2]/stat[1])}})

        sellStatsTable:addRow({{['text'] = 'Lowest Price:'}, 
          {['text'] = stat[4], ['width'] = 270}})
      end
    end
  end

  -- update buy item statistics
  buyStatsTable:clearData()
  if table.empty(purchaseStats) then
    buyStatsTable:addRow({{['text'] = 'No information'}})
  else
    for k, stat in pairs(purchaseStats) do
      if not table.empty(stat) then
        buyStatsTable:addRow({{['text'] = 'Total Transations:'}, 
          {['text'] = stat[1], ['width'] = 270}})

        buyStatsTable:addRow({{['text'] = 'Highest Price:'}, 
          {['text'] = stat[3], ['width'] = 270}})

        buyStatsTable:addRow({{['text'] = 'Average Price:'}, 
          {['text'] = math.floor(stat[2]/stat[1]), ['width'] = 270}})

        buyStatsTable:addRow({{['text'] = 'Lowest Price:'}, 
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
      MarketProtocol.sendMarketBrowse(selectedItem.item.ptr:getId()) -- send browsed msg
    end
  else
    selectedItem:setItem(nil)
    nameLabel:setText(tr('No item selected.'))
  end
end

function Market.onMarketEnter(depotItems, offers, balance, vocation)
  if marketWindow:isVisible() then
    return
  end
  marketOffers[MarketAction.Buy] = {}
  marketOffers[MarketAction.Sell] = {}

  --[[
    TODO: 
      * clear filters on enter
      * add market reset function
  ]]
  information.balance = balance
  information.totalOffers = offers
  if vocation < 0 then
    local player = g_game.getLocalPlayer()
    if player then information.vocation = player:getVocation() end
  else
    -- vocation must be compatible with < 950
    information.vocation = vocation
  end

  if table.empty(currentItems) then
    Market.loadMarketItems(MarketCategory.First)
  end
  Market.loadDepotItems(depotItems)

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

  marketWindow:lock()
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
