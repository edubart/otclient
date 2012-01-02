Inventory = {}

-- private variables
local window = nil

local InventorySlotHead = 1
local InventorySlotNecklace = 2
local InventorySlotBackpack = 3
local InventorySlotArmor = 4
local InventorySlotRight = 5
local InventorySlotLeft = 6
local InventorySlotLegs = 7
local InventorySlotFeet = 8
local InventorySlotRing = 9
local InventorySlotAmmo = 10

-- public functions
function Inventory.create()
  window = UI.display('inventory.otui', { parent = Game.gameRightPanel })
end

function Inventory.destroy()
  window:destroy()
  window = nil
end

-- hooked events
function Inventory.onInventoryChange(slot, item)
  local slotId
  if slot == InventorySlotHead then
    slotId = 'head'
  elseif slot == InventorySlotNecklace then
    slotId = 'necklace'
  elseif slot == InventorySlotBackpack then
    slotId = 'backpack'
  elseif slot == InventorySlotArmor then
    slotId = 'armor'
  elseif slot == InventorySlotRight then
    slotId = 'right'
  elseif slot == InventorySlotLeft then
    slotId = 'left'
  elseif slot == InventorySlotLegs then
    slotId = 'legs'
  elseif slot == InventorySlotFeet then
    slotId = 'feet'
  elseif slot == InventorySlotRing then
    slotId = 'ring'
  elseif slot == InventorySlotAmmo then
    slotId = 'ammo'
  end

  local itemWidget = window:getChildById(slotId)
  itemWidget:setItem(item)
end

function Inventory.onFreeCapacityChange(freeCapacity)
  local widget = window:getChildById('capacity')
  widget:setText("Cap:\n" .. freeCapacity)
end

function Inventory.onSoulChange(soul)
  local widget = window:getChildById('soul')
  widget:setText("Soul:\n" .. soul)
end

function Inventory.onInventoryItemMousePress(itemWidget, mousePos, mouseButton)
  local menu = UIPopupMenu.create()
  menu:addOption('Look', function() print('look') end)
  menu:addOption('Use', function() print('use') end)
  menu:display(mousePos)
end

connect(Game, { onLogin = Inventory.create,
                onLogout = Inventory.destroy,
                onInventoryChange = Inventory.onInventoryChange,
                onFreeCapacityChange = Inventory.onFreeCapacityChange,
                onSoulChange = Inventory.onSoulChange })
