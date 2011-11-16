Outfit = {}

-- private variables
local window = nil
local outfits = nil
local currentOutfit = 1

-- private functions
local function update()
  local creatureWidget = window:getChildById('creature')
  creatureWidget:setOutfitType(outfits[currentOutfit][1])

  local nameWidget = window:getChildById('name')
  nameWidget:setText(outfits[currentOutfit][2])

  local availableAddons = outfits[currentOutfit][3]
  local addon1 = window:getChildById('addon1')
  local addon2 = window:getChildById('addon2')
  local addon3 = window:getChildById('addon3')
  addon1:setChecked(false)
  addon2:setChecked(false)
  addon3:setChecked(false)

  -- Maybe rework this someday
  if availableAddons == 1 then
    addon1:setChecked(true)
  elseif availableAddons == 2 then
    addon2:setChecked(true)
  elseif availableAddons == 3 then
    addon1:setChecked(true)
    addon2:setChecked(true)
  elseif availableAddons == 4 then
    addon3:setChecked(true)
  elseif availableAddons == 5 then
    addon1:setChecked(true)
    addon3:setChecked(true)
  elseif availableAddons == 6 then
    addon2:setChecked(true)
    addon3:setChecked(true)
  elseif availableAddons == 7 then
    addon1:setChecked(true)
    addon2:setChecked(true)
    addon3:setChecked(true)
  end

end

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

  local firstColor = UIWidget.create()
  window:addChild(firstColor)
  firstColor:setStyle('ColorFirst')

  for i=0,18 do
    for j=0,6 do
      local color = UICheckBox.create()
      window:addChild(color)

      local outfitColor = getOufitColor(j*19 + i)

      color:setStyle('Color')
      color:setBackgroundColor(outfitColor)
      color:setMarginTop(j * 3 + j * 14)
      color:setMarginLeft(i * 3 + i * 14)
    end
  end

  outfits = outfitList
  currentOutfit = 1
  update()
end

function Outfit.destroy()
  if window ~= nil then
    window:destroy()
    window = nil
  end
end

function Outfit.nextType()

  currentOutfit = currentOutfit + 1
  if currentOutfit > #outfits then
    currentOutfit = 1
  end

  update()

end

function Outfit.previousType()
  currentOutfit = currentOutfit - 1
  if currentOutfit <= 0 then
    currentOutfit = #outfits
  end

  update()

end

-- hooked events

connect(Game, { onOpenOutfitWindow = Outfit.create,
                onLogout = Outfit.destroy })

connect(Game, { onLogin = Outfit.test })
