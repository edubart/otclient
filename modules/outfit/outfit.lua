Outfit = {}

-- private variables
local window = nil
local outfits = nil
local currentOutfit = 1

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
  window:lock()
  
  local creatureWidget = window:getChildById('creature')
  creatureWidget:setCreature(creature)
  
  outfits = outfitList
  currentOutfit = 1
end

function Outfit.destroy()
  if window ~= nil then
    window:destroy()
    window = nil
  end
end

function Outfit.nextType()
  local creatureWidget = window:getChildById('creature')
  currentOutfit = currentOutfit + 1
  if currentOutfit > #outfits then
    currentOutfit = 1
  end
  
  creatureWidget:setOutfitType(outfits[currentOutfit][1])
  
  -- TODO: update addons
end

function Outfit.previousType()
  local creatureWidget = window:getChildById('creature')
  currentOutfit = currentOutfit - 1
  if currentOutfit <= 0 then
    currentOutfit = #outfits
  end
  
  creatureWidget:setOutfitType(outfits[currentOutfit][1])
  
  -- TODO: update addons
end

-- private functions

-- hooked events

connect(Game, { onOpenOutfitWindow = Outfit.create,
                onLogout = Outfit.destroy })

connect(Game, { onLogin = Outfit.test })
