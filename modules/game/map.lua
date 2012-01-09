function UIMap:onMouseRelease(mousePos, mouseButton)

  local tile = self:getTile(mousePos)
  if not tile then return false end
  
  local keyboardModifiers = g_window.getKeyboardModifiers()
  print ("lol " .. keyboardModifiers)
  if not Options.classicControl then
  
    if keyboardModifiers == KeyboardNoModifier and mouseButton == MouseLeftButton then
      -- auto walk
      return true
    elseif keyboardModifiers == KeyboardNoModifier and mouseButton == MouseRightButton then
      Game.createThingMenu(mousePos, tile:getTopLookThing(), tile:getTopUseThing(), tile:getTopCreature())
      return true
    elseif keyboardModifiers == KeyboardShiftModifier and (mouseButton == MouseRightButton or mouseButton == MouseLeftButton) then
      Game.look(tile:getTopLookThing())
      return true
    elseif keyboardModifiers == KeyboardCtrlModifier and (mouseButton == MouseRightButton or mouseButton == MouseLeftButton) then
      Game.use(tile:getTopUseThing())
      return true
    elseif keyboardModifiers == KeyboardAltModifier and (mouseButton == MouseRightButton or mouseButton == MouseLeftButton) then
      Game.attack(tile:getTopCreature())
      return true
    end
    
  end
  
  return false
end
