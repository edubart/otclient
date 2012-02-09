Containers = {}

-- private variables
local m_containers = {}

-- public functions
function Containers.clean()
  m_containers = {}
end

function Containers.getFreeContainerId()
  for i=0,15 do
    if not m_containers[i] then
      return i
    end
  end
  return 0
end

-- hooked events
function Containers.onOpenContainer(containerId, itemId, name, capacity, hasParent, items)
  local container = m_containers[containerId]
  if container then
    g_game.gameRightPanel:removeChild(container)
  end

  container = displayUI('container.otui', { parent = g_game.gameRightPanel })
  name = name:sub(1,1):upper() .. name:sub(2)
  container:setText(name)

  -- set icon, itemid
  -- closebutton
  -- resize
  if hasParent then
    -- parent button
  end

  container.itemCount = #items
  container.capacity = capacity

  for i=1,capacity do
    local itemWidget = UIItem.create()
    itemWidget:setStyle('Item')
    container:addChild(itemWidget)
    itemWidget.position = {x=65535, y=containerId+64, z=i-1}

    if i <= #items then
      local item = items[i]
      item:setPosition(itemWidget.position)
      itemWidget:setItem(item)
    end
  end

  m_containers[containerId] = container
end

function Containers.onCloseContainer(containerId)
  local container = m_containers[containerId]
  if container then
    g_game.gameRightPanel:removeChild(container)
  end
  m_containers[containerId] = nil
end

function Containers.onContainerAddItem(containerId, item)
  local container = m_containers[containerId]
  if not container or not item or container.itemCount >= container.capacity then return end

  local i = container.itemCount
  while i >= 1 do
    local itemWidget = container:getChildByIndex(i)
    if not itemWidget then return end

    local nextItemWidget = container:getChildByIndex(i+1)
    if not nextItemWidget then return end

    local swapItem = itemWidget:getItem()
    if swapItem then
      swapItem:setPosition(nextItemWidget.position)
      nextItemWidget:setItem(swapItem)
    end

    i = i - 1
  end

  local itemWidget = container:getChildByIndex(1)
  if not itemWidget then return end
  item:setPosition(itemWidget.position)
  itemWidget:setItem(item)

  container.itemCount = container.itemCount + 1
end

function Containers.onContainerUpdateItem(containerId, slot, item)
  local container = m_containers[containerId]
  if not container then return end

  local itemWidget = container:getChildByIndex(slot + 1)
  if not itemWidget then return end
  itemWidget:setItem(item)
  item:setPosition(itemWidget.position)
end

function Containers.onContainerRemoveItem(containerId, slot)
  local container = m_containers[containerId]
  if not container then return end

  local itemWidget = container:getChildByIndex(slot+1)
  if not itemWidget then return end
  itemWidget:setItem(nil)

  for i=slot,container.itemCount-2 do
    local itemWidget = container:getChildByIndex(i+1)
    if not itemWidget then return end

    local nextItemWidget = container:getChildByIndex(i+2)
    if not nextItemWidget then return end

    local item = nextItemWidget:getItem()
    local pos = item:getPosition()
    pos.z = pos.z - 1
    item:setPosition(pos)

    itemWidget:setItem(item)
    nextItemWidget:setItem(nil)
  end

  container.itemCount = container.itemCount - 1
end

connect(g_game, { onGameStart = Containers.clean,
                onGameEnd = Containers.clean,
                onOpenContainer = Containers.onOpenContainer,
                onCloseContainer = Containers.onCloseContainer,
                onContainerAddItem = Containers.onContainerAddItem,
                onContainerUpdateItem = Containers.onContainerUpdateItem,
                onContainerRemoveItem = Containers.onContainerRemoveItem })

