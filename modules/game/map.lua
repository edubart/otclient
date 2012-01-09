function UIMap:onMousePress(mousePos, mouseButton)

  local tile = self:getTile(mousePos)
  if not tile then return false end
  
  if not Options.classicControl then
  
    if mouseButton == MouseRightButton then
      Game.createThingMenu(mousePos, tile:getTopLookThing(), tile:getTopUseThing(), tile:getTopCreature())
      return true
    end
    
  end
  
  return false
end
