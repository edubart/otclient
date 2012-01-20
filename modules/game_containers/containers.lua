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
function Containers.onContainerOpen(containerId, itemId, name, capacity, hasParent)
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

  container.itemCount = 0
  container.capacity = capacity

  for i=1,capacity do
    local item = UIItem.create()
    item:setStyle('Item')
    container:addChild(item)
    item.position = {x=65535, y=containerId+64, z=i-1}
  end

  m_containers[containerId] = container
  print("opencid ".. containerId)
end

function Containers.onContainerClose(containerId)
  print("closecid ".. containerId)
  local container = m_containers[containerId]
  if container then
    Game.gameRightPanel:removeChild(container)
  end
  m_containers[containerId] = nil
end

function Containers.onContainerAddItem(containerId, item)
  print("addcid ".. containerId)
  local container = m_containers[containerId]
  if not container or not item or container.itemCount >= container.capacity then return end

  -- maybe this has to be moved to client internal's files
  local pos = item:getPos()
  pos.z = container.itemCount
  item:setPos(pos)

  local itemWidget = container:getChildByIndex(container.itemCount + 1)
  itemWidget:setItem(item)
  container.itemCount = container.itemCount + 1

end

connect(Game, { onLogin = Containers.clean,
                onLogout = Containers.clean,
                onContainerOpen = Containers.onContainerOpen,
                onContainerClose = Containers.onContainerClose,
                onContainerAddItem = Containers.onContainerAddItem })
