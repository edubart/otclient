UIMiniWindow = extends(UIWindow)

function UIMiniWindow.create()
  local miniwindow = UIMiniWindow.internalCreate()
  return miniwindow
end

function UIMiniWindow:onDragEnter(mousePos)
  local parent = self:getParent()
  if not parent then return false end

  if parent:getClassName() == 'UIMiniWindowContainer' then
    local containerParent = parent:getParent()
    parent:removeChild(self)
    containerParent:addChild(self)
  end

  local oldPos = self:getPosition()
  self.movingReference = { x = mousePos.x - oldPos.x, y = mousePos.y - oldPos.y }
  self:setPosition(oldPos)
  return true
end

function UIMiniWindow:onDragLeave(droppedWidget, mousePos)
  -- TODO: drop on other interfaces
end

function UIMiniWindow:onFocusChange(focused)
  -- miniwindows only raises when its outside MiniWindowContainers
  if not focused then return end
  local parent = self:getParent()
  if parent and parent:getClassName() ~= 'UIMiniWindowContainer' then
    self:raise()
  end
end

