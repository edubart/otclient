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

      * Extend information features
        - Hover over offers for purchase information (balance after transaction, etc)
  ]]

Market = {}

local protocol = runinsandbox('marketprotocol')

marketWindow = nil
mainTabBar = nil
displaysTabBar = nil
offersTabBar = nil
selectionTabBar = nil

nameLabel = nil
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
selectedMyOffer = {}

feeLabel = nil
balanceLabel = nil
totalPriceEdit = nil
piecePriceEdit = nil
amountEdit = nil
searchEdit = nil
radioItemSet = nil
selectedItem = nil
selectedItemLabel = nil
selectedItemTitleLabel = nil
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

buyCancelButton = nil
sellCancelButton = nil
buyMyOfferTable = nil
sellMyOfferTable = nil

offerExhaust = {}
marketOffers = {}
marketItems = {}
information = {}
currentItems = {}
lastCreatedOffer = 0
fee = 0
averagePrice = 0

loaded = false

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
    if slotFilter ~= 255 and item.thingType:getClothSlot() ~= slotFilter then
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
  if filterDepot and Market.getDepotCount(item.marketData.tradeAs) <= 0 then
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

local function clearMyOffers()
  marketOffers[MarketAction.Buy] = {}
  marketOffers[MarketAction.Sell] = {}
  buyMyOfferTable:clearData()
  sellMyOfferTable:clearData()
end

local function clearFilters()
  for _, filter in pairs(filterButtons) do
    if filter and filter:isChecked() ~= filter.default then
      filter:setChecked(filter.default)
    end
  end
end

local function clearFee()
  feeLabel:setText('')
  fee = 20
end

local function refreshTypeList()
  offerTypeList:clearOptions()
  offerTypeList:addOption('Buy')

  if Market.isItemSelected() then
    if Market.getDepotCount(selectedItem.item.marketData.tradeAs) > 0 then
      offerTypeList:addOption('Sell')
    end
  end
end

local function addOffer(offer, offerType)
  if not offer then
    return false
  end
  local id = offer:getId()
  local player = offer:getPlayer()
  local amount = offer:getAmount()
  local price = offer:getPrice()
  local timestamp = offer:getTimeStamp()
  local itemName = offer:getItem():getMarketData().name

  buyOfferTable:toggleSorting(false)
  sellOfferTable:toggleSorting(false)

  buyMyOfferTable:toggleSorting(false)
  sellMyOfferTable:toggleSorting(false)

  if amount < 1 then return false end
  if offerType == MarketAction.Buy then
    if offer.warn then
      buyOfferTable:setColumnStyle('OfferTableWarningColumn', true)
    end

    local row = nil
    if offer.var == MarketRequest.MyOffers then
      row = buyMyOfferTable:addRow({
        {text = itemName},
        {text = price*amount},
        {text = price},
        {text = amount},
        {text = string.gsub(os.date('%c', timestamp), " ", "  "), sortvalue = timestamp}
      })
    else
      row = buyOfferTable:addRow({
        {text = player},
        {text = amount},
        {text = price*amount},
        {text = price},
        {text = string.gsub(os.date('%c', timestamp), " ", "  ")}
      })
    end
    row.ref = id

    if offer.warn then
      row:setTooltip(tr('This offer is 25%% below the average market price'))
      buyOfferTable:setColumnStyle('OfferTableColumn', true)
    end
  else
    if offer.warn then
      sellOfferTable:setColumnStyle('OfferTableWarningColumn', true)
    end

    local row = nil
    if offer.var == MarketRequest.MyOffers then
      row = sellMyOfferTable:addRow({
        {text = itemName},
        {text = price*amount},
        {text = price},
        {text = amount},
        {text = string.gsub(os.date('%c', timestamp), " ", "  "), sortvalue = timestamp}
      })
    else
      row = sellOfferTable:addRow({
        {text = player},
        {text = amount},
        {text = price*amount},
        {text = price},
        {text = string.gsub(os.date('%c', timestamp), " ", "  "), sortvalue = timestamp}
      })
    end
    row.ref = id

    if offer.warn then
      row:setTooltip(tr('This offer is 25%% above the average market price'))
      sellOfferTable:setColumnStyle('OfferTableColumn', true)
    end
  end

  buyOfferTable:toggleSorting(false)
  sellOfferTable:toggleSorting(false)
  buyOfferTable:sort()
  sellOfferTable:sort()

  buyMyOfferTable:toggleSorting(false)
  sellMyOfferTable:toggleSorting(false)
  buyMyOfferTable:sort()
  sellMyOfferTable:sort()

  return true
end

local function mergeOffer(offer)
  if not offer then
    return false
  end

  local id = offer:getId()
  local offerType = offer:getType()
  local amount = offer:getAmount()
  local replaced = false

  if offerType == MarketAction.Buy then
    if averagePrice > 0 then
      offer.warn = offer:getPrice() <= averagePrice - math.floor(averagePrice / 4)
    end

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
    if averagePrice > 0 then
      offer.warn = offer:getPrice() >= averagePrice + math.floor(averagePrice / 4)
    end

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

  selectedMyOffer[MarketAction.Buy] = nil
  selectedMyOffer[MarketAction.Sell] = nil

  -- clear existing offer data
  buyOfferTable:clearData()
  buyOfferTable:setSorting(4, TABLE_SORTING_DESC)
  sellOfferTable:clearData()
  sellOfferTable:setSorting(4, TABLE_SORTING_ASC)

  sellButton:setEnabled(false)
  buyButton:setEnabled(false)

  buyCancelButton:setEnabled(false)
  sellCancelButton:setEnabled(false)

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
      {text = getMarketDescriptionName(desc[1])..':'},
      {text = desc[2]}
    }
    detailsTable:addRow(columns)
  end

  -- update sale item statistics
  sellStatsTable:clearData()
  if table.empty(saleStats) then
    sellStatsTable:addRow({{text = 'No information'}})
  else
    local offerAmount = 0
    local transactions, totalPrice, highestPrice, lowestPrice = 0, 0, 0, 0
    for _, stat in pairs(saleStats) do
      if not stat:isNull() then
        offerAmount = offerAmount + 1
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

    if offerAmount >= 5 and transactions >= 10 then
      averagePrice = math.round(totalPrice / transactions)
    else
      averagePrice = 0
    end

    sellStatsTable:addRow({{text = 'Total Transations:'}, {text = transactions}})
    sellStatsTable:addRow({{text = 'Highest Price:'}, {text = highestPrice}})

    if totalPrice > 0 and transactions > 0 then
      sellStatsTable:addRow({{text = 'Average Price:'},
        {text = math.floor(totalPrice/transactions)}})
    else
      sellStatsTable:addRow({{text = 'Average Price:'}, {text = 0}})
    end

    sellStatsTable:addRow({{text = 'Lowest Price:'}, {text = lowestPrice}})
  end

  -- update buy item statistics
  buyStatsTable:clearData()
  if table.empty(purchaseStats) then
    buyStatsTable:addRow({{text = 'No information'}})
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

    buyStatsTable:addRow({{text = 'Total Transations:'},{text = transactions}})
    buyStatsTable:addRow({{text = 'Highest Price:'}, {text = highestPrice}})

    if totalPrice > 0 and transactions > 0 then
      buyStatsTable:addRow({{text = 'Average Price:'},
        {text = math.floor(totalPrice/transactions)}})
    else
      buyStatsTable:addRow({{text = 'Average Price:'}, {text = 0}})
    end

    buyStatsTable:addRow({{text = 'Lowest Price:'}, {text = lowestPrice}})
  end
end

local function updateSelectedItem(widget)
  selectedItem.item = widget.item
  selectedItem.ref = widget

  Market.resetCreateOffer()
  if Market.isItemSelected() then
    selectedItem:setItem(selectedItem.item.displayItem)
    selectedItemLabel:setText('')
    selectedItemTitleLabel:setText(selectedItem.item.marketData.name)
    Market:hideOffersTableInstructions()
    clearOffers()

    Market.enableCreateOffer(true) -- update offer types
    MarketProtocol.sendMarketBrowse(selectedItem.item.marketData.tradeAs) -- send browsed msg
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

local function destroyAmountWindow()
  if amountWindow then
    amountWindow:destroy()
    amountWindow = nil
  end
end

local function cancelMyOffer(actionType)
  local offer = selectedMyOffer[actionType]
  MarketProtocol.sendMarketCancelOffer(offer:getTimeStamp(), offer:getCounter())
  Market.refreshMyOffers()
end

local function openAmountWindow(callback, actionType, actionText)
  if not Market.isOfferSelected(actionType) then
    return
  end

  amountWindow = g_ui.createWidget('AmountWindow', rootWidget)
  amountWindow:lock()

  local offer = selectedOffer[actionType]
  local item = offer:getItem()

  local maximum = offer:getAmount()
  if actionType == MarketAction.Sell then
    local depot = Market.getDepotCount(item:getId())
    if maximum > depot then
      maximum = depot
    end
  else
    maximum = math.min(maximum, math.floor(information.balance / offer:getPrice()))
  end

  if item:isStackable() then
    maximum = math.min(maximum, MarketMaxAmountStackable)
  else
    maximum = math.min(maximum, MarketMaxAmount)
  end

  local itembox = amountWindow:getChildById('item')
  itembox:setItemId(item:getId())

  local scrollbar = amountWindow:getChildById('amountScrollBar')
  scrollbar:setText(offer:getPrice()..'gp')

  scrollbar.onValueChange = function(widget, value)
    widget:setText((value*offer:getPrice())..'gp')
    itembox:setText(value)
  end

  scrollbar:setRange(1, maximum)
  scrollbar:setValue(1)

  local okButton = amountWindow:getChildById('buttonOk')
  if actionText then
    okButton:setText(actionText)
  end

  local okFunc = function()
    local counter = offer:getCounter()
    local timestamp = offer:getTimeStamp()
    callback(scrollbar:getValue(), timestamp, counter)
    destroyAmountWindow()
  end

  local cancelButton = amountWindow:getChildById('buttonCancel')
  local cancelFunc = function()
    destroyAmountWindow()
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
      if Market.getDepotCount(offer:getItem():getId()) > 0 then
        sellButton:setEnabled(true)
      else
        sellButton:setEnabled(false)
      end
    end
  end
end

local function onSelectMyBuyOffer(table, selectedRow, previousSelectedRow)
  for _, offer in pairs(marketOffers[MarketAction.Buy]) do
    if offer:isEqual(selectedRow.ref) then
      selectedMyOffer[MarketAction.Buy] = offer
      buyCancelButton:setEnabled(true)
    end
  end
end

local function onSelectMySellOffer(table, selectedRow, previousSelectedRow)
  for _, offer in pairs(marketOffers[MarketAction.Sell]) do
    if offer:isEqual(selectedRow.ref) then
      selectedMyOffer[MarketAction.Sell] = offer
      sellCancelButton:setEnabled(true)
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
  local item = selectedItem.item
  local maximum = item.thingType:isStackable() and MarketMaxAmountStackable or MarketMaxAmount

  if option == 'Sell' then
    maximum = math.min(maximum, Market.getDepotCount(item.marketData.tradeAs))
    amountEdit:setMaximum(maximum)
  else
    amountEdit:setMaximum(maximum)
  end
end

local function onTotalPriceChange()
  local amount = amountEdit:getValue()
  local totalPrice = totalPriceEdit:getValue()
  local piecePrice = math.floor(totalPrice/amount)

  piecePriceEdit:setValue(piecePrice, true)
  if Market.isItemSelected() then
    updateFee(piecePrice, amount)
  end
end

local function onPiecePriceChange()
  local amount = amountEdit:getValue()
  local totalPrice = totalPriceEdit:getValue()
  local piecePrice = piecePriceEdit:getValue()

  totalPriceEdit:setValue(piecePrice*amount, true)
  if Market.isItemSelected() then
    updateFee(piecePrice, amount)
  end
end

local function onAmountChange()
  local amount = amountEdit:getValue()
  local piecePrice = piecePriceEdit:getValue()
  local totalPrice = piecePrice * amount

  totalPriceEdit:setValue(piecePrice*amount, true)
  if Market.isItemSelected() then
    updateFee(piecePrice, amount)
  end
end

local function onMarketMessage(messageMode, message)
  Market.displayMessage(message)
end

local function initMarketItems()
  for c = MarketCategory.First, MarketCategory.Last do
    marketItems[c] = {}
  end

  -- save a list of items which are already added
  local itemSet = {}

  -- populate all market items
  local types = g_things.findThingTypeByAttr(ThingAttrMarket, 0)
  for i = 1, #types do
    local itemType = types[i]

    local item = Item.create(itemType:getId())
    if item then
      local marketData = itemType:getMarketData()
      if not table.empty(marketData) and not itemSet[marketData.tradeAs] then
          -- Some items use a different sprite in Market
          item:setId(marketData.showAs)

          -- create new marketItem block
          local marketItem = {
            displayItem = item,
            thingType = itemType,
            marketData = marketData
          }

          -- add new market item
        if marketItems[marketData.category] ~= nil then
          table.insert(marketItems[marketData.category], marketItem)
          itemSet[marketData.tradeAs] = true
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
  local mopTab = mainTabBar:addTab(tr('Market Offers'), marketOffersPanel)
  mopTab.onClick = function()
    mainTabBar:selectTab(mopTab)
    Market.refreshOffers()
  end

  selectionTabBar = marketOffersPanel:getChildById('leftTabBar')
  selectionTabBar:setContentWidget(marketOffersPanel:getChildById('leftTabContent'))

  browsePanel = g_ui.loadUI('ui/marketoffers/browse')
  selectionTabBar:addTab(tr('Browse'), browsePanel)

  -- Currently not used
  -- "Reserved for more functionality later"
  --overviewPanel = g_ui.loadUI('ui/marketoffers/overview')
  --selectionTabBar:addTab(tr('Overview'), overviewPanel)

  displaysTabBar = marketOffersPanel:getChildById('rightTabBar')
  displaysTabBar:setContentWidget(marketOffersPanel:getChildById('rightTabContent'))

  itemStatsPanel = g_ui.loadUI('ui/marketoffers/itemstats')
  displaysTabBar:addTab(tr('Statistics'), itemStatsPanel)

  itemDetailsPanel = g_ui.loadUI('ui/marketoffers/itemdetails')
  displaysTabBar:addTab(tr('Details'), itemDetailsPanel)

  itemOffersPanel = g_ui.loadUI('ui/marketoffers/itemoffers')
  displaysTabBar:addTab(tr('Offers'), itemOffersPanel)
  displaysTabBar:selectTab(displaysTabBar:getTab(tr('Offers')))

  -- setup 'My Offer' section tabs
  myOffersPanel = g_ui.loadUI('ui/myoffers')
  local moTab = mainTabBar:addTab(tr('My Offers'), myOffersPanel)
  moTab.onClick = function()
    mainTabBar:selectTab(moTab)
    Market.refreshMyOffers()
  end

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
  selectedItem = marketOffersPanel:getChildById('selectedItem')
  selectedItemLabel = selectedItem:getChildById('selectedItemLabel')
  selectedItemTitleLabel = marketOffersPanel:getChildById('selectedItemTitleLabel')

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

  -- set filter default values
  clearFilters()

  -- hook filters
  for _, filter in pairs(filterButtons) do
    filter.onCheckChange = Market.updateCurrentItems
  end

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

  -- setup my offers
  buyMyOfferTable = currentOffersPanel:recursiveGetChildById('myBuyingTable')
  sellMyOfferTable = currentOffersPanel:recursiveGetChildById('mySellingTable')
  buyMyOfferTable.onSelectionChange = onSelectMyBuyOffer
  sellMyOfferTable.onSelectionChange = onSelectMySellOffer

  buyCancelButton = currentOffersPanel:getChildById('buyCancelButton')
  buyCancelButton.onClick = function() cancelMyOffer(MarketAction.Buy) end

  sellCancelButton = currentOffersPanel:getChildById('sellCancelButton')
  sellCancelButton.onClick = function() cancelMyOffer(MarketAction.Sell) end


  buyStatsTable:setColumnWidth({120, 270})
  sellStatsTable:setColumnWidth({120, 270})
  detailsTable:setColumnWidth({80, 330})

  buyOfferTable:setSorting(4, TABLE_SORTING_DESC)
  sellOfferTable:setSorting(4, TABLE_SORTING_ASC)

  buyMyOfferTable:setSorting(3, TABLE_SORTING_DESC)
  sellMyOfferTable:setSorting(3, TABLE_SORTING_DESC)
end

function init()
  g_ui.importStyle('market')
  g_ui.importStyle('ui/general/markettabs')
  g_ui.importStyle('ui/general/marketbuttons')
  g_ui.importStyle('ui/general/marketcombobox')
  g_ui.importStyle('ui/general/amountwindow')

  offerExhaust[MarketAction.Sell] = 10
  offerExhaust[MarketAction.Buy] = 20

  registerMessageMode(MessageModes.Market, onMarketMessage)

  protocol.initProtocol()
  connect(g_game, { onGameEnd = Market.reset })
  connect(g_game, { onGameEnd = Market.close })
  marketWindow = g_ui.createWidget('MarketWindow', rootWidget)
  marketWindow:hide()

  initInterface() -- build interface
end

function terminate()
  Market.close()

  unregisterMessageMode(MessageModes.Market, onMarketMessage)

  protocol.terminateProtocol()
  disconnect(g_game, { onGameEnd = Market.reset })
  disconnect(g_game, { onGameEnd = Market.close })

  destroyAmountWindow()
  marketWindow:destroy()

  Market = nil
end

function Market.showMyOffersTableInstructions()
  local instruction = tr('Press %s button to update', tr('Refresh Offers'))
  if sellMyOfferTable then sellMyOfferTable:setText(instruction) end
  if buyMyOfferTable then buyMyOfferTable:setText(instruction) end
end

function Market.hideMyOffersTableInstructions()
  if sellMyOfferTable then sellMyOfferTable:setText('') end
  if buyMyOfferTable then buyMyOfferTable:setText('') end
end

function Market.showOffersTableInstructions()
  local instruction = tr('Select an item to view the offers')
  if sellOfferTable then sellOfferTable:setText(instruction) end
  if buyOfferTable then buyOfferTable:setText(instruction) end
end

function Market.hideOffersTableInstructions()
  if sellOfferTable then sellOfferTable:setText('') end
  if buyOfferTable then buyOfferTable:setText('') end
end


function Market.reset()
  balanceLabel:setColor('#bbbbbb')
  categoryList:setCurrentOption(getMarketCategoryName(MarketCategory.First))
  searchEdit:setText('')

  -- When uses closes market at this screen we need to show this instruction again when it gets opened,
  -- since we cannot load offers for the user ourselves due to the bot protection.
  Market:showMyOffersTableInstructions()

  clearFilters()
  clearMyOffers()
  if not table.empty(information) then
    Market.updateCurrentItems()
  end
end

function Market.displayMessage(message)
  if marketWindow:isHidden() then return end

  local infoBox = displayInfoBox(tr('Market Error'), message)
  infoBox:lock()
end

function Market.clearSelectedItem()
  if Market.isItemSelected() then
    Market.resetCreateOffer(true)
    offerTypeList:clearOptions()
    offerTypeList:setText('Please Select')
    offerTypeList:setEnabled(false)

    clearOffers()
    radioItemSet:selectWidget(nil)
    Market:showOffersTableInstructions()
    selectedItem:setItem(nil)
    selectedItem.item = nil
    selectedItem.ref:setChecked(false)
    selectedItem.ref = nil
    selectedItemLabel:setText('?')
    selectedItemTitleLabel:setText(tr('No item selected.'))

    detailsTable:clearData()
    buyStatsTable:clearData()
    sellStatsTable:clearData()

    Market.enableCreateOffer(false)
  end
end

function Market.isItemSelected()
  return selectedItem and selectedItem.item
end

function Market.isOfferSelected(type)
  return selectedOffer[type] and not selectedOffer[type]:isNull()
end

function Market.getDepotCount(itemId)
  return information.depotItems[itemId] or 0
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
    modules.game_interface.getRootPanel():focus()
    Market.clearSelectedItem()
    Market.reset()
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

function Market.resetCreateOffer(resetFee)
  piecePriceEdit:setValue(1)
  totalPriceEdit:setValue(1)
  amountEdit:setValue(1)
  refreshTypeList()

  if resetFee then
    clearFee()
  else
    updateFee(0, 0)
  end
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

    if selectItem > 0 and item.marketData.tradeAs == selectItem then
      select = itemBox
      selectItem = 0
    end

    local itemWidget = itemBox:getChildById('item')
    itemWidget:setItem(item.displayItem)

    local amount = Market.getDepotCount(item.marketData.tradeAs)
    if amount > 0 then
      itemWidget:setText(amount)
      itemBox:setTooltip('You have '.. amount ..' in your depot.')
    end

    radioItemSet:addWidget(itemBox)
  end

  if select then
    radioItemSet:selectWidget(select, false)
  end

  layout:enableUpdates()
  layout:update()
end

function Market.refreshOffers()
  if Market.isItemSelected() then
    Market.onItemBoxChecked(selectedItem.ref)
  else
    Market.refreshMyOffers()
  end
end

function Market.refreshMyOffers()
  Market:hideMyOffersTableInstructions()
  clearMyOffers()
  MarketProtocol.sendMarketBrowseMyOffers()
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

  local spriteId = selectedItem.item.marketData.tradeAs

  local piecePrice = piecePriceEdit:getValue()
  local amount = amountEdit:getValue()
  local anonymous = anonymous:isChecked() and 1 or 0

  -- error checking
  local errorMsg = ''
  if type == MarketAction.Buy then
    if information.balance < ((piecePrice * amount) + fee) then
      errorMsg = errorMsg..'Not enough balance to create this offer.\n'
    end
  elseif type == MarketAction.Sell then
    if information.balance < fee then
      errorMsg = errorMsg..'Not enough balance to create this offer.\n'
    end
    if Market.getDepotCount(spriteId) < amount then
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

  if amount * piecePrice > MarketMaxPrice then
    errorMsg = errorMsg..'Total price is too high.\n'
  end

  if information.totalOffers >= MarketMaxOffers then
    errorMsg = errorMsg..'You cannot create more offers.\n'
  end

  local timeCheck = os.time() - lastCreatedOffer
  if timeCheck < offerExhaust[type] then
    local waitTime = math.ceil(offerExhaust[type] - timeCheck)
    errorMsg = errorMsg..'You must wait '.. waitTime ..' seconds before creating a new offer.\n'
  end

  if errorMsg ~= '' then
    Market.displayMessage(errorMsg)
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
    updateSelectedItem(widget)
  end
end

-- protocol callback functions

function Market.onMarketEnter(depotItems, offers, balance, vocation)
  if not loaded then
    initMarketItems()
    loaded = true
  end

  updateBalance(balance)
  averagePrice = 0

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

  -- set list of depot items
  information.depotItems = depotItems

  -- update the items widget to match depot items
  if Market.isItemSelected() then
    local spriteId = selectedItem.item.marketData.tradeAs
    MarketProtocol.silent(true) -- disable protocol messages
    Market.refreshItemsWidget(spriteId)
    MarketProtocol.silent(false) -- enable protocol messages
  else
    Market.refreshItemsWidget()
  end

  if table.empty(currentItems) then
    Market.loadMarketItems(MarketCategory.First)
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

function Market.onMarketResourceBalance(resourceType, value)
  return
end
