Inventory = {}

-- private variables
local inventoryPanel
local inventoryButton

-- public functions
function Inventory.init()
  connect(g_game, { onGameEnd = Inventory.clear,
                    onInventoryChange = Inventory.onInventoryChange,
                    onFreeCapacityChange = Inventory.onFreeCapacityChange,
                    onSoulChange = Inventory.onSoulChange })

  Keyboard.bindKeyDown('Ctrl+I', Inventory.toggle)

  inventoryPanel = displayUI('inventory.otui', GameInterface.getRightPanel()):getChildById('inventoryPanel')
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

  inventoryPanel:destroy()
  inventoryPanel = nil
  inventoryButton:destroy()
  inventoryButton = nil
end

function Inventory.toggle()
  local visible = not inventoryPanel:isExplicitlyVisible()
  inventoryPanel:setVisible(visible)
  inventoryButton:setOn(visible)
end

function Inventory.clear()
end

function Inventory.reload()
end

-- hooked events
function Inventory.onInventoryChange(slot, item)
  local itemWidget = inventoryPanel:getChildById('slot' .. slot)
  itemWidget:setItem(item)
end

function Inventory.onFreeCapacityChange(freeCapacity)
  local widget = inventoryPanel:getChildById('capacity')
  widget:setText("Cap:\n" .. freeCapacity)
end

function Inventory.onSoulChange(soul)
  local widget = inventoryPanel:getChildById('soul')
  widget:setText("Soul:\n" .. soul)
end
