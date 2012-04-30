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

  -- this disables scrollbar auto hiding
  local scrollbar = containerWindow:getChildById('miniwindowScrollBar')
  scrollbar:mergeStyle({ ['$disabled'] = { ['width'] = scrollbar:getWidth() } })

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
    itemWidget:setMargin(3)
    itemWidget.position = container:getSlotPosition(slot)
  end

  container.window = containerWindow
  container.itemsPanel = containerPanel
end

local function onContainerClose(container)
  if container.window then container.window:destroy() end
end

local function onContainerAddItem(container, slot, item)
  if not container.window then return end
  refreshContainerItems(container)
end

local function onContainerUpdateItem(container, slot, item, oldItem)
  if not container.window then return end
  local itemWidget = container.itemsPanel:getChildById('item' .. slot)
  itemWidget:setItem(item)
end

local function onContainerRemoveItem(container, slot, item)
  if not container.window then return end
  refreshContainerItems(container)
end

function Containers.init()
  importStyle 'container.otui'

  connect(Container, { onOpen = onContainerOpen,
                       onClose = onContainerClose,
                       onAddItem = onContainerAddItem,
                       onUpdateItem = onContainerUpdateItem,
                       onRemoveItem = onContainerRemoveItem })
  connect(Game, { onGameEnd = Containers.clean() })

  Containers.reloadContainers()
end

function Containers.terminate()
  disconnect(Container, { onOpen = onContainerOpen,
                          onClose = onContainerClose,
                          onAddItem = onContainerAddItem,
                          onUpdateItem = onContainerUpdateItem,
                          onRemoveItem = onContainerRemoveItem })
  disconnect(Game, { onGameEnd = Containers.clean() })
  Containers = nil
end

function Containers.reloadContainers()
  Containers.clean()
  for containerid,container in pairs(g_game.getContainers()) do
    onContainerOpen(container)
  end
end

function Containers.clean()
  for containerid,container in pairs(g_game.getContainers()) do
    if container.window then
      container.window:destroy()
      container.window = nil
      container.itemsPnale = nil
    end
  end
end
