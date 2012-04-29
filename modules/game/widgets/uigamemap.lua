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
  Mouse.setTargetCursor()
  return true
end

function UIGameMap:onDragLeave(droppedWidget, mousePos)
  self.currentDragThing = nil
  Mouse.restoreCursor()
  return true
end

function UIGameMap:onDrop(widget, mousePos)
  if not widget or not widget.currentDragThing then return false end

  local tile = self:getTile(mousePos)
  if not tile then return false end

  local thing = widget.currentDragThing
  local toPos = tile:getPosition()

  local itemPos = thing:getPosition()
  if itemPos.x == toPos.x and itemPos.y == toPos.y and itemPos.z == toPos.z then return false end

  if thing:asItem() and thing:getCount() > 1 then
    GameInterface.moveStackableItem(thing, toPos)
  else
    g_game.move(thing, toPos, 1)
  end

  return true
end

function UIGameMap:onMouseRelease(mousePosition, mouseButton)
  if self.cancelNextRelease then
    self.cancelNextRelease = false
    return true
  end

  local tile = self:getTile(mousePosition)
  if tile == nil then return false end

  if Options.getOption('classicControl') and
     ((Mouse.isPressed(MouseLeftButton) and mouseButton == MouseRightButton) or
      (Mouse.isPressed(MouseRightButton) and mouseButton == MouseLeftButton)) then
    local tile = self:getTile(mousePosition)
    g_game.look(tile:getTopLookThing())
    self.cancelNextRelease = true
    return true
  elseif GameInterface.processMouseAction(mousePosition, mouseButton, nil, tile:getTopLookThing(), tile:getTopUseThing(), tile:getTopCreature(), tile:getTopMultiUseThing()) then
    return true
  elseif mouseButton == MouseLeftButton and self:isPressed() then
    local dirs = g_map.findPath(g_game.getLocalPlayer():getPosition(), tile:getPosition(), 255)
    if #dirs == 0 then
      TextMessage.displayStatus(tr('There is no way.'))
      return true
    end
    g_game.autoWalk(dirs)
    return true
  end
  return false
end
