Market = {}

local protocol = runinsandbox('marketprotocol.lua')

marketWindow = nil
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
selectedOffer = {}

nameLabel = nil
feeLabel = nil
balanceLabel = nil
totalPriceEdit = nil
piecePriceEdit = nil
amountEdit = nil
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

marketOffers = {}
marketItems = {}
information = {}
currentItems = {}
fee = 0

local offerTableHeader = {
  {['text'] = 'Player Name', ['width'] = 100},
  {['text'] = 'Amount', ['width'] = 60},
  {['text'] = 'Total Price', ['width'] = 90},
  {['text'] = 'Piece Price', ['width'] = 80},
  {['text'] = 'Ends at', ['width'] = 120}
}

local function isItemValid(item, category)
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
  if filterDepot and Market.depotContains(item.ptr:getId()) <= 0 then
    return false
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

local function refreshTypeList()
  offerTypeList:clearOptions()
  offerTypeList:addOption('Buy')

  if Market.isItemSelected() then
    if Market.depotContains(selectedItem.item.ptr:getId()) > 0 then
      offerTypeList:addOption('Sell')
    end
  end
end

local function refreshFee()
  feeLabel:setText('')
  fee = 0
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
  -- TODO: optimize offer updates later
  selectedOffer[MarketAction.Buy] = nil
  selectedOffer[MarketAction.Sell] = nil

  if not buyOfferTable or not sellOfferTable then
    return
  end
  balanceLabel:setColor('#bbbbbb')
  buyOfferTable:clearData()
  sellOfferTable:clearData()
  
  sellButton:setEnabled(false)
  buyButton:setEnabled(false)

  for k, offer in pairs(offers) do
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

local function updateSelectedItem(newItem)
  selectedItem.item = newItem

  Market.resetCreateOffer()
  if Market.isItemSelected() then
    selectedItem:setItem(selectedItem.item.ptr)
    nameLabel:setText(selectedItem.item.marketData.name)
    clearOffers()

    Market.enableCreateOffer(true)-- update offer types
    MarketProtocol.sendMarketBrowse(selectedItem.item.ptr:getId()) -- send browsed msg
  else
    Market.Market.clearSelectedItem()
  end
end

local function updateBalance(balance)
  local balance = tonumber(balance)
  if not balance then
    return
  end

  if balance < 0 then balance = 0 end
  information.balance = balance

  balanceLabel:setText('Balance: '..balance..'gp')
  balanceLabel:resizeToText()
end

local function updateDepotItemCount(itemId, amount)
  if Market.depotContains(itemId) < amount then
    return false
  end
  for i = 1, #information.depotItems do
    local depotItem = information.depotItems[i]
    if depotItem and itemId == depotItem.ptr:getId() then
      local depotItemCount = depotItem.ptr:getCount()

      if depotItem.ptr:isStackable() then
        if depotItemCount <= 100 and depotItemCount >= amount then
          if (depotItemCount - amount) <= 0 then
            table.remove(information.depotItems, i)
          else
            depotItem.ptr:setCount(depotItemCount - amount)
            information.depotItems[i] = depotItem
          end
          return true
        else
          local removeCount = math.floor(amount/100)
          local remainder = amount % depotItemCount
          if remainder > 0 then
            removeCount = removeCount + 1
          end
          for j = 1, removeCount do
            if j == removeCount and remainder > 0 then
              updateDepotItemCount(itemId, remainder)
            else
              updateDepotItemCount(itemId, 100)
            end
          end
          return true
        end
      else
        if amount > 0 then
          table.remove(information.depotItems, i)
          amount = amount - 1
        end
      end
    end
  end
  return false
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

  local spinbox = amountWindow:getChildById('amountSpinBox')
  spinbox:setMaximum(selectedOffer[type]:getAmount())
  spinbox:setMinimum(1)
  spinbox:setValue(1)

  local scrollbar = amountWindow:getChildById('amountScrollBar')
  scrollbar:setMaximum(selectedOffer[type]:getAmount())
  scrollbar:setMinimum(1)
  scrollbar:setValue(1)

  scrollbar.onValueChange = function(self, value) spinbox:setValue(value) end
  spinbox.onValueChange = function(self, value) scrollbar:setValue(value) end
  
  local okButton = amountWindow:getChildById('buttonOk')
  if actionText ~= '' then okButton:setText(actionText) end

  local okFunc = function()
    local counter = selectedOffer[type]:getCounter()
    local timestamp = selectedOffer[type]:getTimeStamp()
    callback(spinbox:getValue(), timestamp, counter)
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
      buyButton:setEnabled(true)
    end
  end

  local offer = selectedOffer[MarketAction.Buy]
  if offer then
    local price = offer:getPrice()
    if price > information.balance then
      balanceLabel:setColor('#b22222')
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
    end
  end
end

local function onSelectBuyOffer(table, selectedRow, previousSelectedRow)
  updateBalance()
  for _, offer in pairs(marketOffers[MarketAction.Buy]) do
    if offer:isEqual(selectedRow.ref) then
      selectedOffer[MarketAction.Sell] = offer
      if Market.depotContains(offer:getItem():getId()) >= offer:getAmount() then
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
    if table.hasKey(MarketSlotFilters, slot) then
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

local function initMarketItems()
  -- populate all market items
  marketItems = {}
  local types = g_things.findThingTypeByAttr(ThingAttrMarket, 0)
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

  balanceLabel = marketWindow:getChildById('balanceLabel')

  -- setup offers
  buyButton = itemOffersPanel:getChildById('buyButton')
  buyButton.onClick = function() openAmountWindow(Market.buyMarketOffer, MarketAction.Buy, 'Buy') end

  sellButton = itemOffersPanel:getChildById('sellButton')
  sellButton.onClick = function() openAmountWindow(Market.sellMarketOffer, MarketAction.Sell, 'Sell') end

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
  categoryList.onOptionChange = onChangeCategory
  subCategoryList.onOptionChange = onChangeSubCategory
  slotFilterList.onOptionChange = onChangeSlotFilter

  -- get tables
  buyOfferTable = itemOffersPanel:recursiveGetChildById('buyingTable')
  sellOfferTable = itemOffersPanel:recursiveGetChildById('sellingTable')
  detailsTable = itemDetailsPanel:recursiveGetChildById('detailsTable')
  buyStatsTable = itemStatsPanel:recursiveGetChildById('buyStatsTable')
  sellStatsTable = itemStatsPanel:recursiveGetChildById('sellStatsTable')
  buyOfferTable.onSelectionChange = onSelectBuyOffer
  sellOfferTable.onSelectionChange = onSelectSellOffer
end

function init()
  g_ui.importStyle('market.otui')
  g_ui.importStyle('ui/general/markettabs.otui')
  g_ui.importStyle('ui/general/marketbuttons.otui')
  g_ui.importStyle('ui/general/marketcombobox.otui')
  g_ui.importStyle('ui/general/amountwindow.otui')

  protocol.initProtocol()
  connect(g_game, { onGameEnd = Market.reset })
  marketWindow = g_ui.createWidget('MarketWindow', rootWidget)
  marketWindow:hide()

  initInterface() -- build interface
  initMarketItems()
end

function terminate()
  protocol.terminateProtocol()
  disconnect(g_game, { onGameEnd = Market.reset })

  if marketWindow then
    marketWindow:destroy()
  end

  Market = nil
end

function Market.reset()
  balanceLabel:setColor('#bbbbbb')
  marketWindow:unlock()
  marketWindow:hide()
  categoryList:setCurrentOption(getMarketCategoryName(MarketCategory.First))
  Market.clearSelectedItem()
  clearFilters()
  clearItems()
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
  refreshFee()
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

function Market.loadMarketItems(category)
  if table.empty(marketItems) then
    initMarketItems()
  end

  clearItems()
  for i = 1, #marketItems do
    local item = marketItems[i]
    if isItemValid(item, category) then
      table.insert(currentItems, item)
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

  local errorMsg = ''
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

  if errorMsg ~= '' then
    UIMessageBox.display('Error', errorMsg, MessageBoxOk)
    return
  end

  MarketProtocol.sendMarketCreateOffer(type, spriteId, amount, piecePrice, anonymous)
  if type == MarketAction.Sell then
    --[[
      This is require due to bot protected protocol (cannot update browse item without user input)
      normal way is to use the new retrieved depot items in onMarketEnter and refresh the items widget.   
    ]]
    updateDepotItemCount(spriteId, amount)
    Market.refreshItemsWidget(spriteId)
  end
  Market.resetCreateOffer()
end

function Market.buyMarketOffer(amount, timestamp, counter)
  if timestamp > 0 and amount > 0 then
    MarketProtocol.sendMarketAcceptOffer(timestamp, counter, amount)
  end
end

function Market.sellMarketOffer(amount, timestamp, counter)
  if timestamp > 0 and amount > 0 then
    MarketProtocol.sendMarketAcceptOffer(timestamp, counter, amount)
  end
end

function Market.onItemBoxChecked(widget)
  if widget:isChecked() then
    updateSelectedItem(widget.item)
  end
end

function Market.onMarketEnter(depotItems, offers, balance, vocation)
  updateBalance(balance)
  information.totalOffers = offers
  if vocation < 0 then
    local player = g_game.getLocalPlayer()
    if player then information.vocation = player:getVocation() end
  else
    -- vocation must be compatible with < 950
    information.vocation = vocation
  end

  Market.loadDepotItems(depotItems)
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
  marketWindow:unlock()
  marketWindow:hide()
end

function Market.onMarketDetail(itemId, descriptions, purchaseStats, saleStats)
  updateDetails(itemId, descriptions, purchaseStats, saleStats)
end

function Market.onMarketBrowse(offers)
  updateOffers(offers)
end
