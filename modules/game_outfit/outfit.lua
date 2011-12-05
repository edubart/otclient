Outfit = {}

-- private variables
local window = nil
local m_creature = nil
local m_outfit = nil
local m_outfits = nil
local m_currentOutfit = 1
local m_currentColor = nil
local m_currentClothe = nil

-- private functions
local function onAddonCheckChange(addon, value)
  if addon:isChecked() then
    m_outfit.addons = m_outfit.addons + value
  else
    m_outfit.addons = m_outfit.addons - value
  end
  m_creature:setOutfit(m_outfit)
end

local function onColorCheckChange(color)
  if color == m_currentColor then
    color.onCheckChange = nil
    color:setChecked(true)
    color.onCheckChange = onColorCheckChange
  else
    m_currentColor.onCheckChange = nil
    m_currentColor:setChecked(false)
    m_currentColor.onCheckChange = onColorCheckChange

    m_currentColor = color

    if m_currentClothe:getId() == 'head' then
      m_outfit.head = m_currentColor.colorId
    elseif m_currentClothe:getId() == 'primary' then
      m_outfit.body = m_currentColor.colorId
    elseif m_currentClothe:getId() == 'secondary' then
      m_outfit.legs = m_currentColor.colorId
    elseif m_currentClothe:getId() == 'detail' then
      m_outfit.feet = m_currentColor.colorId
    end
      
    m_creature:setOutfit(m_outfit)
  end
end

local function onClotheCheckChange(clothe)
  if clothe == m_currentClothe then
    clothe.onCheckChange = nil
    clothe:setChecked(true)
    clothe.onCheckChange = onClotheCheckChange
  else
    m_currentClothe.onCheckChange = nil
    m_currentClothe:setChecked(false)
    m_currentClothe.onCheckChange = onClotheCheckChange

    m_currentClothe = clothe
    
    local color = 0
    if m_currentClothe:getId() == 'head' then
      color = m_outfit.head
    elseif m_currentClothe:getId() == 'primary' then
      color = m_outfit.body
    elseif m_currentClothe:getId() == 'secondary' then
      color = m_outfit.legs
    elseif m_currentClothe:getId() == 'detail' then
      color = m_outfit.feet
    end
    window:getChildById('color' .. color):setChecked(true)
  end
end

local function update()
  local nameWidget = window:getChildById('name')
  nameWidget:setText(m_outfits[m_currentOutfit][2])

  local availableAddons = m_outfits[m_currentOutfit][3]
  local addon1 = window:getChildById('addon1')
  local addon2 = window:getChildById('addon2')
  local addon3 = window:getChildById('addon3')
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

  m_outfit.type = m_outfits[m_currentOutfit][1]
  m_outfit.addons = 0
  m_creature:setOutfit(m_outfit)

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
  
  m_currentClothe = window:getChildById('head')
  window:getChildById('head').onCheckChange = onClotheCheckChange
  window:getChildById('primary').onCheckChange = onClotheCheckChange
  window:getChildById('secondary').onCheckChange = onClotheCheckChange
  window:getChildById('detail').onCheckChange = onClotheCheckChange

  local creatureWidget = window:getChildById('creature')
  creatureWidget:setCreature(creature)

  for i=0,18 do
    for j=0,6 do
      local color = UICheckBox.create()
      window:addChild(color)

      local outfitColor = getOufitColor(j*19 + i)
      color:setId('color' .. j*19+i)
      color.colorId = j*19 + i
      color:setStyle('Color')
      color:setBackgroundColor(outfitColor)
      color:setMarginTop(j * 3 + j * 14)
      color:setMarginLeft(10 + i * 3 + i * 14)

      if j*19 + i == m_outfit.head then
        m_currentColor = color
        color:setChecked(true)
      end

      color.onCheckChange = onColorCheckChange
    end
  end

  m_creature = creature
  m_outfits = outfitList

  m_currentOutfit = 1
  for i=1,#outfitList do
    if outfitList[i][1] == m_outfit.type then
      m_currentOutfit = i
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
  m_currentOutfit = m_currentOutfit + 1
  if m_currentOutfit > #m_outfits then
    m_currentOutfit = 1
  end

  update()
end

function Outfit.previousType()
  m_currentOutfit = m_currentOutfit - 1
  if m_currentOutfit <= 0 then
    m_currentOutfit = #m_outfits
  end

  update()
end

-- hooked events

connect(Game, { onOpenOutfitWindow = Outfit.create,
                onLogout = Outfit.destroy })

connect(Game, { onLogin = Outfit.test })
