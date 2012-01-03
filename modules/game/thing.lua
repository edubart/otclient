
-- public functions
function Thing:createMenu(menuPosition)
  local menu = createWidget('PopupMenu')
  menu:addOption('Look', function() Game.look(self) end)

  -- Open or Use, depending if thing is a container
  if self:isContainer() then
    menu:addOption('Open', function() print('open') end)
  else
    menu:addOption('Use', function() print('use') end)
  end
  
  if self:asLocalPlayer() then
    menu:addOption('Set Outfit', function() Game.openOutfitWindow() end)
  end
  
  menu:display(menuPosition)
end
