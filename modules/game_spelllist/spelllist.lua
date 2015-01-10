local SpelllistProfile = 'Default'

spelllistWindow       = nil
spelllistButton       = nil
spellList             = nil
nameValueLabel        = nil
formulaValueLabel     = nil
vocationValueLabel    = nil
groupValueLabel       = nil
typeValueLabel        = nil
cooldownValueLabel    = nil
levelValueLabel       = nil
manaValueLabel        = nil
premiumValueLabel     = nil
descriptionValueLabel = nil

vocationBoxAny        = nil
vocationBoxSorcerer   = nil
vocationBoxDruid      = nil
vocationBoxPaladin    = nil
vocationBoxKnight     = nil

groupBoxAny           = nil
groupBoxAttack        = nil
groupBoxHealing       = nil
groupBoxSupport       = nil

premiumBoxAny         = nil
premiumBoxNo          = nil
premiumBoxYes         = nil

vocationRadioGroup    = nil
groupRadioGroup       = nil
premiumRadioGroup     = nil

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

function getSpelllistProfile()
  return SpelllistProfile
end

function setSpelllistProfile(name)
  if SpelllistProfile == name then return end

  if SpelllistSettings[name] and SpellInfo[name] then
    local oldProfile = SpelllistProfile
    SpelllistProfile = name
    changeSpelllistProfile(oldProfile)
  else
    perror('Spelllist profile \'' .. name .. '\' could not be set.')
  end
end

function online()
  if g_game.getFeature(GameSpellList) then
    spelllistButton:show()
  else
    spelllistButton:hide()
  end

  -- Vocation is only send in newer clients
  if g_game.getClientVersion() >= 950 then
    spelllistWindow:getChildById('buttonFilterVocation'):setVisible(true)
  else
    spelllistWindow:getChildById('buttonFilterVocation'):setVisible(false)
  end
end

function offline()
  resetWindow()
end

function init()
  connect(g_game, { onGameStart = online,
                    onGameEnd   = offline })

  spelllistWindow = g_ui.displayUI('spelllist', modules.game_interface.getRightPanel())
  spelllistWindow:hide()

  spelllistButton = modules.client_topmenu.addRightGameToggleButton('spelllistButton', tr('Spell List'), '/images/topbuttons/spelllist', toggle)
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
  descriptionValueLabel = spelllistWindow:getChildById('labelDescriptionValue')

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

  initializeSpelllist()
  resizeWindow()

  if g_game.isOnline() then
    online()
  end
end

function terminate()
  disconnect(g_game, { onGameStart = online,
                       onGameEnd   = offline })

  disconnect(spellList, { onChildFocusChange = function(self, focusedChild)
                          if focusedChild == nil then return end
                          updateSpellInformation(focusedChild)
                        end })

  spelllistWindow:destroy()
  spelllistButton:destroy()

  vocationRadioGroup:destroy()
  groupRadioGroup:destroy()
  premiumRadioGroup:destroy()
end

function initializeSpelllist()
  for i = 1, #SpelllistSettings[SpelllistProfile].spellOrder do
    local spell = SpelllistSettings[SpelllistProfile].spellOrder[i]
    local info = SpellInfo[SpelllistProfile][spell]

    local tmpLabel = g_ui.createWidget('SpellListLabel', spellList)
    tmpLabel:setId(spell)
    tmpLabel:setText(spell .. '\n\'' .. info.words .. '\'')
    tmpLabel:setPhantom(false)

    local iconId = tonumber(info.icon)
    if not iconId and SpellIcons[info.icon] then
      iconId = SpellIcons[info.icon][1]
    end

    if not(iconId) then
      perror('Spell icon \'' .. info.icon .. '\' not found.')
    end

    tmpLabel:setHeight(SpelllistSettings[SpelllistProfile].iconSize.height + 4)
    tmpLabel:setTextOffset(topoint((SpelllistSettings[SpelllistProfile].iconSize.width + 10) .. ' ' .. (SpelllistSettings[SpelllistProfile].iconSize.height - 32)/2 + 3))
    tmpLabel:setImageSource(SpelllistSettings[SpelllistProfile].iconFile)
    tmpLabel:setImageClip(Spells.getImageClip(iconId, SpelllistProfile))
    tmpLabel:setImageSize(tosize(SpelllistSettings[SpelllistProfile].iconSize.width .. ' ' .. SpelllistSettings[SpelllistProfile].iconSize.height))
    tmpLabel.onClick = updateSpellInformation
  end

  connect(spellList, { onChildFocusChange = function(self, focusedChild)
                          if focusedChild == nil then return end
                          updateSpellInformation(focusedChild)
                        end })
end

function changeSpelllistProfile(oldProfile)
  -- Delete old labels
  for i = 1, #SpelllistSettings[oldProfile].spellOrder do
    local spell = SpelllistSettings[oldProfile].spellOrder[i]
    local tmpLabel = spellList:getChildById(spell)

    tmpLabel:destroy()
  end

  -- Create new spelllist and ajust window
  initializeSpelllist()
  resizeWindow()
  resetWindow()
end

function updateSpelllist()
  for i = 1, #SpelllistSettings[SpelllistProfile].spellOrder do
    local spell = SpelllistSettings[SpelllistProfile].spellOrder[i]
    local info = SpellInfo[SpelllistProfile][spell]
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

  local name        = ''
  local formula     = ''
  local vocation    = ''
  local group       = ''
  local type        = ''
  local cooldown    = ''
  local level       = ''
  local mana        = ''
  local premium     = ''
  local description = ''

  if SpellInfo[SpelllistProfile][spell] then
    local info = SpellInfo[SpelllistProfile][spell]

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
    description = info.description or '-'
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
  descriptionValueLabel:setText(description)
end

function toggle()
  if spelllistButton:isOn() then
    spelllistButton:setOn(false)
    spelllistWindow:hide()
  else
    spelllistButton:setOn(true)
    spelllistWindow:show()
    spelllistWindow:raise()
    spelllistWindow:focus()
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

function resizeWindow()
  spelllistWindow:setWidth(SpelllistSettings['Default'].spellWindowWidth + SpelllistSettings[SpelllistProfile].iconSize.width - 32)
  spellList:setWidth(SpelllistSettings['Default'].spellListWidth + SpelllistSettings[SpelllistProfile].iconSize.width - 32)
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
