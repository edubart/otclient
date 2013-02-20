--[[
    Finalizing Market:
      Note: Feel free to work on any area and submit 
            it as a pull request from your git fork.

      BeniS's Skype: benjiz69

      List:
      * Add offer management:
        - Current Offers
        - Offer History

      * Clean up the interface building
        - Add a new market interface file to handle building?      

      * Add offer table column ordering.
        - Player Name, Amount, Total Price, Peice Price and Ends At

      * Extend information features
        - Hover over offers for purchase information (balance after transaction, etc)
        - Display out of trend market offers based on their previous statistics (like cipsoft does)
  ]]

Market = {}

local protocol = runinsandbox('marketprotocol')

marketWindow = nil
mainTabBar = nil
displaysTabBar = nil
offersTabBar = nil
selectionTabBar = nil

marketOffersPanel = nil
browsePanel = nil
overviewPanel = nil
itemOffersPanel = nil
itemDetailsPanel = nil
itemStatsPanel = nil
myOffersPanel = nil
currentOffersPanel = nil
offerHistoryPanel = nil
itemsPanel = nil
selectedOffer = {}

nameLabel = nil
feeLabel = nil
balanceLabel = nil
totalPriceEdit = nil
piecePriceEdit = nil
amountEdit = nil
searchEdit = nil
radioItemSet = nil
selectedItem = nil
offerTypeList = nil
categoryList = nil
subCategoryList = nil
slotFilterList = nil
createOfferButton = nil
buyButton = nil
sellButton = nil
anonymous = nil
filterButtons = {}

buyOfferTable = nil
sellOfferTable = nil
detailsTable = nil
buyStatsTable = nil
sellStatsTable = nil

offerExhaust = {}
marketOffers = {}
marketItems = {}
information = {}
currentItems = {}
lastCreatedOffer = 0
fee = 0

loaded = false

local offerTableHeader = {
  {['text'] = 'Player Name', ['width'] = 100},
  {['text'] = 'Amount', ['width'] = 60},
  {['text'] = 'Total Price', ['width'] = 90},
  {['text'] = 'Piece Price', ['width'] = 80},
  {['text'] = 'Ends at', ['width'] = 120}
}

local function isItemValid(item, category, searchFilter)
  if not item or not item.marketData then
    return false
  end

  if not category then
    category = MarketCategory.All
  end
  if item.marketData.category ~= category and category ~= MarketCategory.All then
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
  if filterDepot and Market.depotContains(item.ptr:getId()) <= 0 then
    return false
  end
  if searchFilter then
    return marketData.name:lower():find(searchFilter)
  end
  return true
end

local function clearItems()
  currentItems = {}
  Market.refreshItemsWidget()
end

local function clearOffers()
  marketOffers[MarketAction.Buy] = {}
  marketOffers[MarketAction.Sell] = {}
  buyOfferTable:clearData()
  sellOfferTable:clearData()
end

local function clearFilters()
  for _, filter in pairs(filterButtons) do
    if filter and filter:isChecked() then
      filter:setChecked(false)
    end
  end
end

local function clearFee()
  feeLabel:setText('')
  fee = 0
end

local function refreshTypeList()
  offerTypeList:clearOptions()
  offerTypeList:addOption('Buy')

  if Market.isItemSelected() then
    if Market.depotContains(selectedItem.item.ptr:getId()) > 0 then
      offerTypeList:addOption('Sell')
    end
  end
end

local function addOffer(offer, type)
  if not offer then
    return false
  end
  local id = offer:getId()
  local player = offer:getPlayer()
  local amount = offer:getAmount()
  local price = offer:getPrice()
  local timestamp = offer:getTimeStamp()

  if amount < 1 then return false end
  if type == MarketAction.Buy then
    local data = {
      {['text'] = player, ['width'] = 100},
      {['text'] = amount, ['width'] = 60},
      {['text'] = price*amount, ['width'] = 90},
      {['text'] = price, ['width'] = 80},
      {['text'] = string.gsub(os.date('%c', timestamp), " ", "  "), ['width'] = 120}
    }
    buyOfferTable:addRow(data, id)
  else
    local data = {
      {['text'] = player, ['width'] = 100},
      {['text'] = amount, ['width'] = 60},
      {['text'] = price*amount, ['width'] = 90},
      {['text'] = price, ['width'] = 80},
      {['text'] = string.gsub(os.date('%c', timestamp), " ", "  "), ['width'] = 120}
    }
    sellOfferTable:addRow(data, id)
  end
  return true
end

local function mergeOffer(offer)
  if not offer then
    return false
  end
  local id = offer:getId()
  local type = offer:getType()
  local amount = offer:getAmount()
  local replaced = false

  if type == MarketAction.Buy then
    for i = 1, #marketOffers[MarketAction.Buy] do
      local o = marketOffers[MarketAction.Buy][i]
      -- replace existing offer
      if o:isEqual(id) then
        marketOffers[MarketAction.Buy][i] = offer
        replaced = true
      end
    end
    if not replaced then
      table.insert(marketOffers[MarketAction.Buy], offer)
    end
  else
    for i = 1, #marketOffers[MarketAction.Sell] do
      local o = marketOffers[MarketAction.Sell][i]
      -- replace existing offer
      if o:isEqual(id) then
        marketOffers[MarketAction.Sell][i] = offer
        replaced = true
      end
    end
    if not replaced then
      table.insert(marketOffers[MarketAction.Sell], offer)
    end
  end
  return true
end

local function updateOffers(offers)
  if not buyOfferTable or not sellOfferTable then
    return
  end

  balanceLabel:setColor('#bbbbbb')
  selectedOffer[MarketAction.Buy] = nil
  selectedOffer[MarketAction.Sell] = nil

  -- clear existing offer data
  buyOfferTable:clearData()
  sellOfferTable:clearData()
  
  sellButton:setEnabled(false)
  buyButton:setEnabled(false)

  for _, offer in pairs(offers) do
    mergeOffer(offer)
  end
  for type, offers in pairs(marketOffers) do
    for i = 1, #offers do
      addOffer(offers[i], type)
    end
  end
end

local function updateDetails(itemId, descriptions, purchaseStats, saleStats)
  if not selectedItem then
    return
  end

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
    local transactions, totalPrice, highestPrice, lowestPrice = 0, 0, 0, 0
    for _, stat in pairs(saleStats) do
      if not stat:isNull() then
        transactions = transactions + stat:getTransactions()
        totalPrice = totalPrice + stat:getTotalPrice()
        local newHigh = stat:getHighestPrice()
        if newHigh > highestPrice then
          highestPrice = newHigh
        end
        local newLow = stat:getLowestPrice()
        -- ?? getting '0xffffffff' result from lowest price in 9.60 cipsoft
        if (lowestPrice == 0 or newLow < lowestPrice) and newLow ~= 0xffffffff then
          lowestPrice = newLow
        end
      end
    end
    sellStatsTable:addRow({{['text'] = 'Total Transations:'},
      {['text'] = transactions, ['width'] = 270}})

    sellStatsTable:addRow({{['text'] = 'Highest Price:'},
      {['text'] = highestPrice, ['width'] = 270}})

    if totalPrice > 0 and transactions > 0 then
      sellStatsTable:addRow({{['text'] = 'Average Price:'},
        {['text'] = math.floor(totalPrice/transactions), ['width'] = 270}})
    else
      sellStatsTable:addRow({{['text'] = 'Average Price:'},
        {['text'] = 0, ['width'] = 270}})
    end

    sellStatsTable:addRow({{['text'] = 'Lowest Price:'},
      {['text'] = lowestPrice, ['width'] = 270}})
  end

  -- update buy item statistics
  buyStatsTable:clearData()
  if table.empty(purchaseStats) then
    buyStatsTable:addRow({{['text'] = 'No information'}})
  else
    local transactions, totalPrice, highestPrice, lowestPrice = 0, 0, 0, 0
    for _, stat in pairs(purchaseStats) do
      if not stat:isNull() then
        transactions = transactions + stat:getTransactions()
        totalPrice = totalPrice + stat:getTotalPrice()
        local newHigh = stat:getHighestPrice()
        if newHigh > highestPrice then
          highestPrice = newHigh
        end
        local newLow = stat:getLowestPrice()
        -- ?? getting '0xffffffff' result from lowest price in 9.60 cipsoft
        if (lowestPrice == 0 or newLow < lowestPrice) and newLow ~= 0xffffffff then
          lowestPrice = newLow
        end
      end
    end
    buyStatsTable:addRow({{['text'] = 'Total Transations:'},
      {['text'] = transactions, ['width'] = 270}})

    buyStatsTable:addRow({{['text'] = 'Highest Price:'},
      {['text'] = highestPrice, ['width'] = 270}})

    if totalPrice > 0 and transactions > 0 then
      buyStatsTable:addRow({{['text'] = 'Average Price:'},
        {['text'] = math.floor(totalPrice/transactions), ['width'] = 270}})
    else
      buyStatsTable:addRow({{['text'] = 'Average Price:'},
        {['text'] = 0, ['width'] = 270}})
    end

    buyStatsTable:addRow({{['text'] = 'Lowest Price:'},
      {['text'] = lowestPrice, ['width'] = 270}})
  end
end

local function updateSelectedItem(widget)
  selectedItem.item = widget.item
  selectedItem.ref = widget

  Market.resetCreateOffer()
  if Market.isItemSelected() then
    selectedItem:setItem(selectedItem.item.ptr)
    nameLabel:setText(selectedItem.item.marketData.name)
    clearOffers()

    Market.enableCreateOffer(true) -- update offer types
    MarketProtocol.sendMarketBrowse(selectedItem.item.ptr:getId()) -- send browsed msg
  else
    Market.clearSelectedItem()
  end
end

local function updateBalance(balance)
  local balance = tonumber(balance)
  if not balance then
    return
  end

  if balance < 0 then balance = 0 end
  information.balance = balance

  balanceLabel:setText('Balance: '..balance..' gold')
  balanceLabel:resizeToText()
end

local function updateFee(price, amount)
  fee = math.ceil(price / 100 * amount)
  if fee < 20 then
    fee = 20
  elseif fee > 1000 then
    fee = 1000
  end
  feeLabel:setText('Fee: '..fee)
  feeLabel:resizeToText()
end

local function openAmountWindow(callback, type, actionText)
  local actionText = actionText or ''
  if not Market.isOfferSelected(type) then
    return
  end
  amountWindow = g_ui.createWidget('AmountWindow', rootWidget)
  amountWindow:lock()
  local item = selectedOffer[type]:getItem()

  local max = selectedOffer[type]:getAmount(item:getId())
  if type == MarketAction.Sell then
    local depot = Market.depotContains(item:getId())
    if max > depot then
      max = depot
    end
  end

  local itembox = amountWindow:getChildById('item')
  itembox:setItemId(item:getId())
  itembox:setText(1)

  local scrollbar = amountWindow:getChildById('amountScrollBar')
  scrollbar:setText(tostring(selectedOffer[type]:getPrice())..'gp')
  scrollbar:setMaximum(max)
  scrollbar:setMinimum(1)
  scrollbar:setValue(1)

  scrollbar.onValueChange = function(widget, value)
    widget:setText(tostring(value*selectedOffer[type]:getPrice())..'gp')
    itembox:setText(tostring(value))
  end
  
  local okButton = amountWindow:getChildById('buttonOk')
  if actionText ~= '' then
    okButton:setText(actionText)
  end

  local okFunc = function()
    local counter = selectedOffer[type]:getCounter()
    local timestamp = selectedOffer[type]:getTimeStamp()
    callback(scrollbar:getValue(), timestamp, counter)
    okButton:getParent():destroy()
    amountWindow = nil
  end

  local cancelButton = amountWindow:getChildById('buttonCancel')
  local cancelFunc = function()
    cancelButton:getParent():destroy()
    amountWindow = nil
  end

  amountWindow.onEnter = okFunc
  amountWindow.onEscape = cancelFunc

  okButton.onClick = okFunc
  cancelButton.onClick = cancelFunc
end

local function onSelectSellOffer(table, selectedRow, previousSelectedRow)
  updateBalance()
  for _, offer in pairs(marketOffers[MarketAction.Sell]) do
    if offer:isEqual(selectedRow.ref) then
      selectedOffer[MarketAction.Buy] = offer
    end
  end

  local offer = selectedOffer[MarketAction.Buy]
  if offer then
    local price = offer:getPrice()
    if price > information.balance then
      balanceLabel:setColor('#b22222') -- red
      buyButton:setEnabled(false)
    else
      local slice = (information.balance / 2)
      if (price/slice) * 100 <= 40 then
        color = '#008b00' -- green
      elseif (price/slice) * 100 <= 70 then
        color = '#eec900' -- yellow
      else
        color = '#ee9a00' -- orange
      end
      balanceLabel:setColor(color)
      buyButton:setEnabled(true)
    end
  end
end

local function onSelectBuyOffer(table, selectedRow, previousSelectedRow)
  updateBalance()
  for _, offer in pairs(marketOffers[MarketAction.Buy]) do
    if offer:isEqual(selectedRow.ref) then
      selectedOffer[MarketAction.Sell] = offer
      if Market.depotContains(offer:getItem():getId()) > 0 then
        sellButton:setEnabled(true)
      else
        sellButton:setEnabled(false)
      end
    end
  end
end

local function onChangeCategory(combobox, option)
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

local function onChangeSubCategory(combobox, option)
  Market.loadMarketItems(getMarketCategoryId(option))
  slotFilterList:clearOptions()

  local subId = getMarketCategoryId(subCategoryList:getCurrentOption().text)
  local slots = MarketCategoryWeapons[subId].slots
  for _, slot in pairs(slots) do
    if table.haskey(MarketSlotFilters, slot) then
      slotFilterList:addOption(MarketSlotFilters[slot])
    end
  end
  slotFilterList:setEnabled(true)
end

local function onChangeSlotFilter(combobox, option)
  Market.updateCurrentItems()
end

local function onChangeOfferType(combobox, option)
  local id = selectedItem.item.ptr:getId()
  if option == 'Sell' then
    local max = Market.depotContains(id)
    amountEdit:setMaximum(max)
  else
    amountEdit:setMaximum(999999)
  end
end

local function onTotalPriceChange()
  local totalPrice = totalPriceEdit:getValue()
  local piecePrice = piecePriceEdit:getValue()
  local amount = amountEdit:getValue()

  piecePriceEdit:setValue(math.floor(totalPrice/amount))
  if Market.isItemSelected() then
    updateFee(totalPrice, amount)
  end
end

local function onPiecePriceChange()
  local totalPrice = totalPriceEdit:getValue()
  local piecePrice = piecePriceEdit:getValue()
  local amount = amountEdit:getValue()

  totalPriceEdit:setValue(piecePrice*amount)
  if Market.isItemSelected() then
    updateFee(totalPrice, amount)
  end
end

local function onAmountChange()
  local totalPrice = totalPriceEdit:getValue()
  local piecePrice = piecePriceEdit:getValue()
  local amount = amountEdit:getValue()

  piecePriceEdit:setValue(math.floor(totalPrice/amount))
  totalPriceEdit:setValue(piecePrice*amount)
  if Market.isItemSelected() then
    updateFee(totalPrice, amount)
  end
end

local function initMarketItems(category)
  for c = MarketCategory.First, MarketCategory.Last do
    marketItems[c] = {}
  end

  -- populate all market items
  local types = g_things.findThingTypeByAttr(ThingAttrMarket, 0)
  for i = 1, #types do
    local t = types[i]

    local newItem = Item.create(t:getId())
    if newItem then
      local marketData = t:getMarketData()
      if not table.empty(marketData) then
        if marketData.category == category or category == MarketCategory.All then

          -- create new item block
          local item = {
            ptr = newItem,
            marketData = marketData
          }

          -- add new market item
          table.insert(marketItems[marketData.category], item)
        end
      end
    end
  end
end

local function initInterface()
  -- TODO: clean this up
  -- setup main tabs
  mainTabBar = marketWindow:getChildById('mainTabBar')
  mainTabBar:setContentWidget(marketWindow:getChildById('mainTabContent'))

  -- setup 'Market Offer' section tabs
  marketOffersPanel = g_ui.loadUI('ui/marketoffers')
  mainTabBar:addTab(tr('Market Offers'), marketOffersPanel)

  selectionTabBar = marketOffersPanel:getChildById('leftTabBar')
  selectionTabBar:setContentWidget(marketOffersPanel:getChildById('leftTabContent'))

  browsePanel = g_ui.loadUI('ui/marketoffers/browse')
  selectionTabBar:addTab(tr('Browse'), browsePanel)

  overviewPanel = g_ui.loadUI('ui/marketoffers/overview')
  selectionTabBar:addTab(tr('Overview'), overviewPanel)

  displaysTabBar = marketOffersPanel:getChildById('rightTabBar')
  displaysTabBar:setContentWidget(marketOffersPanel:getChildById('rightTabContent'))

  itemOffersPanel = g_ui.loadUI('ui/marketoffers/itemoffers')
  displaysTabBar:addTab(tr('Offers'), itemOffersPanel)

  itemDetailsPanel = g_ui.loadUI('ui/marketoffers/itemdetails')
  displaysTabBar:addTab(tr('Details'), itemDetailsPanel)

  itemStatsPanel = g_ui.loadUI('ui/marketoffers/itemstats')
  displaysTabBar:addTab(tr('Statistics'), itemStatsPanel)

  -- setup 'My Offer' section tabs
  myOffersPanel = g_ui.loadUI('ui/myoffers')
  mainTabBar:addTab(tr('My Offers'), myOffersPanel)

  offersTabBar = myOffersPanel:getChildById('offersTabBar')
  offersTabBar:setContentWidget(myOffersPanel:getChildById('offersTabContent'))

  currentOffersPanel = g_ui.loadUI('ui/myoffers/currentoffers')
  offersTabBar:addTab(tr('Current Offers'), currentOffersPanel)

  offerHistoryPanel = g_ui.loadUI('ui/myoffers/offerhistory')
  offersTabBar:addTab(tr('Offer History'), offerHistoryPanel)

  balanceLabel = marketWindow:getChildById('balanceLabel')

  -- setup offers
  buyButton = itemOffersPanel:getChildById('buyButton')
  buyButton.onClick = function() openAmountWindow(Market.acceptMarketOffer, MarketAction.Buy, 'Buy') end

  sellButton = itemOffersPanel:getChildById('sellButton')
  sellButton.onClick = function() openAmountWindow(Market.acceptMarketOffer, MarketAction.Sell, 'Sell') end

  -- setup selected item
  nameLabel = marketOffersPanel:getChildById('nameLabel')
  selectedItem = marketOffersPanel:getChildById('selectedItem')
  selectedItem.item = {}

  -- setup create new offer
  totalPriceEdit = marketOffersPanel:getChildById('totalPriceEdit')
  piecePriceEdit = marketOffersPanel:getChildById('piecePriceEdit')
  amountEdit = marketOffersPanel:getChildById('amountEdit')
  feeLabel = marketOffersPanel:getChildById('feeLabel')
  totalPriceEdit.onValueChange = onTotalPriceChange
  piecePriceEdit.onValueChange = onPiecePriceChange
  amountEdit.onValueChange = onAmountChange

  offerTypeList = marketOffersPanel:getChildById('offerTypeComboBox')
  offerTypeList.onOptionChange = onChangeOfferType

  anonymous = marketOffersPanel:getChildById('anonymousCheckBox')
  createOfferButton = marketOffersPanel:getChildById('createOfferButton')
  createOfferButton.onClick = Market.createNewOffer
  Market.enableCreateOffer(false)

  -- setup filters
  filterButtons[MarketFilters.Vocation] = browsePanel:getChildById('filterVocation')
  filterButtons[MarketFilters.Level] = browsePanel:getChildById('filterLevel')
  filterButtons[MarketFilters.Depot] = browsePanel:getChildById('filterDepot')
  filterButtons[MarketFilters.SearchAll] = browsePanel:getChildById('filterSearchAll')

  searchEdit = browsePanel:getChildById('searchEdit')
  categoryList = browsePanel:getChildById('categoryComboBox')
  subCategoryList = browsePanel:getChildById('subCategoryComboBox')
  slotFilterList = browsePanel:getChildById('slotComboBox')

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
  categoryList.onOptionChange = onChangeCategory
  subCategoryList.onOptionChange = onChangeSubCategory
  slotFilterList.onOptionChange = onChangeSlotFilter

  -- setup tables
  buyOfferTable = itemOffersPanel:recursiveGetChildById('buyingTable')
  sellOfferTable = itemOffersPanel:recursiveGetChildById('sellingTable')
  detailsTable = itemDetailsPanel:recursiveGetChildById('detailsTable')
  buyStatsTable = itemStatsPanel:recursiveGetChildById('buyStatsTable')
  sellStatsTable = itemStatsPanel:recursiveGetChildById('sellStatsTable')
  buyOfferTable.onSelectionChange = onSelectBuyOffer
  sellOfferTable.onSelectionChange = onSelectSellOffer
end

function init()
  g_ui.importStyle('market')
  g_ui.importStyle('ui/general/markettabs')
  g_ui.importStyle('ui/general/marketbuttons')
  g_ui.importStyle('ui/general/marketcombobox')
  g_ui.importStyle('ui/general/amountwindow')

  offerExhaust[MarketAction.Sell] = 10
  offerExhaust[MarketAction.Buy] = 20

  protocol.initProtocol()
  connect(g_game, { onGameEnd = Market.reset })
  connect(g_game, { onGameEnd = Market.close })
  marketWindow = g_ui.createWidget('MarketWindow', rootWidget)
  marketWindow:hide()

  initInterface() -- build interface
end

function terminate()
  protocol.terminateProtocol()
  disconnect(g_game, { onGameEnd = Market.reset })
  disconnect(g_game, { onGameEnd = Market.close })

  marketWindow:destroy()

  Market = nil
end

function Market.reset()
  balanceLabel:setColor('#bbbbbb')
  categoryList:setCurrentOption(getMarketCategoryName(MarketCategory.First))
  searchEdit:setText('')
  clearFilters()
  if not table.empty(information) then
    Market.updateCurrentItems()
  end
end

function Market.clearSelectedItem()
  if Market.isItemSelected() then
    Market.resetCreateOffer()
    offerTypeList:clearOptions()
    offerTypeList:setText('Please Select')
    offerTypeList:setEnabled(false)

    clearOffers()
    radioItemSet:selectWidget(nil)
    nameLabel:setText('No item selected.')
    selectedItem:setItem(nil)
    selectedItem.item = nil
    selectedItem.ref:setChecked(false)
    selectedItem.ref = nil

    detailsTable:clearData()
    buyStatsTable:clearData()
    sellStatsTable:clearData()

    Market.enableCreateOffer(false)
  end
end

function Market.isItemSelected()
  return selectedItem and not table.empty(selectedItem.item) and selectedItem.item.ptr
end

function Market.isOfferSelected(type)
  return selectedOffer[type] and not selectedOffer[type]:isNull()
end

function Market.depotContains(itemId)
  local count = 0
  for i = 1, #information.depotItems do
    local item = information.depotItems[i]
    if item and item.ptr:getId() == itemId then
      count = count + item.ptr:getCount()
    end
  end
  return count
end

function Market.enableCreateOffer(enable)
  offerTypeList:setEnabled(enable)
  totalPriceEdit:setEnabled(enable)
  piecePriceEdit:setEnabled(enable)
  amountEdit:setEnabled(enable)
  anonymous:setEnabled(enable)
  createOfferButton:setEnabled(enable)

  local prevAmountButton = marketOffersPanel:recursiveGetChildById('prevAmountButton')
  local nextAmountButton = marketOffersPanel:recursiveGetChildById('nextAmountButton')
  
  prevAmountButton:setEnabled(enable)
  nextAmountButton:setEnabled(enable)
end

function Market.close(notify)
  if notify == nil then notify = true end
  if not marketWindow:isHidden() then
    marketWindow:hide()
    marketWindow:unlock()
    if notify then
      MarketProtocol.sendMarketLeave()
    end
  end
end

function Market.incrementAmount()
  amountEdit:setValue(amountEdit:getValue() + 1)
end

function Market.decrementAmount()
  amountEdit:setValue(amountEdit:getValue() - 1)
end

function Market.updateCurrentItems()
  local id = getMarketCategoryId(categoryList:getCurrentOption().text)
  if id == MarketCategory.MetaWeapons then
    id = getMarketCategoryId(subCategoryList:getCurrentOption().text)
  end
  Market.loadMarketItems(id)
end

function Market.resetCreateOffer()
  piecePriceEdit:setValue(1)
  totalPriceEdit:setValue(1)
  amountEdit:setValue(1)
  refreshTypeList()
  clearFee()
end

function Market.refreshItemsWidget(selectItem)
  local selectItem = selectItem or 0
  itemsPanel = browsePanel:recursiveGetChildById('itemsPanel')

  local layout = itemsPanel:getLayout()
  layout:disableUpdates()

  Market.clearSelectedItem()
  itemsPanel:destroyChildren()

  if radioItemSet then
    radioItemSet:destroy()
  end
  radioItemSet = UIRadioGroup.create()

  local select = nil
  for i = 1, #currentItems do
    local item = currentItems[i]
    local itemBox = g_ui.createWidget('MarketItemBox', itemsPanel)
    itemBox.onCheckChange = Market.onItemBoxChecked
    itemBox.item = item

    if selectItem > 0 and item.ptr:getId() == selectItem then
      select = itemBox
    end

    local itemWidget = itemBox:getChildById('item')
    item.ptr:setCount(1) -- reset item count for image
    itemWidget:setItem(item.ptr)

    local amount = Market.depotContains(item.ptr:getId())
    if amount > 0 then
      itemWidget:setText(amount)
      itemBox:setTooltip('You have '.. amount ..' in your depot.')
    end

    radioItemSet:addWidget(itemBox)
  end
  if select then
    select:setChecked(true)
  end

  layout:enableUpdates()
  layout:update()
end

function Market.refreshOffers()
  if Market.isItemSelected() then
    Market.onItemBoxChecked(selectedItem.ref)
  end
end


function Market.loadMarketItems(category)
  clearItems()

  -- check search filter
  local searchFilter = searchEdit:getText()
  if searchFilter and searchFilter:len() > 2 then
    if filterButtons[MarketFilters.SearchAll]:isChecked() then
      category = MarketCategory.All
    end
  end

  if category == MarketCategory.All then
    -- loop all categories
    for category = MarketCategory.First, MarketCategory.Last do
      for i = 1, #marketItems[category] do
        local item = marketItems[category][i]
        if isItemValid(item, category, searchFilter) then
          table.insert(currentItems, item)
        end
      end
    end
  else
    -- loop specific category
    for i = 1, #marketItems[category] do
      local item = marketItems[category][i]
      if isItemValid(item, category, searchFilter) then
        table.insert(currentItems, item)
      end
    end
  end

  Market.refreshItemsWidget()
end

function Market.loadDepotItems(depotItems)
  information.depotItems = {}

  local items = {}
  for i = 1, #depotItems do
    local data = depotItems[i]
    local id, count = data[1], data[2]

    local tmpItem = Item.create(id)
    if tmpItem:isStackable() then
      if count > 100 then
        local createCount = math.floor(count/100)
        local remainder = count % 100
        if remainder > 0 then
          createCount = createCount + 1
        end
        for i = 1, createCount do
          local newItem = Item.create(id)
          if i == createCount and remainder > 0 then
            newItem:setCount(remainder)
          else
            newItem:setCount(100)
          end
          table.insert(items, newItem)
        end
      else
        local newItem = Item.create(id)
        newItem:setCount(count)
        table.insert(items, newItem)
      end
    else
      for i = 1, count do
        table.insert(items, Item.create(id))
      end
    end
  end

  for _, newItem in pairs(items) do
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

function Market.createNewOffer()
  local type = offerTypeList:getCurrentOption().text
  if type == 'Sell' then
    type = MarketAction.Sell
  else
    type = MarketAction.Buy
  end

  if not Market.isItemSelected() then
    return
  end
  local item = selectedItem.item
  local spriteId = item.ptr:getId()

  local piecePrice = piecePriceEdit:getValue()
  local totalPrice = totalPriceEdit:getValue()

  local amount = amountEdit:getValue()
  local anonymous = anonymous:isChecked() and 1 or 0

  -- error checking
  local errorMsg = ''
  if type == MarketAction.Buy then
    if information.balance < ((piecePrice * amount) + fee) then
      errorMsg = errorMsg..'Not enough balance to create this offer.\n'
    end
  elseif type == MarketAction.Sell then
    if Market.depotContains(spriteId) < amount then
      errorMsg = errorMsg..'Not enough items in your depot to create this offer.\n'
    end
  end

  if piecePrice > piecePriceEdit.maximum then
    errorMsg = errorMsg..'Price is too high.\n'
  elseif piecePrice < piecePriceEdit.minimum then
    errorMsg = errorMsg..'Price is too low.\n'
  end
  if amount > amountEdit.maximum then
    errorMsg = errorMsg..'Amount is too high.\n'
  elseif amount < amountEdit.minimum then
    errorMsg = errorMsg..'Amount is too low.\n'
  end

  local timeCheck = os.time() - lastCreatedOffer
  if timeCheck < offerExhaust[type] then
    local waitTime = math.ceil(offerExhaust[type] - timeCheck)
    errorMsg = errorMsg..'You must wait '.. waitTime ..' seconds before creating a new offer.\n'
  end

  if errorMsg ~= '' then
    displayInfoBox('Error', errorMsg)
    return
  end

  MarketProtocol.sendMarketCreateOffer(type, spriteId, amount, piecePrice, anonymous)
  lastCreatedOffer = os.time()
  Market.resetCreateOffer()
end

function Market.acceptMarketOffer(amount, timestamp, counter)
  if timestamp > 0 and amount > 0 then
    MarketProtocol.sendMarketAcceptOffer(timestamp, counter, amount)
    Market.refreshOffers()
  end
end

function Market.onItemBoxChecked(widget)
  if widget:isChecked() then
    if selectedItem.ref and widget ~= selectedItem.ref then
      selectedItem.ref:setChecked(false) -- temporary fix?
    end
    updateSelectedItem(widget)
  end
end

-- protocol callback functions

function Market.onMarketEnter(depotItems, offers, balance, vocation)
  if not loaded then
    initMarketItems(MarketCategory.All)
    loaded = true
  end

  Market.clearSelectedItem()
  updateBalance(balance)
  
  information.totalOffers = offers
  local player = g_game.getLocalPlayer()
  if player then
    information.player = player
  end
  if vocation == -1 then
    if player then
      information.vocation = player:getVocation()
    end
  else
    -- vocation must be compatible with < 950
    information.vocation = vocation
  end

  Market.loadDepotItems(depotItems)
  -- update the items widget to match depot items
  if Market.isItemSelected() then
    local spriteId = selectedItem.item.ptr:getId()
    MarketProtocol.silent(true) -- disable protocol messages
    Market.refreshItemsWidget(spriteId)
    MarketProtocol.silent(false) -- enable protocol messages
  else
    Market.refreshItemsWidget()
  end
 
  if table.empty(currentItems) then
    Market.loadMarketItems(MarketCategory.First)
  end

  -- build offer table header
  if buyOfferTable and not buyOfferTable:hasHeader() then
    buyOfferTable:addHeaderRow(offerTableHeader)
  end
  if sellOfferTable and not sellOfferTable:hasHeader() then
    sellOfferTable:addHeaderRow(offerTableHeader)
  end

  if g_game.isOnline() then
    marketWindow:lock()
    marketWindow:show()
  end
end

function Market.onMarketLeave()
  Market.close(false)
end

function Market.onMarketDetail(itemId, descriptions, purchaseStats, saleStats)
  updateDetails(itemId, descriptions, purchaseStats, saleStats)
end

function Market.onMarketBrowse(offers)
  updateOffers(offers)
end
