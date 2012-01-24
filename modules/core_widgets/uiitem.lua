function UIItem:onDragEnter(mousePos)
  local item = self:getItem()
  if not item then return false end

  self:setBorderWidth(1)

  self.currentDragThing = item
  setTargetCursor()
  return true
end

function UIItem:onDragLeave(widget, mousePos)
  self.currentDragThing = nil
  restoreCursor()
  self:setBorderWidth(0)
  return true
end

function UIItem:onDrop(widget, mousePos)
  if not widget or not widget.currentDragThing then return false end

  local pos = self.position
  local count = widget.currentDragThing:getData()

  Game.move(widget.currentDragThing, pos, count)
  self:setBorderWidth(0)
  return true
end

function UIItem:onHoverChange(hovered)
  if g_ui.getDraggingWidget() and self ~= g_ui.getDraggingWidget() then
    if hovered then
      self:setBorderWidth(1)
    else
      self:setBorderWidth(0)
    end
  end
end

function UIItem:onMouseRelease(mousePosition, mouseButton)
  local item = self:getItem()
  if not item or not self:containsPoint(mousePosition) then return false end
  return Game.processMouseAction(mousePosition, mouseButton, nil, item, item, nil, item)
end

