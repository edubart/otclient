UIMiniWindow = extends(UIWindow)

function UIMiniWindow.create()
  local miniwindow = UIMiniWindow.internalCreate()
  return miniwindow
end

function UIMiniWindow:onMousePress(mousePos, mouseButton)
  local parent = self:getParent()
  if parent:getClassName() ~= 'UIMiniWindowContainer' then
    self:raise()
  end
  return true
end

function UIMiniWindow:onDragEnter(mousePos)
  local parent = self:getParent()
  if parent:getClassName() == 'UIMiniWindowContainer' then
    local containerParent = parent:getParent()
    parent:removeChild(self)
    containerParent:addChild(self)
  end

  local oldPos = self:getPosition()
  self.movingReference = { x = mousePos.x - oldPos.x, y = mousePos.y - oldPos.y }
  self:setPosition(oldPos)
end

function UIMiniWindow:onDragLeave(droppedWidget, mousePos)
  -- TODO: drop on other interfaces
end
