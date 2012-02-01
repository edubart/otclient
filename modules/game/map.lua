function UIMap:onDragEnter(mousePos)
  local tile = self:getTile(mousePos)
  if not tile then return false end

  local thing = tile:getTopMoveThing()
  if not thing then return false end

  self.parsed = false
  self.currentDragThing = thing
  setTargetCursor()
  return true
end

function UIMap:onDragLeave(widget, mousePos)
  if not self.parsed then
    self.currentDragThing = nil
  end
  
  restoreCursor()
  return true
end

function UIMap:onDrop(widget, mousePos)
  if not widget or not widget.currentDragThing then return false end

  local tile = self:getTile(mousePos)
  if not tile then return false end
  
  local data = widget.currentDragThing:getCount()
  if widget.currentDragThing:isStackable() and data > 1 then
    widget.parsed = true
    local moveWindow = displayUI('/game/movewindow.otui')
    local spinbox = moveWindow:getChildById('spinbox')
    spinbox:setMaximum(data)
    spinbox:setMinimum(1)
    spinbox:setCurrentIndex(data)
    
    local okButton = moveWindow:getChildById('buttonOk')
    okButton.onClick = function() Game.move(widget.currentDragThing, tile:getPosition(), spinbox:getCurrentIndex()) okButton:getParent():destroy() widget.currentDragThing = nil end
    moveWindow.onEnter = okButton.onClick
  else
    Game.move(widget.currentDragThing, tile:getPosition(), 1)
  end

  return true
end

function UIMap:onMouseRelease(mousePosition, mouseButton)
  local tile = self:getTile(mousePosition)
  if tile and Game.processMouseAction(mousePosition, mouseButton, nil, tile:getTopLookThing(), tile:getTopUseThing(), tile:getTopCreature(), tile:getTopMultiUseThing()) then return true end
  return false
end

