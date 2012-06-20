UIMiniWindow = extends(UIWindow)

function UIMiniWindow.create()
  local miniwindow = UIMiniWindow.internalCreate()
  return miniwindow
end

function UIMiniWindow:onSetup()
  self:getChildById('closeButton').onClick = function() signalcall(self.onClose, self) end
  self:getChildById('minimizeButton').onClick = function() signalcall(self.onMinimize, self) end
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

function UIMiniWindow:onDragMove(mousePos, mouseMoved)
  local oldMousePosY = mousePos.y - mouseMoved.y
  local children = rootWidget:recursiveGetChildrenByMarginPos(mousePos)
  local overAnyWidget = false
  for i=1,#children do
    local child = children[i]
    if child:getParent():getClassName() == 'UIMiniWindowContainer' then
      overAnyWidget = true

      local childCenterY = child:getY() + child:getHeight() / 2
      if child == self.movedWidget and mousePos.y < childCenterY and oldMousePosY < childCenterY then
        break
      end

      if self.movedWidget then
        self.setMovedChildMargin(0)
        self.setMovedChildMargin = nil
      end

      if mousePos.y < childCenterY then
        self.setMovedChildMargin = function(v) child:setMarginTop(v) end
        self.movedIndex = 0
      else
        self.setMovedChildMargin = function(v) child:setMarginBottom(v) end
        self.movedIndex = 1
      end

      self.movedWidget = child
      self.setMovedChildMargin(self:getHeight())
      break
    end
  end

  if not overAnyWidget and self.movedWidget then
    self.setMovedChildMargin(0)
    self.movedWidget = nil
  end

  return UIWindow.onDragMove(self, mousePos, mouseMoved)
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
  if self.movedWidget then
    self.setMovedChildMargin(0)
    self.movedWidget = nil
    self.setMovedChildMargin = nil
    self.movedIndex = nil
  end
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

function UIMiniWindow:getClassName()
  return 'UIMiniWindow'
end
