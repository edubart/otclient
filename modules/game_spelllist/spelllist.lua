local SpelllistProfile = 'Default'

spelllistWindow       = nil
spelllistButton       = nil
spellList             = nil
nameValueLabel        = nil
formulaValueLabel     = nil
groupValueLabel       = nil
typeValueLabel        = nil
cooldownValueLabel    = nil
strenghtValueLabel       = nil
dexterityValueLabel       = nil
faithValueLabel       = nil
intelligenceValueLabel = nil
manaValueLabel        = nil
premiumValueLabel     = nil
descriptionValueLabel = nil

groupBoxAny           = nil
groupBoxAttack        = nil
groupBoxHealing       = nil
groupBoxSupport       = nil

groupRadioGroup       = nil

-- consts
FILTER_GROUP_ANY          = 0
FILTER_GROUP_ATTACK       = 1
FILTER_GROUP_HEALING      = 2
FILTER_GROUP_SUPPORT      = 3

-- Filter Settings
local filters = {
  canUse = false,
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
  groupValueLabel       = spelllistWindow:getChildById('labelGroupValue')
  typeValueLabel        = spelllistWindow:getChildById('labelTypeValue')
  cooldownValueLabel    = spelllistWindow:getChildById('labelCooldownValue')
  strenghtValueLabel       = spelllistWindow:getChildById('labelStrenghtValue')
  dexterityValueLabel       = spelllistWindow:getChildById('labelDexterityValue')
  faithValueLabel       = spelllistWindow:getChildById('labelFaithValue')
  intelligenceValueLabel = spelllistWindow:getChildById('labelIntelligenceValue')
  manaValueLabel        = spelllistWindow:getChildById('labelManaValue')
  premiumValueLabel     = spelllistWindow:getChildById('labelPremiumValue')
  descriptionValueLabel = spelllistWindow:getChildById('labelDescriptionValue')

  groupBoxAny           = spelllistWindow:getChildById('groupBoxAny')
  groupBoxAttack        = spelllistWindow:getChildById('groupBoxAttack')
  groupBoxHealing       = spelllistWindow:getChildById('groupBoxHealing')
  groupBoxSupport       = spelllistWindow:getChildById('groupBoxSupport')

  groupRadioGroup = UIRadioGroup.create()
  groupRadioGroup:addWidget(groupBoxAny)
  groupRadioGroup:addWidget(groupBoxAttack)
  groupRadioGroup:addWidget(groupBoxHealing)
  groupRadioGroup:addWidget(groupBoxSupport)

  groupRadioGroup:selectWidget(groupBoxAny)

  groupRadioGroup.onSelectionChange     = toggleFilter

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

  groupRadioGroup:destroy()
  --premiumRadioGroup:destroy()
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
    if (not(filters.canUse) or (info.intelligence <= localPlayer:getSkillLevel(Skill.Intelligence) and 
								info.faith <= localPlayer:getSkillLevel(Skill.Faith) and 
								info.strenght <= localPlayer:getSkillLevel(Skill.Strenght) and 
								info.dexterity <= localPlayer:getSkillLevel(Skill.Dexterity))) and 
								(filters.groupId == FILTER_GROUP_ANY or info.group[filters.groupId]) and 
								(filters.premium == FILTER_PREMIUM_ANY or (info.premium and filters.premium == FILTER_PREMIUM_YES) or (not(info.premium) and filters.premium == FILTER_PREMIUM_NO)) then
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
  local group       = ''
  local type        = ''
  local cooldown    = ''
  local strenght    = ''
  local dexterity   = ''
  local faith       = ''
  local intelligence = ''
  local mana        = ''
  local premium     = ''

  if SpellInfo[SpelllistProfile][spell] then
    local info = SpellInfo[SpelllistProfile][spell]

    name    = spell
    formula = info.words

    cooldown = (info.exhaustion / 1000) .. 's'
    for groupId, groupName in ipairs(SpellGroups) do
      if info.group[groupId] then
        group    = group .. (group:len() == 0 and '' or ' / ') .. groupName
        cooldown = cooldown .. ' / ' .. (info.group[groupId] / 1000) .. 's'
      end
    end
	

    type    = info.type
	strenght = info.strenght or '-'
	dexterity = info.dexterity or '-'
    faith = info.faith or '-'
    intelligence = info.intelligence or '-'
    mana    = info.mana .. ' / ' .. info.soul
    premium = (info.premium and 'yes' or 'no')
  end

  nameValueLabel:setText(name)
  formulaValueLabel:setText(formula)
  groupValueLabel:setText(group)
  typeValueLabel:setText(type)
  cooldownValueLabel:setText(cooldown)
  strenghtValueLabel:setText(strenght)
  dexterityValueLabel:setText(dexterity)
  faithValueLabel:setText(faith)
  intelligenceValueLabel:setText(intelligence)
  manaValueLabel:setText(mana)
end

function toggle()
  if spelllistButton:isOn() then
    spelllistButton:setOn(false)
    spelllistWindow:hide()
    resetWindow()
  else
    spelllistButton:setOn(true)
    spelllistWindow:show()
    spelllistWindow:raise()
    spelllistWindow:focus()
  end
end

function toggleFilter(widget, selectedWidget)
  if widget == groupRadioGroup then
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
    if id == 'buttonFilterCanUse' then
      filters.canUse = not(filters.canUse)
      widget:setOn(filters.canUse)
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
  filters.canUse = false

  local buttonFilterCanUse = spelllistWindow:getChildById('buttonFilterCanUse')
  buttonFilterCanUse:setOn(filters.canUse)

  groupRadioGroup:selectWidget(groupBoxAny)

  updateSpelllist()
end
