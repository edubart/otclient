function init()
  g_ui.importStyle('container')

  connect(Container, { onOpen = onContainerOpen,
                       onClose = onContainerClose,
                       onAddItem = onContainerAddItem,
                       onUpdateItem = onContainerUpdateItem,
                       onRemoveItem = onContainerRemoveItem })
  connect(Game, { onGameEnd = clean() })

  reloadContainers()
end

function terminate()
  disconnect(Container, { onOpen = onContainerOpen,
                          onClose = onContainerClose,
                          onAddItem = onContainerAddItem,
                          onUpdateItem = onContainerUpdateItem,
                          onRemoveItem = onContainerRemoveItem })
  disconnect(Game, { onGameEnd = clean() })
end

function reloadContainers()
  clean()
  for _,container in pairs(g_game.getContainers()) do
    onContainerOpen(container)
  end
end

function clean()
  for containerid,container in pairs(g_game.getContainers()) do
    destroy(container)
  end
end

function destroy(container)
  if container.window then
    container.window:destroy()
    container.window = nil
    container.itemsPanel = nil
  end
end

function refreshContainerItems(container)
  for slot=0,container:getCapacity()-1 do
    local itemWidget = container.itemsPanel:getChildById('item' .. slot)
    itemWidget:setItem(container:getItem(slot))
  end
end

function onContainerOpen(container, previousContainer)
  local containerWindow
  if previousContainer then
    containerWindow = previousContainer.window
    previousContainer.window = nil
    previousContainer.itemsPanel = nil
  else
    containerWindow = g_ui.createWidget('ContainerWindow', modules.game_interface.getRightPanel())
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
  scrollbar:mergeStyle({ ['$!on'] = { }})

  local upButton = containerWindow:getChildById('upButton')
  upButton.onClick = function()
    g_game.openParent(container)
  end
  upButton:setVisible(container:hasParent())

  local name = container:getName()
  name = name:sub(1,1):upper() .. name:sub(2)
  containerWindow:setText(name)

  containerItemWidget:setItem(container:getContainerItem())

  containerPanel:destroyChildren()
  for slot=0,container:getCapacity()-1 do
    local itemWidget = g_ui.createWidget('Item', containerPanel)
    itemWidget:setId('item' .. slot)
    itemWidget:setItem(container:getItem(slot))
    itemWidget:setMargin(0)
    itemWidget.position = container:getSlotPosition(slot)
  end

  container.window = containerWindow
  container.itemsPanel = containerPanel

  local layout = containerPanel:getLayout()
  local cellSize = layout:getCellSize()
  containerWindow:setContentMinimumHeight(cellSize.height)
  containerWindow:setContentMaximumHeight(cellSize.height*layout:getNumLines())

  if not previousContainer then
    local filledLines = math.max(math.ceil(container:getItemsCount() / layout:getNumColumns()), 1)
    containerWindow:setContentHeight(filledLines*cellSize.height)
  end

  containerWindow:setup()
end

function onContainerClose(container)
  destroy(container)
end

function onContainerAddItem(container, slot, item)
  if not container.window then return end
  refreshContainerItems(container)
end

function onContainerUpdateItem(container, slot, item, oldItem)
  if not container.window then return end
  local itemWidget = container.itemsPanel:getChildById('item' .. slot)
  itemWidget:setItem(item)
end

function onContainerRemoveItem(container, slot, item)
  if not container.window then return end
  refreshContainerItems(container)
end
