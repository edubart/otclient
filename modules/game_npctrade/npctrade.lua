NPCTrade = {}

local npcWindow
local itemsPanel
local radioTabs
local radioItems
local buyTab
local sellTab
local searchText
local setupPanel
local quantity
local quantityScroll
local nameLabel
local priceLabel
local moneyGoods
local moneyLabel
local weightLabel
local capacityLabel
local offerSelected
local setupButton
local cacheItems
local cacheGoods

local buyWithBackpack = false
local ignoreCapacity = false
local ignoreEquipped = true
local showOnlyHolding = false

local CURRENCY = 'gold'
local WEIGHT_UNIT = 'oz'

-- public functions
function NPCTrade.init()
  cacheItems = {}
  cacheGoods = {}

  npcWindow = displayUI('npctrade.otui')
  npcWindow:setVisible(false)

  itemsPanel = npcWindow:recursiveGetChildById('itemsPanel')
  buyTab = npcWindow:getChildById('buyTab')
  sellTab = npcWindow:getChildById('sellTab')
  searchText = npcWindow:getChildById('searchText')

  setupPanel = npcWindow:recursiveGetChildById('setupPanel')
  quantityLabel = setupPanel:getChildById('quantity')
  quantityScroll = setupPanel:getChildById('quantityScroll')
  nameLabel = setupPanel:getChildById('name')
  priceLabel = setupPanel:getChildById('price')
  moneyLabel = setupPanel:getChildById('money')
  weightLabel = setupPanel:getChildById('weight')
  capacityLabel = setupPanel:getChildById('capacity')
  setupButton = setupPanel:getChildById('setupButton')

  radioTabs = RadioGroup.create()
  radioTabs:addWidget(buyTab)
  radioTabs:addWidget(sellTab)
  radioTabs:selectWidget(buyTab)
  radioTabs.onSelectionChange = NPCTrade.setList

  connect(g_game, { onGameEnd = NPCTrade.hide,
                    onOpenNpcTrade = NPCTrade.onOpenNpcTrade,
                    onPlayerGoods = NPCTrade.onPlayerGoods,
                    onCloseNpcTrade = NPCTrade.onCloseNpcTrade } )
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

  setupPanel = nil
  quantityLabel = nil
  quantityScroll = nil
  nameLabel = nil
  priceLabel = nil
  moneyLabel = nil
  weightLabel = nil
  capacityLabel = nil
  offerSelected = nil
  setupButton = nil

  cacheItems = nil
  cacheGoods = nil
  buyTab = nil
  sellTab = nil

  disconnect(g_game, {  onGameEnd = NPCTrade.hide,
                        onOpenNpcTrade = NPCTrade.onOpenNpcTrade,
                        onPlayerGoods = NPCTrade.onPlayerGoods,
                        onCloseNpcTrade = NPCTrade.onCloseNpcTrade } )

  NPCTrade = nil
end

-- private functions
function NPCTrade.show()
  if g_game.isOnline() then
    npcWindow:show()
    npcWindow:raise()
  end
end

function NPCTrade.hide()
  npcWindow:hide()
end

function NPCTrade.setList(radioTabs, selected, deselected)
  setupButton:setText(selected:getText())
  selected:setOn(true)
  deselected:setOn(false)
  NPCTrade.createItemsOnPanel()

  NPCTrade.resetSetup()
  NPCTrade.refreshItemsPanel()
  NPCTrade.refreshFilters()
end

function NPCTrade.resetSetup()
  nameLabel:clearText()
  weightLabel:clearText()
  priceLabel:clearText()
  searchText:clearText()
  if offerSelected then
    radioItems.selectedWidget:setChecked(false)
    offerSelected = nil
  end
  setupPanel:disable()
end

function NPCTrade.updateSetup()
  if offerSelected then
    if radioItems.selectedWidget:isEnabled() then
      if setupButton:getText() == tr('Buy') then
        local capacityMaxCount = math.floor(100*g_game.getLocalPlayer():getFreeCapacity()/offerSelected[3])
        local priceMaxCount = math.floor(moneyGoods/NPCTrade.getOfferPrice(offerSelected))
        quantityScroll:setMaximum(math.max(0, math.min(100, math.min(priceMaxCount, capacityMaxCount))))
      else
        if cacheGoods[offerSelected[1]:getId()] then -- list might be empty.
          quantityScroll:setMaximum(math.max(0, math.min(100, cacheGoods[offerSelected[1]:getId()])))
        end
      end
    else
      NPCTrade.resetSetup()
    end
  end
end

function NPCTrade.getOfferPrice(offer)
  if setupButton:getText() == tr('Buy') then
    local price = offer[4]
    if buyWithBackpack then
      if offer[1]:isStackable() then
        return price*quantityScroll:getValue() + 20;
      else
        return price*quantityScroll:getValue() + math.ceil(quantityScroll:getValue()/20)*20
      end
    else
      return price*quantityScroll:getValue()
    end
  else
    return offer[5]*quantityScroll:getValue()
  end
end

function NPCTrade.setItem(widget)
  offerSelected = widget.offer

  local offer = widget.offer
  local price = NPCTrade.getOfferPrice(widget.offer)
  local freeCapacity = g_game.getLocalPlayer():getFreeCapacity()
  nameLabel:setText(offer[2])
  weightLabel:setText(string.format('%.2f', offer[3]/100) .. ' ' .. WEIGHT_UNIT)
  priceLabel:setText(price .. ' ' .. CURRENCY)
  capacityLabel:setText(string.format('%.2f', freeCapacity) .. ' ' .. WEIGHT_UNIT)

  quantityLabel:setText(1)
  quantityScroll:setValue(1)
  NPCTrade.updateSetup()

  setupPanel:enable()
end

function NPCTrade.setQuantity(quantity)
  if quantityLabel and offerSelected then
    local price = NPCTrade.getOfferPrice(offerSelected)
    quantityLabel:setText(quantity)
    weightLabel:setText(string.format('%.2f', offerSelected[3]*quantity/100) .. ' ' .. WEIGHT_UNIT)
    priceLabel:setText(price .. ' ' .. CURRENCY)
  end
end

function NPCTrade.setupButton()
  if setupButton:getText() == 'Buy' then
    g_game.buyItem(offerSelected[1], quantityScroll:getValue(), buyWithBackpack, ignoreCapacity)
  else
    g_game.sellItem(offerSelected[1], quantityScroll:getValue(), ignoreEquipped)
  end
end

function NPCTrade.onOpenNpcTrade(items)
  -- items[item] = item
  -- item[1] = ItemPtr
  -- item[2] = name
  -- item[3] = weight
  -- item[4] = buyPrice
  -- item[5] = sellPrice

  cacheItems = items

  NPCTrade.createItemsOnPanel()

  NPCTrade.show()
end

function NPCTrade.switchBuyWithBackpack()
  buyWithBackpack = not buyWithBackpack
  if offerSelected then
    priceLabel:setText(NPCTrade.getOfferPrice(offerSelected) .. ' ' .. CURRENCY)
  end
  return true
end

function NPCTrade.itemPopup(self, mousePosition, mouseButton)
  if mouseButton == MouseRightButton then
    local menu = createWidget('PopupMenu')
    menu:addOption(tr('Look'), function() return g_game.inspectNpcTrade(self.offer[1]) end)
    menu:addSeparator()
    if setupButton:getText() == tr('Buy') then
      menu:addOption((buyWithBackpack and tr('Buy no backpack') or tr('Buy with backpack')), NPCTrade.switchBuyWithBackpack)
      menu:addOption((ignoreCapacity and tr('Consider capacity') or tr('Ignore capacity')), function() ignoreCapacity = not ignoreCapacity return true end)
    else
      menu:addOption((ignoreEquipped and tr('Consider equipped') or tr('Ignore equipped')), function() ignoreEquipped = not ignoreEquipped return true end)
      menu:addOption((showOnlyHolding and tr('Show all items') or tr('Show only holding items')), function() showOnlyHolding = not showOnlyHolding NPCTrade.refreshFilters() return true end)
    end
    menu:display(mousePosition)
  end
end

function NPCTrade.createItemsOnPanel()
  local layout = itemsPanel:getLayout()
  layout:disableUpdates()

  NPCTrade.resetSetup()

  offerSelected = nil
  itemsPanel:destroyChildren()

  if radioItems then
    radioItems:destroy()
  end
  radioItems = RadioGroup.create()

  for i, v in pairs(cacheItems) do
    local price = NPCTrade.getOfferPrice(v)
    if price > 0 then
      local itemBox = createWidget('NPCItemBox', itemsPanel)
      itemBox.offer = v
      itemBox:getChildById('item'):setItem(v[1])
      itemBox:getChildById('nameLabel'):setText(v[2])
      itemBox:getChildById('weightLabel'):setText(string.format('%.2f', v[3]/100) .. ' ' .. WEIGHT_UNIT)
      itemBox:getChildById('priceLabel'):setText(price .. ' ' .. CURRENCY)

      itemBox.onMouseRelease = NPCTrade.itemPopup
      itemBox:getChildById('item').onMouseRelease = function (self, mousePosition, mouseButton) NPCTrade.itemPopup(itemBox, mousePosition, mouseButton) end

      radioItems:addWidget(itemBox)
    end
  end

  layout:enableUpdates()
  layout:update()
end

function NPCTrade.extraFilters(widget, showOnlyHolding)
  if setupButton:getText() == tr('Sell') then
    if not showOnlyHolding or cacheGoods[widget.offer[1]:getId()] then
      return true
    else
      return false
    end
  end

  return true
end

function NPCTrade.refreshFilters()
  local filter = searchText:getText()
  local items = itemsPanel:getChildCount()
  for i = 1, items do
    local itemWidget = itemsPanel:getChildByIndex(i)
    if filter ~= '' then
      if string.find(itemWidget.offer[2], filter) and NPCTrade.extraFilters(itemWidget, showOnlyHolding) then
        itemWidget:show()
      else
        itemWidget:hide()
      end
    else
      if NPCTrade.extraFilters(itemWidget, showOnlyHolding) then
        itemWidget:show()
      else
        itemWidget:hide()
      end
    end
  end
end

function NPCTrade.refreshItemsPanel()
  if setupButton:getText() == tr('Buy') then
    local items = itemsPanel:getChildCount()
    for i = 1, items do
      local itemWidget = itemsPanel:getChildByIndex(i)
      if moneyGoods < NPCTrade.getOfferPrice(itemWidget.offer) then
        itemWidget:disable()
      else
        itemWidget:enable()
      end
    end
  else
    local items = itemsPanel:getChildCount()
    for i = 1, items do
      local itemWidget = itemsPanel:getChildByIndex(i)
      if cacheGoods[itemWidget.offer[1]:getId()] then
        itemWidget:enable()
      else
        itemWidget:disable()
      end
    end
  end
end

function NPCTrade.onPlayerGoods(money, goods)
  moneyGoods = money

  moneyLabel:setText(money .. ' ' .. CURRENCY)
  local freeCapacity = g_game.getLocalPlayer():getFreeCapacity()
  capacityLabel:setText(string.format('%.2f', freeCapacity) .. ' ' .. WEIGHT_UNIT)

  cacheGoods = {}
  for i,v in pairs(goods) do
    cacheGoods[v[1]:getId()] = v[2]
  end

  NPCTrade.refreshItemsPanel()
  NPCTrade.updateSetup()
end

function NPCTrade.onCloseNpcTrade()
  NPCTrade.hide()
end
