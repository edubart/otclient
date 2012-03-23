Inventory = {}

-- private variables
local inventoryWindow
local inventoryButton

-- public functions
function Inventory.init()
  connect(g_game, { onGameEnd = Inventory.clear,
                    onInventoryChange = Inventory.onInventoryChange,
                    onFreeCapacityChange = Inventory.onFreeCapacityChange,
                    onSoulChange = Inventory.onSoulChange })

  Keyboard.bindKeyDown('Ctrl+I', Inventory.toggle)

  inventoryWindow = displayUI('inventory.otui', GameInterface.getRightPanel())
  inventoryButton = TopMenu.addGameToggleButton('inventoryButton', 'Inventory (Ctrl+I)', 'inventory.png', Inventory.toggle)
  inventoryButton:setOn(true)

  if g_game.isOnline() then
    Inventory.reload()
  end
end

function Inventory.terminate()
  connect(g_game, { onGameEnd = Inventory.clear,
                    onInventoryChange = Inventory.onInventoryChange,
                    onFreeCapacityChange = Inventory.onFreeCapacityChange,
                    onSoulChange = Inventory.onSoulChange })

  Keyboard.unbindKeyDown('Ctrl+I')

  inventoryWindow:destroy()
  inventoryWindow = nil
  inventoryButton:destroy()
  inventoryButton = nil
end

function Inventory.toggle()
  local visible = not inventoryWindow:isExplicitlyVisible()
  inventoryWindow:setVisible(visible)
  inventoryButton:setOn(visible)
end

function Inventory.clear()
end

function Inventory.reload()
end

-- hooked events
function Inventory.onInventoryChange(slot, item)
  local itemWidget = inventoryWindow:getChildById('slot' .. slot)
  itemWidget:setItem(item)
end

function Inventory.onFreeCapacityChange(freeCapacity)
  local widget = inventoryWindow:getChildById('capacity')
  widget:setText("Cap:\n" .. freeCapacity)
end

function Inventory.onSoulChange(soul)
  local widget = inventoryWindow:getChildById('soul')
  widget:setText("Soul:\n" .. soul)
end
