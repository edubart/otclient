Outfit = {}

-- private variables
local outfitWindow
local outfitCreature
local outfit
local outfits
local currentOutfit = 1
local currentColorBox
local currentClotheButtonBox

-- private functions
local function onAddonCheckChange(addon, value)
  if addon:isChecked() then
    outfit.addons = outfit.addons + value
  else
    outfit.addons = outfit.addons - value
  end
  outfitCreature:setOutfit(outfit)
end

local function onColorCheckChange(colorBox)
  if colorBox == currentColorBox then
    colorBox.onCheckChange = nil
    colorBox:setChecked(true)
    colorBox.onCheckChange = onColorCheckChange
  else
    currentColorBox.onCheckChange = nil
    currentColorBox:setChecked(false)
    currentColorBox.onCheckChange = onColorCheckChange

    currentColorBox = colorBox

    if currentClotheButtonBox:getId() == 'head' then
      outfit.head = currentColorBox.colorId
    elseif currentClotheButtonBox:getId() == 'primary' then
      outfit.body = currentColorBox.colorId
    elseif currentClotheButtonBox:getId() == 'secondary' then
      outfit.legs = currentColorBox.colorId
    elseif currentClotheButtonBox:getId() == 'detail' then
      outfit.feet = currentColorBox.colorId
    end

    outfitCreature:setOutfit(outfit)
  end
end

local function onClotheCheckChange(clotheButtonBox)
  if clotheButtonBox == currentClotheButtonBox then
    clotheButtonBox.onCheckChange = nil
    clotheButtonBox:setChecked(true)
    clotheButtonBox.onCheckChange = onClotheCheckChange
  else
    currentClotheButtonBox.onCheckChange = nil
    currentClotheButtonBox:setChecked(false)
    currentClotheButtonBox.onCheckChange = onClotheCheckChange

    currentClotheButtonBox = clotheButtonBox

    local colorId = 0
    if currentClotheButtonBox:getId() == 'head' then
      colorId = outfit.head
    elseif currentClotheButtonBox:getId() == 'primary' then
      colorId = outfit.body
    elseif currentClotheButtonBox:getId() == 'secondary' then
      colorId = outfit.legs
    elseif currentClotheButtonBox:getId() == 'detail' then
      colorId = outfit.feet
    end
    outfitWindow:recursiveGetChildById('colorBox' .. colorId):setChecked(true)
  end
end

local function update()
  local nameWidget = outfitWindow:getChildById('outfitName')
  nameWidget:setText(outfits[currentOutfit][2])

  local availableAddons = outfits[currentOutfit][3]
  local addon1 = outfitWindow:getChildById('addon1')
  local addon2 = outfitWindow:getChildById('addon2')
  local addon3 = outfitWindow:getChildById('addon3')
  addon1:setChecked(false)
  addon2:setChecked(false)
  addon3:setChecked(false)
  addon1.onCheckChange = function(self) onAddonCheckChange(self, 1) end
  addon2.onCheckChange = function(self) onAddonCheckChange(self, 2) end
  addon3.onCheckChange = function(self) onAddonCheckChange(self, 4) end
  addon1:setEnabled(false)
  addon2:setEnabled(false)
  addon3:setEnabled(false)

  -- Maybe rework this someday
  if availableAddons == 1 then
    addon1:setEnabled(true)
  elseif availableAddons == 2 then
    addon2:setEnabled(true)
  elseif availableAddons == 3 then
    addon1:setEnabled(true)
    addon2:setEnabled(true)
  elseif availableAddons == 4 then
    addon3:setEnabled(true)
  elseif availableAddons == 5 then
    addon1:setEnabled(true)
    addon3:setEnabled(true)
  elseif availableAddons == 6 then
    addon2:setEnabled(true)
    addon3:setEnabled(true)
  elseif availableAddons == 7 then
    addon1:setEnabled(true)
    addon2:setEnabled(true)
    addon3:setEnabled(true)
  end

  outfit.type = outfits[currentOutfit][1]
  outfit.addons = 0
  outfitCreature:setOutfit(outfit)
end

-- public functions
function Outfit.init()
  connect(g_game, { onOpenOutfitWindow = Outfit.create,
                    onGameEnd = Outfit.destroy })
end

function Outfit.terminate()
  disconnect(g_game, { onOpenOutfitWindow = Outfit.create,
                       onGameEnd = Outfit.destroy })
  Outfit.destroy()

  Outfit = nil
end

function Outfit.create(creature, outfitList)
  outfitCreature = creature
  outfits = outfitList
  Outfit.destroy()

  outfitWindow = displayUI('outfit.otui')
  --outfitWindow:lock()

  outfit = outfitCreature:getOutfit()

  currentClotheButtonBox = outfitWindow:getChildById('head')
  outfitWindow:getChildById('head').onCheckChange = onClotheCheckChange
  outfitWindow:getChildById('primary').onCheckChange = onClotheCheckChange
  outfitWindow:getChildById('secondary').onCheckChange = onClotheCheckChange
  outfitWindow:getChildById('detail').onCheckChange = onClotheCheckChange

  local outfitCreatureBox = outfitWindow:getChildById('outfitCreatureBox')
  local colorBoxPanel = outfitWindow:getChildById('colorBoxPanel')
  outfitCreatureBox:setCreature(outfitCreature)

  for j=0,6 do
    for i=0,18 do
      local colorBox = createWidget('ColorBox', colorBoxPanel)
      local outfitColor = getOufitColor(j*19 + i)
      colorBox:setImageColor(outfitColor)
      colorBox:setId('colorBox' .. j*19+i)
      colorBox.colorId = j*19 + i

      if j*19 + i == outfit.head then
        currentColorBox = colorBox
        colorBox:setChecked(true)
      end
      colorBox.onCheckChange = onColorCheckChange
    end
  end

  currentOutfit = 1
  for i=1,#outfitList do
    if outfitList[i][1] == outfit.type then
      currentOutfit = i
      break
    end
  end

  update()
end

function Outfit.destroy()
  if outfitWindow then
    outfitWindow:destroy()
    outfitWindow = nil
    outfitCreature = nil
    currentColorBox = nil
    currentClotheButtonBox = nil
  end
end

function Outfit.accept()
  g_game.changeOutfit(outfit)
  Outfit.destroy()
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

