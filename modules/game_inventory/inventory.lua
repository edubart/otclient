Inventory = {}

-- private variables
local window = nil

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
  local itemWidget = window:getChildById('slot' .. slot)
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
  if mouseButton ~= MouseRightButton then return end
  
  local item = itemWidget:getItem()
  if not item then return end
  
  local menu = UIPopupMenu.create()
  
  -- Look
  local itemId = item:getId()
  local slotId = tonumber(itemWidget:getId():sub(5))
  menu:addOption('Look', function() Game.lookAtInventory(itemId, slotId) end)
  
  -- Open or Use, depending if thing is a container
  if item:isContainer() then
    menu:addOption('Open', function() print('open') end)
  else
    menu:addOption('Use', function() print('use') end)
  end
  
  menu:display(mousePos)
end

connect(Game, { onLogin = Inventory.create,
                onLogout = Inventory.destroy,
                onInventoryChange = Inventory.onInventoryChange,
                onFreeCapacityChange = Inventory.onFreeCapacityChange,
                onSoulChange = Inventory.onSoulChange })
