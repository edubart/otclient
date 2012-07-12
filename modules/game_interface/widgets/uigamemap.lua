UIGameMap = extends(UIMap)

function UIGameMap.create()
  local gameMap = UIGameMap.internalCreate()
  gameMap:setKeepAspectRatio(true)
  gameMap:setVisibleDimension({width = 15, height = 11})
  return gameMap
end


function UIGameMap:onDragEnter(mousePos)
  local tile = self:getTile(mousePos)
  if not tile then return false end

  local thing = tile:getTopMoveThing()
  if not thing then return false end

  self.currentDragThing = thing
  g_mouse.setTargetCursor()
  return true
end

function UIGameMap:onDragLeave(droppedWidget, mousePos)
  self.currentDragThing = nil
  g_mouse.restoreCursor()
  return true
end

function UIGameMap:onDrop(widget, mousePos)
  if not widget or not widget.currentDragThing then return false end

  local tile = self:getTile(mousePos)
  if not tile then return false end

  local thing = widget.currentDragThing
  local toPos = tile:getPosition()

  local thingPos = thing:getPosition()
  if thingPos.x == toPos.x and thingPos.y == toPos.y and thingPos.z == toPos.z then return false end

  if thing:asItem() and thing:getCount() > 1 then
    GameInterface.moveStackableItem(thing, toPos)
  else
    g_game.move(thing, toPos, 1)
  end

  return true
end

function UIGameMap:onMousePress()
  self.cancelNextRelease = false
end

function UIGameMap:onMouseRelease(mousePosition, mouseButton)
  if self.cancelNextRelease then
    self.cancelNextRelease = false
    return true
  end

  local tile = self:getTile(mousePosition)
  if tile == nil then return false end

  local localPlayerPos = g_game.getLocalPlayer():getPosition()
  local autoWalkPos = tile:getPosition()
  if autoWalkPos.z ~= localPlayerPos.z then
    local dz = autoWalkPos.z - localPlayerPos.z
    autoWalkPos.x = autoWalkPos.x + dz
    autoWalkPos.y = autoWalkPos.y + dz
    autoWalkPos.z = localPlayerPos.z
  end

  local lookThing = tile:getTopLookThing()
  local useThing = tile:getTopUseThing()
  local creatureThing = tile:getTopCreature()
  local multiUseThing = tile:getTopMultiUseThing()

  local ret = GameInterface.processMouseAction(mousePosition, mouseButton, autoWalkPos, lookThing, useThing, creatureThing, multiUseThing)
  if ret then
    self.cancelNextRelease = true
  end

  return ret
end
