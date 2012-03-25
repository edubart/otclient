UIGameMap = extends(UIMap)

function UIGameMap.create()
  local gameMap = UIGameMap.internalCreate()
  return gameMap
end


function UIGameMap:onDragEnter(mousePos)
  local tile = self:getTile(mousePos)
  if not tile then return false end

  local thing = tile:getTopMoveThing()
  if not thing then return false end

  self.parsed = false
  self.currentDragThing = thing
  Mouse.setTargetCursor()
  return true
end

function UIGameMap:onDragLeave(droppedWidget, mousePos)
  if not self.parsed then
    self.currentDragThing = nil
  end

  Mouse.restoreCursor()
  return true
end

function UIGameMap:onDrop(widget, mousePos)
  if not widget or not widget.currentDragThing then return false end

  local tile = self:getTile(mousePos)
  if not tile then return false end

  local count = widget.currentDragThing:getCount()
  if widget.currentDragThing:isStackable() and count > 1 then
    widget.parsed = true
    local moveWindow = createWidget('CountWindow', rootWidget)
    local spinbox = moveWindow:getChildById('spinbox')
    spinbox:setMaximum(count)
    spinbox:setMinimum(1)
    spinbox:setCurrentIndex(count)

    local okButton = moveWindow:getChildById('buttonOk')
    okButton.onClick = function()
      g_game.move(widget.currentDragThing, tile:getPosition(), spinbox:getCurrentIndex())
      okButton:getParent():destroy()
      widget.currentDragThing = nil
    end
    moveWindow.onEnter = okButton.onClick
  else
    g_game.move(widget.currentDragThing, tile:getPosition(), 1)
  end

  return true
end

function UIGameMap:onClick(mousePosition)
  local tile = self:getTile(mousePosition)
  if tile == nil then return false end
  local dirs = g_map.findPath(g_game.getLocalPlayer():getPosition(), tile:getPosition(), 255)
  if #dirs == 0 then
    TextMessage.displayStatus('There is no way.')
    return true
  end
  g_game.autoWalk(dirs)
  return true
end

function UIGameMap:onMouseRelease(mousePosition, mouseButton)
  local tile = self:getTile(mousePosition)
  if tile == nil then return false end
  if GameInterface.processMouseAction(mousePosition, mouseButton, nil, tile:getTopLookThing(), tile:getTopUseThing(), tile:getTopCreature(), tile:getTopMultiUseThing()) then
    return true
  end
  return false
end
