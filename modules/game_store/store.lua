Store = {}



-- LOCAL GLOBALS
-- prefer local variables over globals for better code organization

local PROTOCOL = runinsandbox('storeprotocol')
local STORE_BUTTON
local STORE_WINDOW
local OPTION_CHANGE_HANDLERS = {}
local IS_WAITING_ON_USER_INPUT_FLAG = false
local PATH_TO_ICONS = '/images/store/64/'
local DEFAULT_OFFER_ICON_NAME = 'unknown' -- is localed at PATH_TO_ICONS and is a PNG-file
local TOP_BUTTON_ICON_PATH = '/images/topbuttons/chest'
local STYLES_TABLE = {
  'ui/general/storecombobox',
  'ui/general/storealertwindow',
  'ui/general/storepromptwindow',
  'store'
}
local COIN_BALANCE_LABEL
local BROWSE_PANEL
local CATEGORY_LIST
local ITEM_DETAILS_PANEL

local function initInterface()
  local mainTabBar = STORE_WINDOW:getChildById('mainTabBar')
  local mainTabContentPanel = STORE_WINDOW:getChildById('mainTabContentPanel')
  mainTabBar:setContentWidget(mainTabContentPanel)

  COIN_BALANCE_LABEL = STORE_WINDOW:getChildById('balanceLabel')

  BROWSE_PANEL = g_ui.loadUI('ui/storeoffers/browse')
  mainTabBar:addTab(tr('Browse'), BROWSE_PANEL)

  CATEGORY_LIST = BROWSE_PANEL:getChildById('categoryComboBox')


  ITEM_DETAILS_PANEL = BROWSE_PANEL:getChildById('itemDetailsPanel')
  local itemDetailsPanelTabBar = BROWSE_PANEL:getChildById('itemDetailsPanelTabBar')
  itemDetailsPanelTabBar:addTab(tr('Item Details'), ITEM_DETAILS_PANEL)


  local transactionHistoryPanel = g_ui.loadUI('ui/myoffers/offerhistory')
  mainTabBar:addTab(tr('Transaction History'), transactionHistoryPanel)
end

local function getImageSourcePathFromOfferIcon(icon)
  local path, count = string.gsub( PATH_TO_ICONS .. icon, ".png", "" )

  if g_resources.fileExists(path .. '.png') then
    return path
  else
    return PATH_TO_ICONS .. DEFAULT_OFFER_ICON_NAME
  end
end

function init()
  local function toggle() return 0 end

  for i, style in pairs(STYLES_TABLE) do
    g_ui.importStyle(style)
  end

  STORE_BUTTON = modules.client_topmenu.addRightGameToggleButton('storeButton', tr('Store'), TOP_BUTTON_ICON_PATH, toggle)
  PROTOCOL.initProtocol()
  STORE_WINDOW = g_ui.createWidget('StoreWindow', rootWidget)
  STORE_WINDOW:hide()

  initInterface()

  STORE_BUTTON.onClick = function()
    if STORE_BUTTON:isOn() == true then
      Store.close()
    else
      Store.open()
    end
  end

  connect(g_game, { onGameEnd = Store.close })
end

function terminate()
  PROTOCOL.terminateProtocol()
  Store.close()
  disconnect(g_game, { onGameEnd = Store.close })
end

function Store.clearOffers()
  local itemsPanel = BROWSE_PANEL:recursiveGetChildById('itemsPanel')
  itemsPanel:destroyChildren()
end

function Store.close()
  STORE_WINDOW:hide()
  STORE_BUTTON:setOn(false)
  Store.clearOffers()
  modules.game_interface.getRootPanel():focus()
  CATEGORY_LIST.onOptionChange = nil
end

function Store.open()
  STORE_BUTTON:setOn(true)
  Store.hideItemDetails()
  Store.showStoreIsUpdatingMessage()

  if lastCategory then -- assuming that the category didn't get removed server-side...
    -- update categories in case the shop has been updated
    --StoreProtocol.sendOpenStore() -- changes will be reflected either when the user restarts the client, or reloads the module...
    StoreProtocol.sendRequestStoreOffers(lastCategory)
  else
    StoreProtocol.sendOpenStore()
  end
  STORE_WINDOW:show()
  STORE_WINDOW:focus()
end

function Store.onCheckChange(itemBox)
  local offer = itemBox.offer

  local selectedItem = ITEM_DETAILS_PANEL:getChildById('selectedItem')
  local itemNameLabel = ITEM_DETAILS_PANEL:getChildById('itemName')
  local itemPriceLabel = ITEM_DETAILS_PANEL:getChildById('itemPrice')
  local itemAmountLabel = ITEM_DETAILS_PANEL:getChildById('itemAmount')
  local itemDescription = ITEM_DETAILS_PANEL:recursiveGetChildById('itemDescription')
  local buyButton = ITEM_DETAILS_PANEL:getChildById('buyButton')

  selectedItem:setImageSource(getImageSourcePathFromOfferIcon(offer.icons[1]))
  itemNameLabel:setText(tr("Name") .. ": " .. offer.name)
  itemPriceLabel:setText(tr("Price") .. ": " .. offer.price .. ' ' .. tr("coins"))
  itemAmountLabel:setText(tr("Amount") .. ": x" .. offer.amount)

  if offer.description and offer.description ~= "" then
    itemDescription:setText(offer.description)
  else
    itemDescription:setText(tr("This item has no description"))
  end

  itemDescription:resizeToText()

  buyButton.onClick = function()
    Store.purchaseHandler(offer.id, offer.type)
  end

  Store.showItemDetails()
end

function Store.hideItemDetails()
  local children = ITEM_DETAILS_PANEL:getChildren()
  ITEM_DETAILS_PANEL:setText(tr("Select an item to preview details and make a purchase"))

  for i, child in pairs(children) do
    child:hide()
  end
end

function Store.showItemDetails()
  local children = ITEM_DETAILS_PANEL:getChildren()
  ITEM_DETAILS_PANEL:setText("")

  for i, child in pairs(children) do
    child:show()
  end
end

-- fires when 1) user presses the buy button and when 2) onRequestPurchaseData is fired
function Store.purchaseHandler(thingId, offerType)
  STORE_WINDOW:lock()

  if offerType == StoreProtocol.OfferTypes.OFFER_TYPE_NAMECHANGE then
    if not IS_WAITING_ON_USER_INPUT_FLAG then
      -- use a flag to prevent duplicate prompts in case server messes up
      IS_WAITING_ON_USER_INPUT_FLAG = true

      Store.showNameInputPrompt(function(name)
        StoreProtocol.sendBuyStoreOffer(thingId, StoreProtocol.ClientOfferTypes.CLIENT_STORE_OFFER_NAMECHANGE, name)
        IS_WAITING_ON_USER_INPUT_FLAG = false
      end, function()
        IS_WAITING_ON_USER_INPUT_FLAG = false
      end)
    end
  else
    StoreProtocol.sendBuyStoreOffer(thingId, offerType)
  end
end

-- multipurpose way to display an alert containing an arbitrary long message to the user; feel free to use
function Store.showAlertWindow(message, onClickHandler)
  local alert = g_ui.createWidget('StoreAlertWindow', rootWidget)
  local alertMessage = alert:recursiveGetChildById('alert')
  local confirmButton = alert:getChildById('confirmButton')
  alertMessage:setText(message)
  alertMessage:resizeToText()
  alert:lock()
  STORE_WINDOW:unlock()
  alert:focus()

  confirmButton.onClick = function()
    if onClickHandler then onClickHandler(alert) end

    STORE_WINDOW:focus()
    alert:unlock()
    alert:destroy()
  end
end

-- TODO: make it generic
function Store.showNameInputPrompt(onConfirmHandler, onCancelHandler)
  local promptWindow = g_ui.createWidget('StorePromptWindow', rootWidget)
  promptWindow:show()
  promptWindow:focus()

  local confirmButton = promptWindow:recursiveGetChildById('confirmButton')
  local cancelButton = promptWindow:recursiveGetChildById('cancelButton')
  local nameInput = promptWindow:recursiveGetChildById('nameInput')

  confirmButton.onClick = function()
    local name = nameInput:getDisplayedText()
    if onConfirmHandler then onConfirmHandler(name) end

    promptWindow:hide()
    promptWindow:destroy()
    STORE_WINDOW:unlock()
  end

  cancelButton.onClick = function()
    if onCancelHandler then onCancelHandler() end

    promptWindow:hide()
    promptWindow:destroy()
    STORE_WINDOW:unlock()
  end
end

function Store.setItemsPanelText(text)
  local itemsPanel = BROWSE_PANEL:recursiveGetChildById('itemsPanel')
  itemsPanel:setText(text)
end

function Store.showStoreIsUpdatingMessage()
  Store.setItemsPanelText(tr("Fetching items..."))
end

function Store.hideStoreIsUpdatingMessage()
  Store.setItemsPanelText("")
end

-- fires when users selects another option (category) from the select list
function Store.onOptionChange()
  Store.clearOffers()
  Store.showStoreIsUpdatingMessage()

  lastCategory = CATEGORY_LIST:getCurrentOption().text
  for handlerName,handler in pairs(OPTION_CHANGE_HANDLERS) do
    if handlerName == CATEGORY_LIST:getCurrentOption().text then
      handler()
    end
  end
end

function Store.updateBalance(balance)
  COIN_BALANCE_LABEL:setText(tr('Coin Balance: ') .. balance)
  COIN_BALANCE_LABEL:resizeToText()
end



-- PACKET HANDLERS
-- these functions are called in StoreProtocol and get fired after we receive and parse a packet

function Store.onCoinBalance(balance)
  Store.updateBalance(balance)
end

function Store.onStoreError(error, message)
  Store.showAlertWindow(message)
end

function Store.onRequestPurchaseData(offerId, clientOfferType)
  -- print('onRequestPurchaseData')
  --Store.purchaseHandler(offerId, clientOfferType)
end

function Store.onCoinBalanceUpdating()
  -- print('onCoinBalanceUpdating')
end

function Store.onOpenStore(categories)
  CATEGORY_LIST:clearOptions()
  OPTION_CHANGE_HANDLERS = {}

  for k,category in pairs(categories) do
    if k == #category then break end
    local catName = category.name
    -- register a listener for when this category gets selected
    OPTION_CHANGE_HANDLERS[catName] = function()
      StoreProtocol.sendRequestStoreOffers(catName)
    end

    CATEGORY_LIST:addOption(catName)
    CATEGORY_LIST.onOptionChange = Store.onOptionChange
  end
end

function Store.onStoreOffers(category)
  -- print('got ', #category.offers, ' offers')

  local itemsPanel = BROWSE_PANEL:recursiveGetChildById('itemsPanel')

  local layout = itemsPanel:getLayout()
  Store.clearOffers()
  Store.showStoreIsUpdatingMessage()
  layout:disableUpdates()
  Store.hideItemDetails()

  local radioItemSet = UIRadioGroup.create()
  local enabledOfferCount = 0

  for i,offer in pairs(category.offers) do

    if offer.type ~= 255 then
      -- setup item box and its event handling
      local itemBox = g_ui.createWidget('StoreItemBox', itemsPanel)
      itemBox.offer = offer -- well need this to handle the checkChangeEvent

      -- display amount
      local nameLabel = itemBox:getChildById('nameLabel')
      if offer.type == StoreProtocol.OfferTypes.OFFER_TYPE_PREMIUM then
        nameLabel:setText(offer.amount .. " " .. tr("days"))
      else
        nameLabel:setText(offer.amount .. 'x')
      end

      -- display price
      local priceLabel = itemBox:getChildById('priceLabel')
      priceLabel:setText(offer.price .. " " .. tr("coins"))


      local itemWidget = itemBox:getChildById('item')
      itemWidget:setMarginLeft(3)
      itemWidget:setMarginTop(3)

      local iconPath = getImageSourcePathFromOfferIcon(offer.icons[1]) -- TODO: check for gender when offertype is outfit
      itemWidget:setImageSource(iconPath)

      if offer.type == StoreProtocol.OfferTypes.OFFER_TYPE_NONE then
        itemBox:disable()
        itemBox:setTooltip(offer.disableReason)
      else
        itemBox.onCheckChange = Store.onCheckChange
      end

      radioItemSet:addWidget(itemBox)
      enabledOfferCount = enabledOfferCount + 1
    end
  end

  layout:enableUpdates()

  Store.hideStoreIsUpdatingMessage()
  if enabledOfferCount == 0 and #category.offers > 0 then
    Store.setItemsPanelText(tr("You already own all the offers from this category"))
  elseif #category.offers == 0 then
    Store.setItemsPanelText(tr("There are currently no offers in this category"))
  end

  layout:update()
end

function Store.onOpenTransactionHistory()
  --print('onOpenTransactionHistory')
end

function Store.onCompletePurchase(message, balance)
  Store.updateBalance(balance)

  -- We need to update the offer list because some items such as outfits can be bought only once
  -- and thus should be removed from the UI. Using this window we capture a click event from user
  -- and update the offer list automatically.
  Store.showAlertWindow(message, function()
    local category = CATEGORY_LIST:getCurrentOption().text
    StoreProtocol.sendRequestStoreOffers(category)
  end)
end