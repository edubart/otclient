Inventory = {}

-- private variables
local window = nil

-- public functions
function Inventory.create()
  window = displayUI('inventory.otui', { parent = Game.gameRightPanel })
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

connect(Game, { onLogin = Inventory.create,
                onLogout = Inventory.destroy,
                onInventoryChange = Inventory.onInventoryChange,
                onFreeCapacityChange = Inventory.onFreeCapacityChange,
                onSoulChange = Inventory.onSoulChange })

