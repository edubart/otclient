Outfit = {}

-- private variables
local addonSets = {
  [1] = { 1 },
  [2] = { 2 },
  [3] = { 1, 2 },
  [4] = { 3 },
  [5] = { 1, 3 },
  [6] = { 2, 3 },
  [7] = { 1, 2, 3 }
}
local outfitWindow
local outfit
local outfits
local outfitCreature
local currentOutfit = 1

local addons
local currentColorBox
local currentClotheButtonBox
local colorBoxes = {}

local mount
local mounts
local mountCreature
local currentMount = 1

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

local function updateOutfit()
  if table.empty(outfits) then
    return
  end
  local nameWidget = outfitWindow:getChildById('outfitName')
  nameWidget:setText(outfits[currentOutfit][2])

  local availableAddons = outfits[currentOutfit][3]

  local prevAddons = {}
  for k, addon in pairs(addons) do
    prevAddons[k] = addon.widget:isChecked()
    addon.widget:setChecked(false)
    addon.widget:setEnabled(false)
  end

  if availableAddons > 0 then
    for _, i in pairs(addonSets[availableAddons]) do
      addons[i].widget:setEnabled(true)
    end
  end

  outfit.addons = 0
  for k, addon in pairs(prevAddons) do
    if addon and addons[k].widget:isEnabled() then
      addons[k].widget:setChecked(true)
    end
  end

  outfit.type = outfits[currentOutfit][1]
  outfitCreature:setOutfit(outfit)
end

function updateMount()
  if table.empty(mounts) or not mount then
    return
  end
  local nameMountWidget = outfitWindow:getChildById('mountName')
  nameMountWidget:setText(mounts[currentMount][2])
  
  mount.type = mounts[currentMount][1]
  mountCreature:setOutfit(mount)
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

function Outfit.create(creatureOutfit, outfitList, creatureMount, mountList)
  outfitCreature = creatureOutfit
  mountCreature = creatureMount
  outfits = outfitList
  mounts = mountList
  Outfit.destroy() 

  outfitWindow = g_ui.displayUI('outfitwindow.otui')
  outfit = outfitCreature:getOutfit()
  if mountCreature then 
    mount = mountCreature:getOutfit()
  end
  
  addons = {
    [1] = {widget = outfitWindow:getChildById('addon1'), value = 1},
    [2] = {widget = outfitWindow:getChildById('addon2'), value = 2},
    [3] = {widget = outfitWindow:getChildById('addon3'), value = 4}
  }
  
  for k, addon in pairs(addons) do
    addon.widget.onCheckChange = function(self) onAddonCheckChange(self, addon.value) end
  end
  
  if outfit.addons > 0 then
    for _, i in pairs(addonSets[outfit.addons]) do
      addons[i].widget:setChecked(true)
    end
  end
  
  currentClotheButtonBox = outfitWindow:getChildById('head')
  outfitWindow:getChildById('head').onCheckChange = onClotheCheckChange
  outfitWindow:getChildById('primary').onCheckChange = onClotheCheckChange
  outfitWindow:getChildById('secondary').onCheckChange = onClotheCheckChange
  outfitWindow:getChildById('detail').onCheckChange = onClotheCheckChange

  local outfitCreatureBox = outfitWindow:getChildById('outfitCreatureBox')
  local colorBoxPanel = outfitWindow:getChildById('colorBoxPanel')
  outfitCreatureBox:setCreature(outfitCreature)

  local mountCreatureBox = outfitWindow:getChildById('mountCreatureBox')
  mountCreatureBox:setCreature(mountCreature)

  for j=0,6 do
    for i=0,18 do
      local colorBox = g_ui.createWidget('ColorBox', colorBoxPanel)
      local outfitColor = getOufitColor(j*19 + i)
      colorBox:setImageColor(outfitColor)
      colorBox:setId('colorBox' .. j*19+i)
      colorBox.colorId = j*19 + i

      if j*19 + i == outfit.head then
        currentColorBox = colorBox
        colorBox:setChecked(true)
      end
      colorBox.onCheckChange = onColorCheckChange
      table.insert(colorBoxes, colorBox)
    end
  end

  currentOutfit = 1
  for i=1,#outfitList do
    if outfitList[i][1] == outfit.type then
      currentOutfit = i
      break
    end
  end
  currentMount = 1
  for i=1,#mountList do
    if mount and mountList[i][1] == mount.type then
      currentMount = i
      break
    end
  end

  updateOutfit()
  updateMount()
end

function Outfit.destroy()
  if outfitWindow then
    outfitWindow:destroy()
    outfitWindow = nil
    outfitCreature = nil
    mountCreature = nil
    currentColorBox = nil
    currentClotheButtonBox = nil
  end
end

function Outfit.randomize()
  local outfitTemplate = {
    outfitWindow:getChildById('head'),
    outfitWindow:getChildById('primary'),
    outfitWindow:getChildById('secondary'),
    outfitWindow:getChildById('detail')
  }
  
  for k, section in pairs(outfitTemplate) do
    section:setChecked(true)
    colorBoxes[math.random(1, #colorBoxes)]:setChecked(true)
    section:setChecked(false)
  end
  outfitTemplate[1]:setChecked(true)
end

function Outfit.accept()
  if mount then outfit.mount = mount.type end
  g_game.changeOutfit(outfit)
  Outfit.destroy()
end

function Outfit.nextOutfitType()
  if not outfits then
    return
  end
  currentOutfit = currentOutfit + 1
  if currentOutfit > #outfits then
    currentOutfit = 1
  end
  updateOutfit()
end

function Outfit.previousOutfitType()
  if not outfits then
    return
  end
  currentOutfit = currentOutfit - 1
  if currentOutfit <= 0 then
    currentOutfit = #outfits
  end
  updateOutfit()
end

function Outfit.nextMountType()
  if not mounts then
    return
  end
  currentMount = currentMount + 1
  if currentMount > #mounts then
    currentMount = 1
  end
  updateMount()
end

function Outfit.previousMountType()
  if not mounts then
    return
  end
  currentMount = currentMount - 1
  if currentMount <= 0 then
    currentMount = #mounts
  end
  updateMount()
end
