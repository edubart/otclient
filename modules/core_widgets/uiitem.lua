function UIItem:onDragEnter(mousePos)
  if self:isVirtual() then return false end

  local item = self:getItem()
  if not item then return true end

  self:setBorderWidth(1)

  self.parsed = false
  self.currentDragThing = item
  setTargetCursor()
  return true
end

function UIItem:onDragLeave(widget, mousePos)
  if self:isVirtual() then return false end

  if not self.parsed then
    self.currentDragThing = nil
  end

  restoreCursor()
  self:setBorderWidth(0)
  return true
end

function UIItem:onDrop(widget, mousePos)
  if self:isVirtual() then return false end

  if not widget or not widget.currentDragThing then return true end

  local pos = self.position
  local count = widget.currentDragThing:getCount()
  if widget.currentDragThing:isStackable() and count > 1 then
    widget.parsed = true
    local moveWindow = displayUI('/game/movewindow.otui')
    local spinbox = moveWindow:getChildById('spinbox')
    spinbox:setMaximum(count)
    spinbox:setMinimum(1)
    spinbox:setCurrentIndex(count)

    local okButton = moveWindow:getChildById('buttonOk')
    okButton.onClick = function() Game.move(widget.currentDragThing, pos, spinbox:getCurrentIndex()) okButton:getParent():destroy() widget.currentDragThing = nil end
    moveWindow.onEnter = okButton.onClick
  else
    Game.move(widget.currentDragThing, pos, 1)
  end

  self:setBorderWidth(0)
  return true
end

function UIItem:onHoverChange(hovered)
  if self:isVirtual() then return end

  if g_ui.getDraggingWidget() and self ~= g_ui.getDraggingWidget() then
    if hovered then
      self:setBorderWidth(1)
    else
      self:setBorderWidth(0)
    end
  end
end

function UIItem:onMouseRelease(mousePosition, mouseButton)
  if self:isVirtual() then return false end

  local item = self:getItem()
  if not item or not self:containsPoint(mousePosition) then return false end
  return Game.processMouseAction(mousePosition, mouseButton, nil, item, item, nil, item)
end

