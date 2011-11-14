Outfit = {}

-- private variables
local window = nil

-- public functions
function Outfit.test()
  local button = UIButton.create()
  UI.root:addChild(button)
  button:setText('Set Outfit')
  button:setStyle('Button')
  button:moveTo({x = 0, y = 100})
  button:setWidth('100')
  button:setHeight('30')
  button.onClick = function() Game.openOutfitWindow() end
end

function Outfit.create(creature, outfitList)
  Outfit.destroy()
  window = loadUI("/outfit/outfit.otui", UI.root)
  
  local creatureWidget = window:getChildById('creature')
  creatureWidget:setCreature(creature)
end

function Outfit.destroy()
  if window ~= nil then
    window:destroy()
    window = nil
  end
end

-- private functions

-- hooked events

connect(Game, { onOpenOutfitWindow = Outfit.create,
                onLogout = Outfit.destroy })

connect(Game, { onLogin = Outfit.test })
