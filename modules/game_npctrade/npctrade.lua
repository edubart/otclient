BUY = 1
SELL = 2
CURRENCY = 'gold'
WEIGHT_UNIT = 'oz'
LAST_INVENTORY = 10

npcWindow = nil
itemsPanel = nil
radioTabs = nil
radioItems = nil
searchText = nil
setupPanel = nil
quantity = nil
quantityScroll = nil
nameLabel = nil
priceLabel = nil
moneyLabel = nil
weightLabel = nil
capacityLabel = nil
tradeButton = nil
buyTab = nil
sellTab = nil

showCapacity = true
buyWithBackpack = nil
ignoreCapacity = nil
ignoreEquipped = nil
showAllItems = nil

playerFreeCapacity = nil
playerMoney = nil
tradeItems = {}
playerItems = nil
selectedItem = nil

function init()
  npcWindow = g_ui.displayUI('npctrade.otui')
  npcWindow:setVisible(false)

  itemsPanel = npcWindow:recursiveGetChildById('itemsPanel')
  searchText = npcWindow:recursiveGetChildById('searchText')

  setupPanel = npcWindow:recursiveGetChildById('setupPanel')
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

  radioTabs = UIRadioGroup.create()
  radioTabs:addWidget(buyTab)
  radioTabs:addWidget(sellTab)
  radioTabs:selectWidget(buyTab)
  radioTabs.onSelectionChange = onTradeTypeChange

  if g_game.isOnline() then
    playerFreeCapacity = g_game.getLocalPlayer():getFreeCapacity()
  end

  connect(g_game, { onGameEnd = hide,
                    onOpenNpcTrade = onOpenNpcTrade,
                    onCloseNpcTrade = onCloseNpcTrade,
                    onPlayerGoods = onPlayerGoods } )

  connect(LocalPlayer, { onFreeCapacityChange = onFreeCapacityChange,
                         onInventoryChange = onInventoryChange } )
end

function terminate()
  npcWindow:destroy()

  disconnect(g_game, {  onGameEnd = hide,
                        onOpenNpcTrade = onOpenNpcTrade,
                        onCloseNpcTrade = onCloseNpcTrade,
                        onPlayerGoods = onPlayerGoods } )

  disconnect(LocalPlayer, { onFreeCapacityChange = onFreeCapacityChange,
                            onInventoryChange = onInventoryChange } )
end

function show()
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

function hide()
  npcWindow:hide()
end

function onItemBoxChecked(widget)
  if widget:isChecked() then
    local item = widget.item
    selectedItem = item
    refreshItem(item)
    tradeButton:enable()
  end
end

function onQuantityValueChange(quantity)
  if selectedItem then
    weightLabel:setText(string.format('%.2f', selectedItem.weight*quantity) .. ' ' .. WEIGHT_UNIT)
    priceLabel:setText(getItemPrice(selectedItem) .. ' ' .. CURRENCY)
  end
end

function onTradeTypeChange(radioTabs, selected, deselected)
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

function onTradeClick()
  if getCurrentTradeType() == BUY then
    g_game.buyItem(selectedItem.ptr, quantityScroll:getValue(), ignoreCapacity:isChecked(), buyWithBackpack:isChecked())
  else
    g_game.sellItem(selectedItem.ptr, quantityScroll:getValue(), ignoreEquipped:isChecked())
  end
end

function onSearchTextChange()
  refreshPlayerGoods()
end

function itemPopup(self, mousePosition, mouseButton)
  if mouseButton == MouseRightButton then
    local menu = g_ui.createWidget('PopupMenu')
    menu:addOption(tr('Look'), function() return g_game.inspectNpcTrade(self:getItem()) end)
    menu:display(mousePosition)
    return true
  end
  return false
end

function onBuyWithBackpackChange()
  if selectedItem then
    refreshItem(selectedItem)
  end
end

function onIgnoreCapacityChange()
  refreshPlayerGoods()
end

function onIgnoreEquippedChange()
  refreshPlayerGoods()
end

function onShowAllItemsChange()
  refreshPlayerGoods()
end

function setCurrency(currency)
  CURRENCY = currency
end

function showCapacity(state)
  showCapacity = state
end

function clearSelectedItem()
  nameLabel:clearText()
  weightLabel:clearText()
  priceLabel:clearText()
  tradeButton:disable()
  quantityScroll:setMaximum(1)
  if selectedItem then
    radioItems:selectWidget(nil)
    selectedItem = nil
  end
end

function getCurrentTradeType()
  if tradeButton:getText() == tr('Buy') then
    return BUY
  else
    return SELL
  end
end

function getItemPrice(item, single)
  local amount = 1
  local single = single or false
  if not single then
    amount = quantityScroll:getValue()
  end
  if getCurrentTradeType() == BUY then
    if buyWithBackpack:isChecked() then
      if item.ptr:isStackable() then
          return item.price*amount + 20
      else
        return item.price*amount + math.ceil(amount/20)*20
      end
    end
  end
  return item.price*amount
end

function getSellQuantity(item)
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

function canTradeItem(item)
  if getCurrentTradeType() == BUY then
    return (ignoreCapacity:isChecked() or (not ignoreCapacity:isChecked() and playerFreeCapacity >= item.weight)) and playerMoney >= getItemPrice(item, true)
  else
    return getSellQuantity(item) > 0
  end
end

function refreshItem(item)
  nameLabel:setText(item.name)
  weightLabel:setText(string.format('%.2f', item.weight) .. ' ' .. WEIGHT_UNIT)
  priceLabel:setText(getItemPrice(item) .. ' ' .. CURRENCY)

  if getCurrentTradeType() == BUY then
    local capacityMaxCount = math.floor(playerFreeCapacity / item.weight)
    if ignoreCapacity:isChecked() then
      capacityMaxCount = 100
    end
    local priceMaxCount = math.floor(playerMoney / getItemPrice(item, true))
    local finalCount = math.max(0, math.min(100, math.min(priceMaxCount, capacityMaxCount)))
    quantityScroll:setMaximum(finalCount)

    if quantityScroll:getValue() > finalCount then
      quantityScroll:setValue(finalCount)
    end
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

function refreshTradeItems()
  local layout = itemsPanel:getLayout()
  layout:disableUpdates()

  clearSelectedItem()

  searchText:clearText()
  setupPanel:disable()
  itemsPanel:destroyChildren()

  if radioItems then
    radioItems:destroy()
  end
  radioItems = UIRadioGroup.create()

  local currentTradeItems = tradeItems[getCurrentTradeType()]
  for key,item in pairs(currentTradeItems) do
    local itemBox = g_ui.createWidget('NPCItemBox', itemsPanel)
    itemBox.item = item

    local text = ''
    local name = item.name
    text = text .. name
    if showCapacity then
      local weight = string.format('%.2f', item.weight) .. ' ' .. WEIGHT_UNIT
      text = text .. '\n' .. weight
    end
    local price = item.price .. ' ' .. CURRENCY
    text = text .. '\n' .. price
    itemBox:setText(text)

    local itemWidget = itemBox:getChildById('item')
    itemWidget:setItem(item.ptr)
    itemWidget.onMouseRelease = itemPopup

    radioItems:addWidget(itemBox)
  end

  layout:enableUpdates()
  layout:update()
end

function refreshPlayerGoods()
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

function onOpenNpcTrade(items)
  tradeItems[BUY] = {}
  tradeItems[SELL] = {}

  for key,item in pairs(items) do
    
    if item[4] > 0 then
      local newItem = {}
      newItem.ptr = item[1]
      newItem.name = item[2]
      newItem.weight = item[3] / 100
      newItem.price = item[4]
      table.insert(tradeItems[BUY], newItem)
    end
    
    if item[5] > 0 then
      local newItem = {}
      newItem.ptr = item[1]
      newItem.name = item[2]
      newItem.weight = item[3] / 100
      newItem.price = item[5]
      table.insert(tradeItems[SELL], newItem)
    end
  end

  refreshTradeItems()
  addEvent(show) -- player goods has not been parsed yet
end

function closeNpcTrade()
  g_game.closeNpcTrade()
  hide()
end

function onCloseNpcTrade()
  hide()
end

function onPlayerGoods(money, items)
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

function onFreeCapacityChange(localPlayer, freeCapacity, oldFreeCapacity)
  playerFreeCapacity = freeCapacity

  if npcWindow:isVisible() then
    refreshPlayerGoods()
  end
end

function onInventoryChange(inventory, item, oldItem)
  if selectedItem then
    refreshItem(selectedItem)
  end
end

