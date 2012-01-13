function UIItem:onMouseRelease(mousePosition, mouseButton)
  local item = self:getItem()
  if not item or not self:containsPoint(mousePosition) then return false end
  return Game.processMouseAction(mousePosition, mouseButton, nil, item, item, nil, item)
end

