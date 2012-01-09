function UIMap:onMouseRelease(mousePosition, mouseButton)
  local tile = self:getTile(mousePosition)
  if tile and Game.processMouseAction(mousePosition, mouseButton, nil, tile:getTopLookThing(), tile:getTopUseThing(), tile:getTopCreature(), tile:getTopMultiUseThing()) then return true end
  return false
end
