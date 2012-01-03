
-- public functions
function Thing:createMenu(menuPosition)
  local menu = createWidget('PopupMenu')
  menu:addOption('Look', function() Game.look(self) end)

  -- Open or Use, depending if thing is a container
  if not self:asCreature() then
    if self:isContainer() then
      menu:addOption('Open', function() print('open') end)
    else
      if self:isMultiUse() then
        menu:addOption('Use with ...', function() print('use with...') end)
      else
        menu:addOption('Use', function() Game.use(self) end)
      end
    end
    
    if self:isRotateable() then
      menu:addOption('Rotate', function() print('rotate') end)
    end
    
    menu:addSeparator()
    
    if not self:isNotMoveable() and self:isPickupable() then
      menu:addOption('Trade with ...', function() print('trade with') end)
    end
     
  else
  
    menu:addSeparator()
  
    if self:asLocalPlayer() then
      menu:addOption('Set Outfit', function() Game.openOutfitWindow() end)
    else
      -- todo: check for stop attack/follow
      menu:addOption('Attack', function() print('attack') end)
      menu:addOption('Follow', function() print('follow') end)
      
      if self:asPlayer() then
        menu:addSeparator()
        menu:addOption('Message to ' .. self:asCreature():getName(), function() print('message') end)
        menu:addOption('Add to VIP list', function() print('vip') end)
        menu:addOption('Ignore ' .. self:asCreature():getName(), function() print('ignore') end)
        menu:addOption('Invite to Party', function() print('invite to party') end)
      end
      
    end

    menu:addSeparator()
    menu:addOption('Copy Name', function() print('copy name') end)
    
  end
  
  menu:display(menuPosition)
end
