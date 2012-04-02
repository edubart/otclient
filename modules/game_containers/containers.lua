Containers = {}
local function refreshContainerItems(container)
  for slot=0,container:getCapacity()-1 do
    local itemWidget = container.itemsPanel:getChildById('item' .. slot)
    itemWidget:setItem(container:getItem(slot))
  end
end

local function onContainerOpen(container, previousContainer)
  local containerWindow
  if previousContainer then
    containerWindow = previousContainer.window
    previousContainer.window = nil
    previousContainer.itemsPanel = nil
  else
    containerWindow = createWidget('ContainerWindow', GameInterface.getRightPanel())
  end
  containerWindow:setId('container' .. container:getId())
  local containerPanel = containerWindow:getChildById('contentsPanel')
  local containerItemWidget = containerWindow:getChildById('containerItemWidget')
  containerWindow.onClose = function()
    g_game.close(container)
    containerWindow:hide()
  end

  local upButton = containerWindow:getChildById('upButton')
  upButton.onClick = function()
    g_game.openParent(container)
  end
  upButton:setVisible(container:hasParent())

  local name = container:getName()
  name = name:sub(1,1):upper() .. name:sub(2)
  containerWindow:setText(name)

  containerItemWidget:setItemId(container:getItemId())

  containerPanel:destroyChildren()
  for slot=0,container:getCapacity()-1 do
    local itemWidget = createWidget('Item', containerPanel)
    itemWidget:setId('item' .. slot)
    itemWidget:setItem(container:getItem(slot))
    itemWidget.position = container:getSlotPosition(slot)
  end

  container.window = containerWindow
  container.itemsPanel = containerPanel
end

local function onContainerClose(container)
  local containerWindow = container.window
  if containerWindow then
    containerWindow:destroy()
  end
end

local function onContainerAddItem(container, slot, item)
  refreshContainerItems(container)
end

local function onContainerUpdateItem(container, slot, item, oldItem)
  local itemWidget = container.itemsPanel:getChildById('item' .. slot)
  itemWidget:setItem(item)
end

local function onContainerRemoveItem(container, slot, item)
  refreshContainerItems(container)
end

function Containers.init()
  importStyle 'container.otui'

  connect(Container, { onOpen = onContainerOpen,
                       onClose = onContainerClose,
                       onAddItem = onContainerAddItem,
                       onUpdateItem = onContainerUpdateItem,
                       onRemoveItem = onContainerRemoveItem })
end

function Containers.terminate()
  disconnect(Container, { onOpen = onContainerOpen,
                          onClose = onContainerClose,
                          onAddItem = onContainerAddItem,
                          onUpdateItem = onContainerUpdateItem,
                          onRemoveItem = onContainerRemoveItem })
end

function Containers.clean()

end
