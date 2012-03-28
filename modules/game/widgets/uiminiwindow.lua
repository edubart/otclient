UIMiniWindow = extends(UIWindow)

function UIMiniWindow.create()
  local miniwindow = UIMiniWindow.internalCreate()
  return miniwindow
end

function UIMiniWindow:onSetup()
  addEvent(function()
    self:getChildById('closeButton').onClick = function() signalcall(self.onClose, self) end
    self:getChildById('minimizeButton').onClick = function() signalcall(self.onMinimize, self) end
  end)
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
  self.free = true
  return true
end

function UIMiniWindow:onMousePress()
  local parent = self:getParent()
  if not parent then return false end
  if parent:getClassName() ~= 'UIMiniWindowContainer' then
    self:raise()
    return true
  end
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

function UIMiniWindow:onClose()
end

function UIMiniWindow:onMinimize()
  if self:isOn() then
    self:setOn(false)
    self:getChildById('contentsPanel'):show()
    self:getChildById('miniwindowScrollBar'):show()
    self:getChildById('bottomResizeBorder'):show()
    self:getChildById('minimizeButton'):setOn(false)
    self:setHeight(self.savedHeight)
  else
    self.savedHeight = self:getHeight()
    self:setHeight(self.minimizedHeight)
    self:setOn(true)
    self:getChildById('contentsPanel'):hide()
    self:getChildById('miniwindowScrollBar'):hide()
    self:getChildById('bottomResizeBorder'):hide()
    self:getChildById('minimizeButton'):setOn(true)
  end
end

