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
function Containers.onContainerOpen(containerId, itemId, name, capacity, hasParent, items)
  local container = m_containers[containerId]
  if container then
    Game.gameRightPanel:removeChild(container)
  end

  container = displayUI('container.otui', { parent = Game.gameRightPanel })
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
      item:setPos(itemWidget.position)
      itemWidget:setItem(item)
    end
  end

  m_containers[containerId] = container
end

function Containers.onContainerClose(containerId)
  local container = m_containers[containerId]
  if container then
    Game.gameRightPanel:removeChild(container)
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
      swapItem:setPos(nextItemWidget.position)
      nextItemWidget:setItem(swapItem)
    end
    
    i = i - 1
  end

  local itemWidget = container:getChildByIndex(1)
  if not itemWidget then return end
  item:setPos(itemWidget.position)
  itemWidget:setItem(item)

  container.itemCount = container.itemCount + 1
end

function Containers.onContainerUpdateItem(containerId, slot, item)
  local container = m_containers[containerId]
  if not container then return end
  
  local itemWidget = container:getChildByIndex(slot + 1)
  if not itemWidget then return end
  itemWidget:setItem(item)
  item:setPos(itemWidget.position)
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
    local pos = item:getPos()
    pos.z = pos.z - 1
    item:setPos(pos)
    
    itemWidget:setItem(item)
    nextItemWidget:setItem(nil)
  end
  
  container.itemCount = container.itemCount - 1
end

connect(Game, { onLogin = Containers.clean,
                onLogout = Containers.clean,
                onContainerOpen = Containers.onContainerOpen,
                onContainerClose = Containers.onContainerClose,
                onContainerAddItem = Containers.onContainerAddItem,
                onContainerUpdateItem = Containers.onContainerUpdateItem,
                onContainerRemoveItem = Containers.onContainerRemoveItem })
                
