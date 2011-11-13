-- place any code for testing purposes here

function UIItem.onMouseRelease(self, mousePos, mouseButton)
  if mouseButton ~= MouseRightButton then return end
  local top = self:getY()
  local bottom = self:getY() + self:getHeight()
  local left = self:getX()
  local right = self:getX() + self:getWidth()
  if not (mousePos.y >= top and mousePos.y <= bottom and mousePos.x >= left and mousePos.x <= right) then return end

  local menuFile = self:getStyle()['popup menu']
  if not menuFile then return end

  local popupMenu = UI.loadAndDisplay(menuFile)
  if not popupMenu then return end

  popupMenu:moveTo(mousePos)
  popupMenu.onMouseRelease = function(self) self:destroy() end
end

local function init()
end

addEvent(init)