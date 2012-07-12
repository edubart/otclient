Inventory = {}

-- public variables
InventorySlotStyles = {
  [InventorySlotHead] = "HeadSlot",
  [InventorySlotNeck] = "NeckSlot",
  [InventorySlotBack] = "BackSlot",
  [InventorySlotBody] = "BodySlot",
  [InventorySlotRight] = "RightSlot",
  [InventorySlotLeft] = "LeftSlot",
  [InventorySlotLeg] = "LegSlot",
  [InventorySlotFeet] = "FeetSlot",
  [InventorySlotFinger] = "FingerSlot",
  [InventorySlotAmmo] = "AmmoSlot"
}

-- private variables
local inventoryWindow
local inventoryPanel
local inventoryButton

-- public functions
function Inventory.init()
  connect(LocalPlayer, { onInventoryChange = Inventory.onInventoryChange })
  connect(g_game, { onGameStart = Inventory.refresh })

  g_keyboard.bindKeyDown('Ctrl+I', Inventory.toggle)

  inventoryWindow = g_ui.loadUI('inventory.otui', GameInterface.getRightPanel())
  inventoryWindow:disableResize()
  inventoryPanel = inventoryWindow:getChildById('contentsPanel')
  inventoryButton = TopMenu.addRightGameToggleButton('inventoryButton', tr('Inventory') .. ' (Ctrl+I)', 'inventory.png', Inventory.toggle)
  inventoryButton:setOn(true)

  Inventory.refresh()
end

function Inventory.terminate()
  disconnect(LocalPlayer, { onInventoryChange = Inventory.onInventoryChange })
  disconnect(g_game, { onGameStart = Inventory.refresh })

  g_keyboard.unbindKeyDown('Ctrl+I')

  inventoryWindow:destroy()
  inventoryButton:destroy()
  inventoryWindow = nil
  inventoryButton = nil
  inventoryPanel = nil

  Inventory = nil
end

function Inventory.refresh()
  local player = g_game.getLocalPlayer()
  for i=InventorySlotFirst,InventorySlotLast do
    if player then
      Inventory.onInventoryChange(player, i, player:getInventoryItem(i))
    else
      Inventory.onInventoryChange(player, i, nil)
    end
  end
end

function Inventory.toggle()
  if inventoryButton:isOn() then
    inventoryWindow:close()
    inventoryButton:setOn(false)
  else
    inventoryWindow:open()
    inventoryButton:setOn(true)
  end
end

function Inventory.onMiniWindowClose()
  inventoryButton:setOn(false)
end

-- hooked events
function Inventory.onInventoryChange(player, slot, item, oldItem)
  local itemWidget = inventoryPanel:getChildById('slot' .. slot)
  if(item) then
    itemWidget:setStyle('Item')
    itemWidget:setItem(item)
  else
    itemWidget:setStyle(InventorySlotStyles[slot])
    itemWidget:setItem(nil)
  end
end
