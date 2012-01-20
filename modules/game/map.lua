function UIMap:onDragEnter(mousePos)
  local tile = self:getTile(mousePos)
  if not tile then return false end
  
  local thing = tile:getTopMoveThing()
  if not thing then return false end

  self.currentDragThing = thing
  setTargetCursor()
  return true
end

function UIMap:onDragLeave(widget, mousePos)
  self.currentDragThing = nil
  restoreCursor()
  return true
end

function UIMap:onDrop(widget, mousePos)
  if not widget or not widget.currentDragThing then return false end
  
  local pos = self:getPosition(mousePos)
  local count = 1 -- todo make a window for it
  
  Game.move(widget.currentDragThing, pos, count)
  return true
end

function UIMap:onMouseRelease(mousePosition, mouseButton)
  local tile = self:getTile(mousePosition)
  if tile and Game.processMouseAction(mousePosition, mouseButton, nil, tile:getTopLookThing(), tile:getTopUseThing(), tile:getTopCreature(), tile:getTopMultiUseThing()) then return true end
  return false
end

