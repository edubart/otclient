Outfit = {}

-- private variables
local window = nil
local m_creature = nil
local m_outfit = nil
local m_outfits = nil
local m_currentOutfit = 1
local m_currentColor = nil

-- private functions
local function onAddonCheckChange(addon, value)
  if addon:isChecked() then
    m_outfit.addons = m_outfit.addons + value
  else
    m_outfit.addons = m_outfit.addons - value
  end
  m_creature:setOutfit(m_outfit)
end

local function update()
  local nameWidget = window:getChildById('name')
  nameWidget:setText(m_outfits[currentOutfit][2])

  local availableAddons = m_outfits[currentOutfit][3]
  local addon1 = window:getChildById('addon1')
  local addon2 = window:getChildById('addon2')
  local addon3 = window:getChildById('addon3')
  addon1.onCheckChange = function() onAddonCheckChange(addon1, 1) end
  addon2.onCheckChange = function() onAddonCheckChange(addon2, 2) end
  addon3.onCheckChange = function() onAddonCheckChange(addon3, 4) end
  addon1:setChecked(false)
  addon2:setChecked(false)
  addon3:setChecked(false)
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

  m_outfit.type = m_outfits[currentOutfit][1]
  m_outfit.addons = 0
  m_creature:setOutfit(m_outfit)

end

local function onColorCheckChange(color)
  if color == m_currentColor then
    color.onCheckChange = nil
    color:setChecked(true)
    color.onCheckChange = function() onColorCheckChange(color) end
  else
    m_currentColor.onCheckChange = nil
    m_currentColor:setChecked(false)
    local color2 = m_currentColor
    m_currentColor.onCheckChange = function() onColorCheckChange(color2) end

    m_currentColor = color

    m_outfit.head = m_currentColor.colorId
    m_creature:setOutfit(m_outfit)
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
  window = UI.display('outfit.otui', { parent = UI.root })
  window:lock()

  m_outfit = creature:getOutfit()

  local creatureWidget = window:getChildById('creature')
  creatureWidget:setCreature(creature)

  for i=0,18 do
    for j=0,6 do
      local color = UICheckBox.create()
      window:addChild(color)

      local outfitColor = getOufitColor(j*19 + i)
      color.colorId = j*19 + i
      color:setStyle('Color')
      color:setBackgroundColor(outfitColor)
      color:setMarginTop(j * 3 + j * 14)
      color:setMarginLeft(10 + i * 3 + i * 14)

      if j*19 + i == m_outfit.head then
        m_currentColor = color
        color:setChecked(true)
      end

      color.onCheckChange = function() onColorCheckChange(color) end
    end
  end

  m_creature = creature
  m_outfits = outfitList

  currentOutfit = 1
  for i=1,#outfitList do
    if outfitList[i][1] == m_outfit.type then
      currentOutfit = i
      break
    end
  end

  update()
end

function Outfit.destroy()
  if window ~= nil then
    window:destroy()
    window = nil
  end
end

function Outfit.accept()
  Game.setOutfit(m_outfit)
  Outfit.destroy()
end

function Outfit.nextType()
  currentOutfit = currentOutfit + 1
  if currentOutfit > #m_outfits then
    currentOutfit = 1
  end

  update()
end

function Outfit.previousType()
  currentOutfit = currentOutfit - 1
  if currentOutfit <= 0 then
    currentOutfit = #m_outfits
  end

  update()
end

-- hooked events

connect(Game, { onOpenOutfitWindow = Outfit.create,
                onLogout = Outfit.destroy })

connect(Game, { onLogin = Outfit.test })
