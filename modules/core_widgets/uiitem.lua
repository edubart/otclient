function UIItem:onDragEnter(mousePos)
  local item = self:getItem()
  if not item then return false end

  self.currentDragThing = item
  setTargetCursor()
  return true
end

function UIItem:onDragLeave(widget, mousePos)
  self.currentDragThing = nil
  restoreCursor()
  return true
end

function UIItem:onDrop(widget, mousePos)
  if not widget or not widget.currentDragThing then return false end

  local pos = self.position
  local count = 1 -- todo make a window for it

  Game.move(widget.currentDragThing, pos, count)
  return true
end


function UIItem:onMouseRelease(mousePosition, mouseButton)
  local item = self:getItem()
  if not item or not self:containsPoint(mousePosition) then return false end
  return Game.processMouseAction(mousePosition, mouseButton, nil, item, item, nil, item)
end

