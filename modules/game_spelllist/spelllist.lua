spelllistWindow     = nil
spelllistButton     = nil
spellList           = nil
nameValueLabel      = nil
formulaValueLabel   = nil
vocationValueLabel  = nil
groupValueLabel     = nil
typeValueLabel      = nil
cooldownValueLabel  = nil
levelValueLabel     = nil
manaValueLabel      = nil
premiumValueLabel   = nil

vocationBoxAny      = nil
vocationBoxSorcerer = nil
vocationBoxDruid    = nil
vocationBoxPaladin  = nil
vocationBoxKnight   = nil

groupBoxAny         = nil
groupBoxAttack      = nil
groupBoxHealing     = nil
groupBoxSupport     = nil

premiumBoxAny       = nil
premiumBoxNo        = nil
premiumBoxYes       = nil

vocationRadioGroup  = nil
groupRadioGroup     = nil
premiumRadioGroup   = nil

-- consts
FILTER_PREMIUM_ANY        = 0
FILTER_PREMIUM_NO         = 1
FILTER_PREMIUM_YES        = 2

FILTER_VOCATION_ANY       = 0
FILTER_VOCATION_SORCERER  = 1
FILTER_VOCATION_DRUID     = 2
FILTER_VOCATION_PALADIN   = 3
FILTER_VOCATION_KNIGHT    = 4

FILTER_GROUP_ANY          = 0
FILTER_GROUP_ATTACK       = 1
FILTER_GROUP_HEALING      = 2
FILTER_GROUP_SUPPORT      = 3

-- Filter Settings
local filters = {
  level        = false,
  vocation     = false,
  
  vocationId   = FILTER_VOCATION_ANY,  
  premium      = FILTER_PREMIUM_ANY,
  groupId      = FILTER_GROUP_ANY
}

local spellDisplayOrder = {'Animate Dead', 'Annihilation', 'Avalanche', 'Berserk', 'Blood Rage', 'Brutal Strike', 'Cancel Invisibility', 'Challenge', 'Chameleon', 'Charge', 'Conjure Arrow', 'Conjure Bolt', 'Conjure Explosive Arrow', 'Conjure Piercing Bolt', 'Conjure Poisoned Arrow', 'Conjure Power Bolt', 'Conjure Sniper Arrow', 'Convince Creature', 'Creature Illusion', 'Cure Bleeding', 'Cure Burning', 'Cure Curse', 'Cure Electrification', 'Cure Poison', 'Cure Poison Rune', 'Curser', 'Death Strike', 'Desintegrate', 'Destroy Field', 'Divine Caldera', 'Divine Healing', 'Divine Missile', 'Electrify', 'Enchant Party', 'Enchant Spear', 'Enchant Staff', 'Energy Beam', 'Energy Field', 'Energy Strike', 'Energy Wall', 'Energy Wave', 'Energybomb', 'Envenom', 'Eternal Winter', 'Ethereal Spear', 'Explosion', 'Fierce Berserk', 'Find Person', 'Fire Field', 'Fire Wall', 'Fire Wave', 'Fireball', 'Firebomb', 'Flame Strike', 'Food', 'Front Sweep', 'Great Energy Beam', 'Great Fireball', 'Great Light', 'Groundshaker', 'Haste', 'Heal Friend', 'Heal Party', 'Heavy Magic Missile', 'Hells Core', 'Holy Flash', 'Holy Missile', 'Ice Strike', 'Ice Wave', 'Icicle', 'Ignite', 'Inflict Wound', 'Intense Healing', 'Intense Healing Rune', 'Intense Recovery', 'Intense Wound Cleansing', 'Invisibility', 'Levitate', 'Light', 'Light Healing', 'Light Magic Missile', 'Lightning', 'Magic Rope', 'Magic Shield', 'Magic Wall', 'Mass Healing', 'Paralyze', 'Physical Strike', 'Poison Bomb', 'Poison Field', 'Poison Wall', 'Protect Party', 'Protector', 'Rage of the Skies', 'Recovery', 'Salvation', 'Sharpshooter', 'Soulfire', 'Stalagmite', 'Stone Shower', 'Strong Energy Strike', 'Strong Ethereal Spear', 'Strong Flame Strike', 'Strong Haste', 'Strong Ice Strike', 'Strong Ice Wave', 'Strong Terra Strike', 'Sudden Death', 'Summon Creature', 'Swift Foot', 'Terra Strike', 'Terra Wave', 'Thunderstorm', 'Train Party', 'Ultimate Energy Strike', 'Ultimate Flame Strike', 'Ultimate Healing', 'Ultimate Healing Rune', 'Ultimate Ice Strike', 'Ultimate Light', 'Ultimate Terra Strike', 'Whirlwind Throw', 'Wild Growth', 'Wound Cleansing', 'Wrath of Nature'}

function getIconImageClip(id)
  return (((id-1)%12)*32) .. ' ' .. ((math.ceil(id/12)-1)*32) .. ' 32 32'
end

function setupOptions() 
  if g_game.getClientVersion() >= 950 then -- Vocation is only send in newer clients
    spelllistWindow:getChildById('buttonFilterVocation'):setVisible(true)
  else
    spelllistWindow:getChildById('buttonFilterVocation'):setVisible(false)
  end
end

function init()
  connect(g_game, { onGameStart = setupOptions,
                    onGameEnd 	= resetWindow })
          
  spelllistWindow = g_ui.displayUI('spelllist.otui', modules.game_interface.getRightPanel())
  spelllistWindow:hide()
  
  spelllistButton = TopMenu.addRightGameToggleButton('spelllistButton', tr('Spell List'), 'spelllist.png', toggle)
  spelllistButton:setOn(false)
  
  nameValueLabel        = spelllistWindow:getChildById('labelNameValue')
  formulaValueLabel     = spelllistWindow:getChildById('labelFormulaValue')
  vocationValueLabel    = spelllistWindow:getChildById('labelVocationValue')
  groupValueLabel       = spelllistWindow:getChildById('labelGroupValue')
  typeValueLabel        = spelllistWindow:getChildById('labelTypeValue')
  cooldownValueLabel    = spelllistWindow:getChildById('labelCooldownValue')
  levelValueLabel       = spelllistWindow:getChildById('labelLevelValue')
  manaValueLabel        = spelllistWindow:getChildById('labelManaValue')
  premiumValueLabel     = spelllistWindow:getChildById('labelPremiumValue')
  
  vocationBoxAny        = spelllistWindow:getChildById('vocationBoxAny')
  vocationBoxSorcerer   = spelllistWindow:getChildById('vocationBoxSorcerer')
  vocationBoxDruid      = spelllistWindow:getChildById('vocationBoxDruid')
  vocationBoxPaladin    = spelllistWindow:getChildById('vocationBoxPaladin')
  vocationBoxKnight     = spelllistWindow:getChildById('vocationBoxKnight')
  
  groupBoxAny           = spelllistWindow:getChildById('groupBoxAny')
  groupBoxAttack        = spelllistWindow:getChildById('groupBoxAttack')
  groupBoxHealing       = spelllistWindow:getChildById('groupBoxHealing')
  groupBoxSupport       = spelllistWindow:getChildById('groupBoxSupport')
  
  premiumBoxAny         = spelllistWindow:getChildById('premiumBoxAny')
  premiumBoxYes         = spelllistWindow:getChildById('premiumBoxYes')
  premiumBoxNo          = spelllistWindow:getChildById('premiumBoxNo')

  vocationRadioGroup = UIRadioGroup.create()
  vocationRadioGroup:addWidget(vocationBoxAny)
  vocationRadioGroup:addWidget(vocationBoxSorcerer)
  vocationRadioGroup:addWidget(vocationBoxDruid)
  vocationRadioGroup:addWidget(vocationBoxPaladin)
  vocationRadioGroup:addWidget(vocationBoxKnight)
  
  groupRadioGroup = UIRadioGroup.create()
  groupRadioGroup:addWidget(groupBoxAny)
  groupRadioGroup:addWidget(groupBoxAttack)
  groupRadioGroup:addWidget(groupBoxHealing)
  groupRadioGroup:addWidget(groupBoxSupport)
  
  premiumRadioGroup = UIRadioGroup.create()
  premiumRadioGroup:addWidget(premiumBoxAny)
  premiumRadioGroup:addWidget(premiumBoxYes)
  premiumRadioGroup:addWidget(premiumBoxNo)
  
  premiumRadioGroup:selectWidget(premiumBoxAny)
  vocationRadioGroup:selectWidget(vocationBoxAny)
  groupRadioGroup:selectWidget(groupBoxAny)
  
  vocationRadioGroup.onSelectionChange  = toggleFilter
  groupRadioGroup.onSelectionChange     = toggleFilter
  premiumRadioGroup.onSelectionChange   = toggleFilter
  
  spellList = spelllistWindow:getChildById('spellList')   

  g_keyboard.bindKeyPress('Down', function() spellList:focusNextChild(KeyboardFocusReason) end, spelllistWindow)
  g_keyboard.bindKeyPress('Up', function() spellList:focusPreviousChild(KeyboardFocusReason) end, spelllistWindow)

  for i = 1, #spellDisplayOrder do
    local spell = spellDisplayOrder[i]
    local info = SpellInfo[spell]
    local tmpLabel = g_ui.createWidget('SpellListLabel', spellList)
    tmpLabel:setId(spell)
    tmpLabel:setText(spell .. '\n\'' .. info.words .. '\'')
    tmpLabel:setPhantom(false)
  
    if not(SpellIcons[info.icon]) then 
      perror('Spell icon \'' .. info.icon .. '\' not found.') 
    else
      tmpLabel:setImageClip(getIconImageClip(SpellIcons[info.icon][1]))
    end
  
    tmpLabel.onClick = updateSpellInformation
  end
 
  connect(spellList, { onChildFocusChange = function(self, focusedChild)
                          if focusedChild == nil then return end
                          updateSpellInformation(focusedChild)
                        end })   
 
  setupOptions()
end

function terminate()
  disconnect(g_game, { onGameStart = setupOptions,
                       onGameEnd = resetWindow })

  disconnect(spellList, { onChildFocusChange = function(self, focusedChild)
                          if focusedChild == nil then return end
                          updateSpellInformation(focusedChild)
                        end })   

  spelllistButton:destroy()
  spelllistButton      = nil
  spelllistWindow:destroy()
  spelllistWindow      = nil
  
  vocationRadioGroup:destroy()
  vocationRadioGroup   = nil
  groupRadioGroup:destroy()
  groupRadioGroup      = nil
  premiumRadioGroup:destroy()
  premiumRadioGroup    = nil
  
  spellList            = nil
  nameValueLabel       = nil
  formulaValueLabel    = nil
  vocationValueLabel   = nil
  groupValueLabel      = nil
  typeValueLabel       = nil
  cooldownValueLabel   = nil
  levelValueLabel      = nil
  manaValueLabel       = nil
  premiumValueLabel    = nil

  vocationBoxAny       = nil
  vocationBoxSorcerer  = nil
  vocationBoxDruid     = nil
  vocationBoxPaladin   = nil
  vocationBoxKnight    = nil

  groupBoxAny          = nil
  groupBoxAttack       = nil
  groupBoxHealing      = nil
  groupBoxSupport      = nil

  premiumBoxAny        = nil
  premiumBoxNo         = nil
  premiumBoxYes        = nil
end

function updateSpelllist()
  for i = 1, #spellDisplayOrder do
    local spell = spellDisplayOrder[i]
    local info = SpellInfo[spell]
    local tmpLabel = spellList:getChildById(spell)
  
    local localPlayer = g_game.getLocalPlayer()
    if (not(filters.level) or info.level <= localPlayer:getLevel()) and (not(filters.vocation) or table.find(info.vocations, localPlayer:getVocation())) and (filters.vocationId == FILTER_VOCATION_ANY or table.find(info.vocations, filters.vocationId) or table.find(info.vocations, filters.vocationId+4)) and (filters.groupId == FILTER_GROUP_ANY or info.group[filters.groupId]) and (filters.premium == FILTER_PREMIUM_ANY or (info.premium and filters.premium == FILTER_PREMIUM_YES) or (not(info.premium) and filters.premium == FILTER_PREMIUM_NO)) then
      tmpLabel:setVisible(true)
    else
      tmpLabel:setVisible(false)
    end
  end
end

function updateSpellInformation(widget)
  local spell = widget:getId()
  
  local name     = ''
  local formula  = ''
  local vocation = ''
  local group    = ''
  local type     = ''
  local cooldown = ''
  local level    = ''
  local mana     = ''
  local premium  = ''
  
  if SpellInfo[spell] then
    local info = SpellInfo[spell]
    
    name    = spell
    formula = info.words

    for i = 1, #info.vocations do
      local vocationId = info.vocations[i]
      if vocationId <= 4 or not(table.find(info.vocations, (vocationId-4))) then
        vocation = vocation .. (vocation:len() == 0 and '' or ', ') .. VocationNames[vocationId]
      end
    end
    
    cooldown = (info.exhaustion / 1000) .. 's'
    for groupId, groupName in ipairs(SpellGroups) do
      if info.group[groupId] then
        group    = group .. (group:len() == 0 and '' or ' / ') .. groupName
        cooldown = cooldown .. ' / ' .. (info.group[groupId] / 1000) .. 's'
      end
    end
  
    type    = info.type
    level   = info.level
    mana    = info.mana .. ' / ' .. info.soul
    premium = (info.premium and 'yes' or 'no')
  end
    
  nameValueLabel:setText(name)
  formulaValueLabel:setText(formula)
  vocationValueLabel:setText(vocation)
  groupValueLabel:setText(group)
  typeValueLabel:setText(type)
  cooldownValueLabel:setText(cooldown)
  levelValueLabel:setText(level)
  manaValueLabel:setText(mana)
  premiumValueLabel:setText(premium)
end

function toggle()
  if spelllistButton:isOn() then
    spelllistWindow:hide()
    spelllistButton:setOn(false)
  else
    spelllistWindow:show()
    spelllistButton:setOn(true)
  end
end

function toggleFilter(widget, selectedWidget)
  if widget == vocationRadioGroup then
    local boxId = selectedWidget:getId()
    if boxId == 'vocationBoxAny' then
      filters.vocationId = FILTER_VOCATION_ANY
    elseif boxId == 'vocationBoxSorcerer' then
      filters.vocationId = FILTER_VOCATION_SORCERER
    elseif boxId == 'vocationBoxDruid' then
      filters.vocationId = FILTER_VOCATION_DRUID
    elseif boxId == 'vocationBoxPaladin' then
      filters.vocationId = FILTER_VOCATION_PALADIN
    elseif boxId == 'vocationBoxKnight' then
      filters.vocationId = FILTER_VOCATION_KNIGHT
    end
  elseif widget == groupRadioGroup then
    local boxId = selectedWidget:getId()
    if boxId == 'groupBoxAny' then
      filters.groupId = FILTER_GROUP_ANY
    elseif boxId == 'groupBoxAttack' then
      filters.groupId = FILTER_GROUP_ATTACK
    elseif boxId == 'groupBoxHealing' then
      filters.groupId = FILTER_GROUP_HEALING
    elseif boxId == 'groupBoxSupport' then
      filters.groupId = FILTER_GROUP_SUPPORT
    end
  elseif widget == premiumRadioGroup then
    local boxId = selectedWidget:getId()
    if boxId == 'premiumBoxAny' then
      filters.premium = FILTER_PREMIUM_ANY
    elseif boxId == 'premiumBoxNo' then
      filters.premium = FILTER_PREMIUM_NO
    elseif boxId == 'premiumBoxYes' then
      filters.premium = FILTER_PREMIUM_YES
    end
  else
    local id = widget:getId()
    if id == 'buttonFilterLevel' then
      filters.level = not(filters.level)
      widget:setOn(filters.level)
    elseif id == 'buttonFilterVocation' then
      filters.vocation = not(filters.vocation)
      widget:setOn(filters.vocation)
    end
  end
  
  updateSpelllist()
end

function resetWindow()
  spelllistWindow:hide()
  spelllistButton:setOn(false)

  -- Resetting filters
  filters.level = false
  filters.vocation = false
  
  local buttonFilterLevel = spelllistWindow:getChildById('buttonFilterLevel')
  buttonFilterLevel:setOn(filters.level)
  
  local buttonFilterVocation = spelllistWindow:getChildById('buttonFilterVocation')
  buttonFilterVocation:setOn(filters.vocation)
  
  vocationRadioGroup:selectWidget(vocationBoxAny)
  groupRadioGroup:selectWidget(groupBoxAny)
  premiumRadioGroup:selectWidget(premiumBoxAny)
  
  updateSpelllist()
end