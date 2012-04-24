function UIItem:onDragEnter(mousePos)
  if self:isVirtual() then return false end

  local item = self:getItem()
  if not item then return false end

  self:setBorderWidth(1)
  self.currentDragThing = item
  Mouse.setTargetCursor()
  return true
end

function UIItem:onDragLeave(droppedWidget, mousePos)
  if self:isVirtual() then return false end
  self.currentDragThing = nil
  Mouse.restoreCursor()
  self:setBorderWidth(0)
  return true
end

function UIItem:onDrop(widget, mousePos)
  if self:isVirtual() then return false end

  if not widget or not widget.currentDragThing then return false end

  local item = widget.currentDragThing
  local toPos = self.position

  local itemPos = item:getPosition()
  if itemPos.x == self.position.x and itemPos.y == self.position.y and itemPos.z == self.position.z then return false end

  if item:getCount() > 1 then
    GameInterface.moveStackableItem(item, toPos)
  else
    g_game.move(item, toPos, 1)
  end

  self:setBorderWidth(0)
  return true
end

function UIItem:onHoverChange(hovered)
  if self:isVirtual() then return end

  local draggingWidget = g_ui.getDraggingWidget()
  if draggingWidget and self ~= draggingWidget then
    local gotMap = draggingWidget:getClassName() == 'UIMap'
    local gotItem = draggingWidget:getClassName() == 'UIItem' and not draggingWidget:isVirtual()
    if hovered and (gotItem or gotMap) then
      self:setBorderWidth(1)
    else
      self:setBorderWidth(0)
    end
  end
end

function UIItem:onMouseRelease(mousePosition, mouseButton)
  if self.cancelNextRelease then
    self.cancelNextRelease = false
    return true
  end

  if self:isVirtual() then return false end

  local item = self:getItem()
  if not item or not self:containsPoint(mousePosition) then return false end

  if Options.getOption('classicControl') and
     ((Mouse.isPressed(MouseLeftButton) and mouseButton == MouseRightButton) or
      (Mouse.isPressed(MouseRightButton) and mouseButton == MouseLeftButton)) then
    g_game.look(item)
    self.cancelNextRelease = true
    return true
  elseif GameInterface.processMouseAction(mousePosition, mouseButton, nil, item, item, nil, item) then
    return true
  end
  return false
end

