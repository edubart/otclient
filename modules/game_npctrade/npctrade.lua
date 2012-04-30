NPCTrade = {}

-- private variables
local BUY = 1
local SELL = 2
local CURRENCY = 'gold'
local WEIGHT_UNIT = 'oz'
local LAST_INVENTORY = 10

local npcWindow
local itemsPanel
local radioTabs
local radioItems
local searchText
local setupPanel
local quantity
local quantityScroll
local nameLabel
local priceLabel
local moneyLabel
local weightLabel
local capacityLabel
local tradeButton
local buyTab
local sellTab

local buyWithBackpack
local ignoreCapacity
local ignoreEquipped
local showAllItems

local playerFreeCapacity
local playerMoney
local tradeItems = {}
local playerItems

local selectedItem

-- private functions
local function clearSelectedItem()
  nameLabel:clearText()
  weightLabel:clearText()
  priceLabel:clearText()
  quantityLabel:clearText()
  tradeButton:disable()
  quantityScroll:setMaximum(1)
  if selectedItem then
    radioItems:selectWidget(nil)
    selectedItem = nil
  end
end

local function getCurrentTradeType()
  if tradeButton:getText() == tr('Buy') then
    return BUY
  else
    return SELL
  end
end

local function getItemPrice(item)
  if getCurrentTradeType() == BUY then
    if buyWithBackpack:isChecked() then
      if item.ptr:isStackable() then
        return item.price*quantityScroll:getValue() + 20;
      else
        return item.price*quantityScroll:getValue() + math.ceil(quantityScroll:getValue()/20)*20
      end
    end
  end
  return item.price*quantityScroll:getValue()
end

local function getSellQuantity(item)
  if not playerItems[item.ptr:getId()] then
    return 0
  end

  local removeAmount = 0
  if ignoreEquipped:isChecked() then
    local localPlayer = g_game.getLocalPlayer()
    for i=1,LAST_INVENTORY do
      local inventoryItem = localPlayer:getInventoryItem(i)
      if inventoryItem and inventoryItem:getId() == item.ptr:getId() then
        removeAmount = removeAmount + inventoryItem:getCount()
      end
    end
  end
  return playerItems[item.ptr:getId()] - removeAmount
end

local function canTradeItem(item)
  if getCurrentTradeType() == BUY then
    return (ignoreCapacity:isChecked() or (not ignoreCapacity:isChecked() and playerFreeCapacity >= item.weight)) and playerMoney >= getItemPrice(item)
  else
    return getSellQuantity(item) > 0
  end
end

local function refreshItem(item)
  nameLabel:setText(item.name)
  weightLabel:setText(string.format('%.2f', item.weight) .. ' ' .. WEIGHT_UNIT)
  priceLabel:setText(getItemPrice(item) .. ' ' .. CURRENCY)

  quantityLabel:setText(1)
  quantityScroll:setValue(1)

  if getCurrentTradeType() == BUY then
    local capacityMaxCount = math.floor(playerFreeCapacity / item.weight)
    if ignoreCapacity:isChecked() then
      capacityMaxCount = 100
    end
    local priceMaxCount = math.floor(playerMoney / getItemPrice(item))
    quantityScroll:setMaximum(math.max(0, math.min(100, math.min(priceMaxCount, capacityMaxCount))))
  else
    local removeAmount = 0
    if ignoreEquipped:isChecked() then
      local localPlayer = g_game.getLocalPlayer()
      for i=1,LAST_INVENTORY do
        local inventoryItem = localPlayer:getInventoryItem(i)
        if inventoryItem and inventoryItem:getId() == item.ptr:getId() then
          removeAmount = removeAmount + inventoryItem:getCount()
        end
      end
    end
    quantityScroll:setMaximum(math.max(0, math.min(100, getSellQuantity(item))))
  end

  setupPanel:enable()
end

local function refreshTradeItems()
  local layout = itemsPanel:getLayout()
  layout:disableUpdates()

  clearSelectedItem()

  searchText:clearText()
  setupPanel:disable()
  itemsPanel:destroyChildren()

  if radioItems then
    radioItems:destroy()
  end
  radioItems = RadioGroup.create()

  local currentTradeItems = tradeItems[getCurrentTradeType()]
  for key,item in pairs(currentTradeItems) do
    local itemBox = createWidget('NPCItemBox', itemsPanel)
    itemBox.item = item

    local name = item.name
    local weight = string.format('%.2f', item.weight) .. ' ' .. WEIGHT_UNIT
    local price = item.price .. ' ' .. CURRENCY
    itemBox:setText(name .. '\n' .. weight .. '\n' .. price)

    local itemWidget = itemBox:getChildById('item')
    itemWidget:setItem(item.ptr)
    itemWidget.onMouseRelease = NPCTrade.itemPopup

    radioItems:addWidget(itemBox)
  end

  layout:enableUpdates()
  layout:update()
end

local function refreshPlayerGoods()
  moneyLabel:setText(playerMoney .. ' ' .. CURRENCY)
  capacityLabel:setText(string.format('%.2f', playerFreeCapacity) .. ' ' .. WEIGHT_UNIT)

  local currentTradeType = getCurrentTradeType()
  local searchFilter = searchText:getText():lower()
  local foundSelectedItem = false

  local items = itemsPanel:getChildCount()
  for i=1,items do
    local itemWidget = itemsPanel:getChildByIndex(i)
    local item = itemWidget.item

    local canTrade = canTradeItem(item)
    itemWidget:setEnabled(canTrade)

    local searchCondition = (searchFilter == '') or (searchFilter ~= '' and string.find(item.name:lower(), searchFilter) ~= nil)
    local showAllItemsCondition = (currentTradeType == BUY) or (showAllItems:isChecked()) or (currentTradeType == SELL and not showAllItems:isChecked() and canTrade)
    itemWidget:setVisible(searchCondition and showAllItemsCondition)

    if selectedItem == item and itemWidget:isEnabled() and itemWidget:isVisible() then
      foundSelectedItem = true
    end
  end

  if not foundSelectedItem then
    clearSelectedItem()
  end

  if selectedItem then
    refreshItem(selectedItem)
  end
end

-- hooked functions
local function onOpenNpcTrade(items)
  tradeItems[BUY] = {}
  tradeItems[SELL] = {}

  for key,item in pairs(items) do
    local newItem = {}
    newItem.ptr = item[1]
    newItem.name = item[2]
    newItem.weight = item[3] / 100

    if item[4] >= 0 then
      newItem.price = item[4]
      table.insert(tradeItems[BUY], newItem)
    elseif item[5] >= 0 then
      newItem.price = item[5]
      table.insert(tradeItems[SELL], newItem)
    else
      error("server error: item name " .. item[1] .. " has neither buy or sell price.")
    end
  end

  refreshTradeItems()
  addEvent(NPCTrade.show) -- player goods has not been parsed yet
end

local function onCloseNpcTrade()
  NPCTrade.hide()
end

local function onPlayerGoods(money, items)
  playerMoney = money

  playerItems = {}
  for key,item in pairs(items) do
    local id = item[1]:getId()
    if not playerItems[id] then
      playerItems[id] = item[2]
    else
      playerItems[id] = playerItems[id] + item[2]
    end
  end

  refreshPlayerGoods()
end

local function onFreeCapacityChange(localPlayer, freeCapacity, oldFreeCapacity)
  playerFreeCapacity = freeCapacity

  if npcWindow:isVisible() then
    refreshPlayerGoods()
  end
end

local function onInventoryChange(inventory, item, oldeItem)
  if selectedItem then
    refreshItem(selectedItem)
  end
end


-- public functions
function NPCTrade.init()
  npcWindow = displayUI('npctrade.otui')
  npcWindow:setVisible(false)

  itemsPanel = npcWindow:recursiveGetChildById('itemsPanel')
  searchText = npcWindow:recursiveGetChildById('searchText')

  setupPanel = npcWindow:recursiveGetChildById('setupPanel')
  quantityLabel = setupPanel:getChildById('quantity')
  quantityScroll = setupPanel:getChildById('quantityScroll')
  nameLabel = setupPanel:getChildById('name')
  priceLabel = setupPanel:getChildById('price')
  moneyLabel = setupPanel:getChildById('money')
  weightLabel = setupPanel:getChildById('weight')
  capacityLabel = setupPanel:getChildById('capacity')
  tradeButton = npcWindow:recursiveGetChildById('tradeButton')

  buyWithBackpack = npcWindow:recursiveGetChildById('buyWithBackpack')
  ignoreCapacity = npcWindow:recursiveGetChildById('ignoreCapacity')
  ignoreEquipped = npcWindow:recursiveGetChildById('ignoreEquipped')
  showAllItems = npcWindow:recursiveGetChildById('showAllItems')

  buyTab = npcWindow:getChildById('buyTab')
  sellTab = npcWindow:getChildById('sellTab')

  radioTabs = RadioGroup.create()
  radioTabs:addWidget(buyTab)
  radioTabs:addWidget(sellTab)
  radioTabs:selectWidget(buyTab)
  radioTabs.onSelectionChange = NPCTrade.onTradeTypeChange

  if g_game.isOnline() then -- event wont be sent again when reloading modules
    playerFreeCapacity = g_game.getLocalPlayer():getFreeCapacity()
  end

  connect(g_game, { onGameEnd = NPCTrade.hide,
                    onOpenNpcTrade = onOpenNpcTrade,
                    onCloseNpcTrade = onCloseNpcTrade,
                    onPlayerGoods = onPlayerGoods } )

  connect(LocalPlayer, { onFreeCapacityChange = onFreeCapacityChange,
                         onInventoryChange = onInventoryChange } )
end

function NPCTrade.terminate()
  --radioTabs:destroy()
  radioTabs = nil
  npcWindow:destroy()
  npcWindow = nil
  itemsPanel = nil
  buyButton = nil
  sellButton = nil
  searchText = nil
  buyTab = nil
  sellTab = nil

  setupPanel = nil
  quantityLabel = nil
  quantityScroll = nil
  nameLabel = nil
  priceLabel = nil
  moneyLabel = nil
  weightLabel = nil
  capacityLabel = nil
  offerSelected = nil
  tradeButton = nil

  disconnect(g_game, {  onGameEnd = NPCTrade.hide,
                        onOpenNpcTrade = onOpenNpcTrade,
                        onCloseNpcTrade = onCloseNpcTrade,
                        onPlayerGoods = onPlayerGoods } )

  disconnect(LocalPlayer, { onFreeCapacityChange = onFreeCapacityChange,
                            onInventoryChange = onInventoryChange } )

  NPCTrade = nil
end

function NPCTrade.show()
  if g_game.isOnline() then
    if #tradeItems[BUY] > 0 then
      radioTabs:selectWidget(buyTab)
    else
      radioTabs:selectWidget(sellTab)
    end

    npcWindow:show()
    npcWindow:raise()
    npcWindow:focus()
  end
end

function NPCTrade.hide()
  npcWindow:hide()
end

function NPCTrade.onItemBoxChecked(widget)
  if widget:isChecked() then
    local item = widget.item
    selectedItem = item
    refreshItem(item)
    tradeButton:enable()
  end
end

function NPCTrade.onQuantityValueChange(quantity)
  if quantityLabel and selectedItem then
    quantityLabel:setText(quantity)
    weightLabel:setText(string.format('%.2f', selectedItem.weight*quantity) .. ' ' .. WEIGHT_UNIT)
    priceLabel:setText(getItemPrice(selectedItem) .. ' ' .. CURRENCY)
  end
end

function NPCTrade.onTradeTypeChange(radioTabs, selected, deselected)
  tradeButton:setText(selected:getText())
  selected:setOn(true)
  deselected:setOn(false)

  local currentTradeType = getCurrentTradeType()
  buyWithBackpack:setVisible(currentTradeType == BUY)
  ignoreCapacity:setVisible(currentTradeType == BUY)
  ignoreEquipped:setVisible(currentTradeType == SELL)
  showAllItems:setVisible(currentTradeType == SELL)

  refreshTradeItems()
  refreshPlayerGoods()
end

function NPCTrade.onTradeClick()
  if getCurrentTradeType() == BUY then
    g_game.buyItem(selectedItem.ptr, quantityScroll:getValue(), buyWithBackpack:isChecked(), ignoreCapacity:isChecked())
  else
    g_game.sellItem(selectedItem.ptr, quantityScroll:getValue(), ignoreEquipped:isChecked())
  end
end

function NPCTrade.onSearchTextChange()
  refreshPlayerGoods()
end

function NPCTrade.itemPopup(self, mousePosition, mouseButton)
  if mouseButton == MouseRightButton then
    local menu = createWidget('PopupMenu')
    menu:addOption(tr('Look'), function() return g_game.inspectNpcTrade(self:getItem()) end)
    menu:display(mousePosition)
    return true
  end
  return false
end

function NPCTrade.onBuyWithBackpackChange()
  if selectedItem then
    refreshItem(selectedItem)
  end
end

function NPCTrade.onIgnoreCapacityChange()
  refreshPlayerGoods()
end

function NPCTrade.onIgnoreEquippedChange()
  refreshPlayerGoods()
end

function NPCTrade.onShowAllItemsChange()
  refreshPlayerGoods()
end
