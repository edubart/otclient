skillsWindow = nil
skillsButton = nil
applyButton = nil

healthButton = nil
healthValueLabel = nil
newHealthArrowLabel = nil
newHealthValueLabel = nil

manaButton = nil
manaValueLabel = nil
newManaArrowLabel = nil
newManaValueLabel = nil

blessingsButton = nil
blessingsValueLabel = nil

resetsButton = nil
resetsValueLabel = nil

skillPointsButton = nil
skillPointsValueLabel = nil
newSkillPointsArrowLabel = nil
newSkillPointsValueLabel = nil

capacityButton = nil
capacityValueLabel = nil
newCapacityArrowLabel = nil
newCapacityValueLabel = nil

walkSpeedButton = nil
walkSpeedValueLabel = nil
newWalkSpeedArrowLabel = nil
newWalkSpeedValueLabel = nil

attackSpeedButton = nil
attackSpeedValueLabel = nil
newAttackSpeedArrowLabel = nil
newAttackSpeedValueLabel = nil

magicButton = nil
magicValueLabel = nil
newMagicArrowLabel = nil
newMagicValueLabel = nil
magicPlusButton = nil
magicMinusButton = nil

vitalityButton = nil
vitalityValueLabel = nil
newVitalityArrowLabel = nil
newVitalityValueLabel = nil
vitalityPlusButton = nil
vitalityMinusButton = nil

strenghtButton = nil
strenghtValueLabel = nil
newStrenghtArrowLabel = nil
newStrenghtValueLabel = nil
strenghtPlusButton = nil
strenghtMinusButton = nil

defenceButton = nil
defenceValueLabel = nil
newDefenceArrowLabel = nil
newDefenceValueLabel = nil
defencePlusButton = nil
defenceMinusButton = nil

dexterityButton = nil
dexterityValueLabel = nil
newDexterityArrowLabel = nil
newDexterityValueLabel = nil
dexterityPlusButton = nil
dexterityMinusButton = nil

intelligenceButton = nil
intelligenceValueLabel = nil
newIntelligenceArrowLabel = nil
newIntelligenceValueLabel = nil
intelligencePlusButton = nil
intelligenceMinusButton = nil

faithButton = nil
faithValueLabel = nil
newFaithArrowLabel = nil
newFaithValueLabel = nil
faithPlusButton = nil
faithMinusButton = nil

enduranceButton = nil
enduranceValueLabel = nil
newEnduranceArrowLabel = nil
newEnduranceValueLabel = nil
endurancePlusButton = nil
enduranceMinusButton = nil

criticalChanceButton = nil
criticalChanceValueLabel = nil

criticalDamageButton = nil
criticalValueLabel = nil

wandDamageButton = nil
wandValueLabel = nil

rodDamageButton = nil
rodValueLabel = nil

player = nil

newSkillPoints = nil
newMagic = nil
newVitality = nil
newStrenght = nil
newDefence = nil
newDexterity = nil
newIntelligence = nil
newFaith = nil
newEndurance = nil

function init()

	skillsButton = modules.client_topmenu.addRightGameToggleButton('skillsButton', tr('Skills') .. ' (Ctrl+S)', '/images/topbuttons/skills', toggle)
	
	skillsWindow = g_ui.loadUI('skills', modules.game_interface.getRightPanel())
	
	applyButton = skillsWindow:recursiveGetChildById('applybutton')

	healthButton = skillsWindow:recursiveGetChildById('health')
	healthValueLabel = healthButton:recursiveGetChildById('value')
	newHealthArrowLabel = healthButton:recursiveGetChildById('arrow')
	newHealthValueLabel = healthButton:recursiveGetChildById('newValue')

	manaButton = skillsWindow:recursiveGetChildById('mana')
	manaValueLabel = manaButton:recursiveGetChildById('value')
	newManaArrowLabel = manaButton:recursiveGetChildById('arrow')
	newManaValueLabel = manaButton:recursiveGetChildById('newValue')
	
	blessingsButton = skillsWindow:recursiveGetChildById('blessings')
	blessingsValueLabel = blessingsButton:recursiveGetChildById('value')
	
	resetsButton = skillsWindow:recursiveGetChildById('resets')
	resetsValueLabel = resetsButton:recursiveGetChildById('value')
	
	skillPointsButton = skillsWindow:recursiveGetChildById('skillpoints')
	skillPointsValueLabel = skillPointsButton:recursiveGetChildById('value')
	newSkillPointsArrowLabel = skillPointsButton:recursiveGetChildById('arrow')
	newSkillPointsValueLabel = skillPointsButton:recursiveGetChildById('newValue')

	capacityButton = skillsWindow:recursiveGetChildById('capacity')
	capacityValueLabel = capacityButton:recursiveGetChildById('value')
	newCapacityArrowLabel = capacityButton:recursiveGetChildById('arrow')
	newCapacityValueLabel = capacityButton:recursiveGetChildById('newValue')

	walkSpeedButton = skillsWindow:recursiveGetChildById('walkspeed')
	walkSpeedValueLabel = walkSpeedButton:recursiveGetChildById('value')
	newWalkSpeedArrowLabel = walkSpeedButton:recursiveGetChildById('arrow')
	newWalkSpeedValueLabel = walkSpeedButton:recursiveGetChildById('newValue')

	attackSpeedButton = skillsWindow:recursiveGetChildById('attackspeed')
	attackSpeedValueLabel = attackSpeedButton:recursiveGetChildById('value')
	newAttackSpeedArrowLabel = attackSpeedButton:recursiveGetChildById('arrow')
	newAttackSpeedValueLabel = attackSpeedButton:recursiveGetChildById('newValue')

	magicButton = skillsWindow:recursiveGetChildById('magiclevel')
	magicValueLabel = magicButton:recursiveGetChildById('value')
	newMagicArrowLabel = magicButton:recursiveGetChildById('arrow')
	newMagicValueLabel = magicButton:recursiveGetChildById('newValue')
	magicPlusButton = magicButton:recursiveGetChildById('plusbutton')
	magicMinusButton = magicButton:recursiveGetChildById('minusbutton')

	vitalityButton = skillsWindow:recursiveGetChildById('skillId'..Skill.Vitality)
	vitalityValueLabel = vitalityButton:recursiveGetChildById('value')
	newVitalityArrowLabel = vitalityButton:recursiveGetChildById('arrow')
	newVitalityValueLabel = vitalityButton:recursiveGetChildById('newValue')
	vitalityPlusButton = vitalityButton:recursiveGetChildById('plusbutton')
	vitalityMinusButton = vitalityButton:recursiveGetChildById('minusbutton')

	strenghtButton = skillsWindow:recursiveGetChildById('skillId'..Skill.Strenght)
	strenghtValueLabel = strenghtButton:recursiveGetChildById('value')
	newStrenghtArrowLabel = strenghtButton:recursiveGetChildById('arrow')
	newStrenghtValueLabel = strenghtButton:recursiveGetChildById('newValue')
	strenghtPlusButton = strenghtButton:recursiveGetChildById('plusbutton')
	strenghtMinusButton = strenghtButton:recursiveGetChildById('minusbutton')

	defenceButton = skillsWindow:recursiveGetChildById('skillId'..Skill.Defence)
	defenceValueLabel = defenceButton:recursiveGetChildById('value')
	newDefenceArrowLabel = defenceButton:recursiveGetChildById('arrow')
	newDefenceValueLabel = defenceButton:recursiveGetChildById('newValue')
	defencePlusButton = defenceButton:recursiveGetChildById('plusbutton')
	defenceMinusButton = defenceButton:recursiveGetChildById('minusbutton')

	dexterityButton = skillsWindow:recursiveGetChildById('skillId'..Skill.Dexterity)
	dexterityValueLabel = dexterityButton:recursiveGetChildById('value')
	newDexterityArrowLabel = dexterityButton:recursiveGetChildById('arrow')
	newDexterityValueLabel = dexterityButton:recursiveGetChildById('newValue')
	dexterityPlusButton = dexterityButton:recursiveGetChildById('plusbutton')
	dexterityMinusButton = dexterityButton:recursiveGetChildById('minusbutton')

	intelligenceButton = skillsWindow:recursiveGetChildById('skillId'..Skill.Intelligence)
	intelligenceValueLabel = intelligenceButton:recursiveGetChildById('value')
	newIntelligenceArrowLabel = intelligenceButton:recursiveGetChildById('arrow')
	newIntelligenceValueLabel = intelligenceButton:recursiveGetChildById('newValue')
	intelligencePlusButton = intelligenceButton:recursiveGetChildById('plusbutton')
	intelligenceMinusButton = intelligenceButton:recursiveGetChildById('minusbutton')

	faithButton = skillsWindow:recursiveGetChildById('skillId'..Skill.Faith)
	faithValueLabel = faithButton:recursiveGetChildById('value')
	newFaithArrowLabel = faithButton:recursiveGetChildById('arrow')
	newFaithValueLabel = faithButton:recursiveGetChildById('newValue')
	faithPlusButton = faithButton:recursiveGetChildById('plusbutton')
	faithMinusButton = faithButton:recursiveGetChildById('minusbutton')

	enduranceButton = skillsWindow:recursiveGetChildById('skillId'..Skill.Endurance)
	enduranceValueLabel = enduranceButton:recursiveGetChildById('value')
	newEnduranceArrowLabel = enduranceButton:recursiveGetChildById('arrow')
	newEnduranceValueLabel = enduranceButton:recursiveGetChildById('newValue')
	endurancePlusButton = enduranceButton:recursiveGetChildById('plusbutton')
	enduranceMinusButton = enduranceButton:recursiveGetChildById('minusbutton')
	
	--criticalChanceButton = skillsWindow:recursiveGetChildById('skillId7')
	--criticalChanceValueLabel = criticalChanceButton:recursiveGetChildById('value')
	
	--criticalDamageButton = skillsWindow:recursiveGetChildById('skillId8')
	--criticalDamageValueLabel = criticalDamageButton:recursiveGetChildById('value')
	
	wandDamageButton = skillsWindow:recursiveGetChildById('wanddamage')
	wandDamageValueLabel = wandDamageButton:recursiveGetChildById('value')
	
	rodDamageButton = skillsWindow:recursiveGetChildById('roddamage')
	rodDamageValueLabel = rodDamageButton:recursiveGetChildById('value')
	
	g_keyboard.bindKeyDown('Ctrl+S', toggle)
			
	skillsButton:setOn(true)
	skillsWindow:setup()
	
	connect(LocalPlayer, {
		onExperienceChange = onExperienceChange,
		onLevelChange = onLevelChange,
		onHealthChange = onHealthChange,
		onManaChange = onManaChange,
		onBlessingsChange = onBlessingsChange,
		onResetsChange = onResetsChange,
		onSoulChange = onSoulChange,
		onSkillPointsChange = onSkillPointsChange,
		onFreeCapacityChange = onFreeCapacityChange,
		onTotalCapacityChange = onTotalCapacityChange,
		onAttackSpeedChange = onAttackSpeedChange,
		onStaminaChange = onStaminaChange,
		onRegenerationChange = onRegenerationChange,
		onBaseMagicLevelChange = onBaseMagicLevelChange,
		onMagicLevelChange = onMagicLevelChange,
		onNewBaseMagicLevelChange = onNewBaseMagicLevelChange,
		onBaseSkillChange = onBaseSkillChange,
		onSkillChange = onSkillChange,
		onNewBaseSkillChange = onNewBaseSkillChange,
		onBaseSpeedChange = onBaseSpeedChange,
		onSpeedChange = onSpeedChange,
	})
	
	connect(g_game, {
		onGameEnd = offline
	})
end

function terminate()

	disconnect(LocalPlayer, {
		onExperienceChange = onExperienceChange,
		onLevelChange = onLevelChange,
		onHealthChange = onHealthChange,
		onManaChange = onManaChange,
		onBlessingsChange = onBlessingsChange,
		onResetsChange = onResetsChange,
		onSoulChange = onSoulChange,
		onSkillPointsChange = onSkillPointsChange,
		onFreeCapacityChange = onFreeCapacityChange,
		onTotalCapacityChange = onTotalCapacityChange,
		onSpeedChange = onSpeedChange,
		onBaseSpeedChange = onBaseSpeedChange,
		onAttackSpeedChange = onAttackSpeedChange,
		onStaminaChange = onStaminaChange,
		onRegenerationChange = onRegenerationChange,
		onBaseMagicLevelChange = onBaseMagicLevelChange,
		onMagicLevelChange = onMagicLevelChange,
		onNewBaseMagicLevelChange = onNewBaseMagicLevelChange,
		onBaseSkillChange = onBaseSkillChange,
		onSkillChange = onSkillChange,
		onNewBaseSkillChange = onNewBaseSkillChange,
	})
	
	disconnect(g_game, {
		onGameStart = resetChanges,
		onGameEnd = offline
	})

	g_keyboard.unbindKeyDown('Ctrl+S')
	
	healthButton:destroy()
	healthValueLabel = nil
	newHealthArrowLabel = nil
	newHealthValueLabel = nil

	manaButton:destroy()
	manaValueLabel = nil
	newManaArrowLabel = nil
	newManaValueLabel = nil
	
	blessingsButton:destroy()
	blessingsValueLabel = nil
	
	resetsButton:destroy()
	resetsValueLabel = nil

	skillPointsButton:destroy()
	skillPointsValueLabel = nil
	newSkillPointsArrowLabel = nil
	newSkillPointsValueLabel = nil

	capacityButton:destroy()
	capacityValueLabel = nil
	newCapacityArrowLabel = nil
	newCapacityValueLabel = nil

	walkSpeedButton:destroy()
	walkSpeedValueLabel = nil
	newWalkSpeedArrowLabel = nil
	newWalkSpeedValueLabel = nil

	attackSpeedButton:destroy()
	attackSpeedValueLabel = nil
	newAttackSpeedArrowLabel = nil
	newAttackSpeedValueLabel = nil

	magicButton:destroy()
	magicValueLabel = nil
	newMagicArrowLabel = nil
	newMagicValueLabel = nil

	vitalityButton:destroy()
	vitalityValueLabel = nil
	newVitalityArrowLabel = nil
	newVitalityValueLabel = nil
	
	strenghtButton:destroy()
	strenghtValueLabel = nil
	newStrenghtArrowLabel = nil
	newStrenghtValueLabel = nil

	defenceButton:destroy()
	defenceValueLabel = nil
	newDefenceArrowLabel = nil
	newDefenceValueLabel = nil
	
	dexterityButton:destroy()
	dexterityValueLabel = nil
	newDexterityArrowLabel = nil
	newDexterityValueLabel = nil

	intelligenceButton:destroy()
	intelligenceValueLabel = nil
	newIntelligenceArrowLabel = nil
	newIntelligenceValueLabel = nil

	faithButton:destroy()
	faithValueLabel = nil
	newFaithArrowLabel = nil
	newFaithValueLabel = nil

	enduranceButton:destroy()
	enduranceValueLabel = nil
	newEnduranceArrowLabel = nil
	newEnduranceValueLabel = nil
	
	--criticalChanceButton:destroy()
	--criticalChanceValueLabel = nil
	
	--criticalDamageButton:destroy()
	--criticalDamageValueLabel = nil
	
	wandDamageButton:destroy()
	wandDamageValueLabel = nil
	
	rodDamageButton:destroy()
	rodDamageValueLabel = nil

	player = nil

	pointsUsed = nil
	newMagic = nil
	newVitality = nil
	newStrenght = nil
	newDefence = nil
	newDexterity = nil
	newIntelligence = nil
	newFaith = nil
	newEndurance = nil

	applyButton:destroy()
	skillsWindow:destroy()
	skillsButton:destroy()
end

function setSkillsTooltips()	
	local skillMagic = skillsWindow:recursiveGetChildById('magiclevel')
	skillMagic:setTooltip(SkillDescription.Magic)

	local skillVitality = skillsWindow:recursiveGetChildById('skillId'..Skill.Vitality)
	skillVitality:setTooltip(SkillDescription.Vitality)

	local skillStrenght = skillsWindow:recursiveGetChildById('skillId'..Skill.Strenght)
	skillStrenght:setTooltip(SkillDescription.Strenght)

	local skillFaith = skillsWindow:recursiveGetChildById('skillId'..Skill.Faith)
	skillFaith:setTooltip(SkillDescription.Faith)

	local skillIntelligence = skillsWindow:recursiveGetChildById('skillId'..Skill.Intelligence)
	skillIntelligence:setTooltip(SkillDescription.Intelligence)

	local skillDexterity = skillsWindow:recursiveGetChildById('skillId'..Skill.Dexterity)
	skillDexterity:setTooltip(SkillDescription.Dexterity)

	local skilDefence = skillsWindow:recursiveGetChildById('skillId'..Skill.Defence)
	skilDefence:setTooltip(SkillDescription.Defence)

	local skillEndurance = skillsWindow:recursiveGetChildById('skillId'..Skill.Endurance)
	skillEndurance:setTooltip(SkillDescription.Endurance)
	
	local skillWand = skillsWindow:recursiveGetChildById('wanddamage')
	skillWand:setTooltip(SkillDescription.Wand)
	
	local skillRod = skillsWindow:recursiveGetChildById('roddamage')
	skillRod:setTooltip(SkillDescription.Rod)
	
end

function onClickApply()
	
	g_game.checkVersionToPlay(clientVersionToPlay)

	if not player then 
		toggle() 
		toggle()
		return 
	end

	if newMagic < 0 then newMagic = player:getBaseMagicLevel() toggle() toggle() return end
	if newVitality < 0 then newVitality = player:getBaseSkillLevel(Skill.Vitality) toggle() toggle() return end
	if newStrenght < 0 then newStrenght = player:getBaseSkillLevel(Skill.Strenght) toggle() toggle() return end
	if newDefence < 0 then newDefence = player:getBaseSkillLevel(Skill.Defence) toggle() toggle() return end
	if newDexterity < 0 then newDexterity = player:getBaseSkillLevel(Skill.Dexterity) toggle() toggle() return end
	if newIntelligence < 0 then newIntelligence = player:getBaseSkillLevel(Skill.Intelligence) toggle() toggle() return end
	if newFaith < 0 then newFaith = player:getBaseSkillLevel(Skill.Faith) toggle() toggle() return end
	if newEndurance < 0 then newEndurance = player:getBaseSkillLevel(Skill.Endurance) toggle() toggle() return end
	
	local playerLevel = player:getLevel()
	
	local maxMagic 		= 10
	local maxVitality 	= math.ceil( math.max( playerLevel * 0.8 + 3.2 + 8, playerLevel))
	local maxStrenght 	= maxVitality
	local maxIntelligence = maxVitality
	local maxFaith 		= maxVitality
	local maxDexterity 	= maxVitality
	local maxDefence 	= maxVitality
	local maxEndurance 	= maxVitality
	
	if (playerLevel >= 9) then
		maxMagic 		= math.ceil( math.max( playerLevel * math.max( 2.5 - (0.02 * (playerLevel - 8)), 1.0 ) * 0.5, playerLevel * 1.0 ) )
		maxVitality 	= math.ceil( math.max(playerLevel * 1.6 - 3.2 + 8, playerLevel))
		maxStrenght 	= maxVitality
		maxIntelligence = maxVitality
		maxFaith 		= maxVitality
		maxDexterity 	= maxVitality
		maxDefence 		= maxVitality
		maxEndurance 	= maxVitality
	end
	
	if (playerLevel >= 33) then
		maxFaith = faithInfo['max']
	end

	if (playerLevel >= 41) then
        maxMagic 		= math.ceil( math.max( playerLevel * math.max( 2.5 - (0.02 * (playerLevel - 8)), 1.0 ) * 0.5, playerLevel * 1.0 ) )
		maxVitality 	= math.ceil( math.max(playerLevel * 1.2 + 12.8 + 8, playerLevel))
		maxStrenght 	= maxVitality
		maxIntelligence = maxVitality
		maxDexterity 	= maxVitality
		maxDefence 		= maxVitality
		maxEndurance 	= maxVitality
	end

	if (playerLevel >= 61) then
        maxMagic 		= math.ceil( 60 + (playerLevel - 60) / 2 )
		maxVitality 	= math.ceil( math.max(playerLevel * 0.8 + 36.8 + 8, playerLevel))
		maxStrenght 	= maxVitality
		maxIntelligence = maxVitality
		maxDexterity 	= maxVitality
		maxDefence 		= maxVitality
		maxEndurance 	= maxVitality
	end
	
	if (playerLevel >= 80) then
		maxIntelligence = intelligenceInfo['max']
	end

	if (playerLevel > 81) then
        maxMagic 		= math.ceil( 70 + (playerLevel - 80) / 2 )
		maxVitality 	= math.ceil(playerLevel * 0.4 + 68.8 + 8)
		maxStrenght 	= maxVitality
		maxDexterity 	= maxVitality
		maxDefence 		= maxVitality
		maxEndurance 	= maxVitality
	end
	
	if maxMagic > magicInfo['max'] then maxMagic = magicInfo['max'] end
	if maxVitality > vitalityInfo['max'] then maxVitality = vitalityInfo['max'] end
	if maxStrenght > strenghtInfo['max'] then maxStrenght = strenghtInfo['max'] end
	if maxIntelligence > intelligenceInfo['max'] then maxIntelligence = intelligenceInfo['max'] end
	if maxFaith > faithInfo['max'] then maxFaith = faithInfo['max'] end
	if maxDexterity > dexterityInfo['max'] then maxDexterity = dexterityInfo['max'] end
	if maxDefence > defenceInfo['max'] then maxDefence = defenceInfo['max'] end
	if maxEndurance > enduranceInfo['max'] then maxEndurance = enduranceInfo['max'] end	   
	   
	local checks = true

	if (newMagic > maxMagic) then modules.game_textmessage.displayBroadcastMessage("The maximum magic at your level is "..maxMagic..".") checks = false end
	if (newVitality > maxVitality) then dules.game_textmessage.displayBroadcastMessage("The maximum vitality at your level is "..maxVitality..".") checks = false end 
	if (newStrenght > maxStrenght) then modules.game_textmessage.displayBroadcastMessage("The maximum strenght at your level is "..maxStrenght..".") checks = false end
	if (newDefence > maxDefence) then modules.game_textmessage.displayBroadcastMessage("The maximum defence at your level is "..maxDefence..".") checks = false end
	if (newIntelligence > maxIntelligence) then modules.game_textmessage.displayBroadcastMessage("The maximum intelligence at your level is "..maxIntelligence..".") checks = false end
	if (newFaith > maxFaith) then modules.game_textmessage.displayBroadcastMessage("The maximum faith at your level is "..maxFaith..".") checks = false end
	if (newDexterity > maxDexterity) then modules.game_textmessage.displayBroadcastMessage("The maximum dexterity at your level is "..maxDexterity..".") checks = false end
	if (newEndurance > maxEndurance) then modules.game_textmessage.displayBroadcastMessage("The maximum endurance at your level is "..maxEndurance..".") checks = false end
	if (checks == false) then return false end
	
	g_game.applyNewSkills(newMagic, newVitality, newStrenght, newDefence, newDexterity, newIntelligence, newFaith, newEndurance)
	
	setExtraLabelsInvisible()

	resetChanges()

	toggle() 
	toggle()
end

function setSkillPointsValueAndVisibility(arrowLabel, valueLabel, value, currentValue)
	local boolean = value <= currentValue
	arrowLabel:setVisible(boolean)
	valueLabel:setText(value)
	valueLabel:setWidth(valueLabel:getTextSize().width)
	valueLabel:setVisible(boolean)
end

function setValueAndVisibility(arrowLabel, valueLabel, value, currentValue)
	local boolean = value > currentValue
	arrowLabel:setVisible(boolean)
	valueLabel:setText(value)
	valueLabel:setWidth(valueLabel:getTextSize().width)
	valueLabel:setVisible(boolean)
end

function setNewValueAndVisibility(arrowLabel, newValueLabel, value, visibility)
	arrowLabel:setVisible(visibility)
	valueLabel:setText(value)
	valueLabel:setWidth(valueLabel:getTextSize().width)
	valueLabel:setVisible(visibility)
end

function setApplyButtonVisibility()
	if newSkillPoints < player:getSkillPoints() then 
		applyButton:setVisible(true) 
	else 
		applyButton:setVisible(false) 
	end
end

function setPlusButtonVisibility(button, cost)
	if player:getSkillPoints() >= cost then
		button:setVisible(true)
	else
		button:setVisible(false)
	end
end

function setMinusButtonVisibility(button, skillValue, newSkillValue)
	if newSkillValue > skillValue then
		button:setVisible(true)
	else
		button:setVisible(false)
	end
end

function getNewAttackSpeedInPercent()
	local bonusAttackSpeed = dexterityInfo["attackSpeed"] * (math.max(newDexterity, dexterityInfo["initial"]) - dexterityInfo["initial"])
	return ((initial.attackSpeed + bonusAttackSpeed) / initial.attackSpeed) * 100
end

function getNewMaxMana()
	local initialMana = initial.mana
	local levelsMana = levelGain.mana * (player:getLevel() - 1)
	local magicMana = magicInfo['mana'] * math.max(newMagic, player:getBaseMagicLevel())
	local intelligenceMana = intelligenceInfo['mana'] * (math.max(newIntelligence, player:getBaseSkillLevel(Skill.Intelligence)) - intelligenceInfo['initial'])
	local faithMana = faithInfo['mana'] * (math.max(newFaith, player:getBaseSkillLevel(Skill.Faith)) - faithInfo['initial'])
	return initialMana + levelsMana + magicMana + intelligenceMana + faithMana
end

function getNewMaxHealth()
	local initialHealth = initial.health
	local levelsHealth = levelGain.health * (player:getLevel() - 1)
	local vitalityHealth = vitalityInfo['health'] * (math.max(newVitality, player:getBaseSkillLevel(Skill.Vitality))  - vitalityInfo['initial'])
	local enduranceHealth = enduranceInfo['health'] * (math.max(newEndurance, player:getBaseSkillLevel(Skill.Endurance))  - enduranceInfo['initial'])
	return initialHealth + levelsHealth + vitalityHealth + enduranceHealth
end

function getNewWalkSpeed()
	local baseSpeed = player:getBaseSpeed()
	print(baseSpeed)
	local dexterityWalkSpeed = dexterityInfo['walkSpeed'] * (math.max(newDexterity, player:getBaseSkillLevel(Skill.Dexterity)) - math.max(dexterityInfo['initial'], player:getBaseSkillLevel(Skill.Dexterity)))
	print(dexterityWalkSpeed)
	return baseSpeed + dexterityWalkSpeed
end

function getNewAttackSpeed()
	local initialAttackSpeed = 2000 -- 2 seconds
	local dexterityAttackSpeed = dexterityInfo['attackSpeed'] * 10 * (math.max(newDexterity, player:getBaseSkillLevel(Skill.Dexterity))  - dexterityInfo['initial'])
	return initialAttackSpeed - dexterityAttackSpeed
end

function getNewMaxCapacity()
	local initialCapacity = 365 * 100.0 -- starts level 1 with 365 cap
	local levelsCapacity = levelGain.capacity * (player:getLevel() - 1)
	local strenghtCapacity = strenghtInfo['cap'] * 100.0 * (math.max(newStrenght, player:getBaseSkillLevel(Skill.Strenght)) - strenghtInfo['initial']) 
	local enduranceCapacity = enduranceInfo['cap'] * 100.0 * (math.max(newEndurance, player:getBaseSkillLevel(Skill.Endurance)) - enduranceInfo['initial'])
	return (initialCapacity + levelsCapacity + strenghtCapacity + enduranceCapacity) / 100.0
end

--[[function getNewFreeCapacity()
	local freeCapacity = player:getFreeCapacity()
	local levelsCapacity = levelGain.capacity * (player:getLevel() - 1)
	local strenghtCapacity = strenghtInfo['cap'] * 100.0 * (math.max(newStrenght, player:getBaseSkillLevel(Skill.Strenght)) - strenghtInfo['initial']) 
	local enduranceCapacity = enduranceInfo['cap'] * 100.0 * (math.max(newEndurance, player:getBaseSkillLevel(Skill.Endurance)) - enduranceInfo['initial'])
	return (freeCapacity + strenghtCapacity + enduranceCapacity) / 100.0
end]]

function onClickAddMagic()

	if newSkillPoints - magicInfo['cost'] >= 0 then
			
		applyButton:setVisible(true)
		newSkillPoints = newSkillPoints - magicInfo['cost']
		setSkillPointsValueAndVisibility(newSkillPointsArrowLabel, newSkillPointsValueLabel, newSkillPoints, player:getSkillPoints())

		local currentMagic = player:getBaseMagicLevel()
		if newMagic < currentMagic then newMagic = currentMagic end

		newMagic = newMagic + 1
		setValueAndVisibility(newMagicArrowLabel, newMagicValueLabel, newMagic, player:getBaseMagicLevel())
		magicMinusButton:setVisible(true)
		
		setValueAndVisibility(newManaArrowLabel, newManaValueLabel, getNewMaxMana(), player:getMaxMana())
	end
	
	setApplyButtonVisibility()
	setPlusButtonVisibility(magicPlusButton, magicInfo['cost'])
end

function onClickAddVitality()

	if newSkillPoints - vitalityInfo['cost'] >= 0 then
			
		applyButton:setVisible(true)
		newSkillPoints = newSkillPoints - vitalityInfo['cost']
		setSkillPointsValueAndVisibility(newSkillPointsArrowLabel, newSkillPointsValueLabel, newSkillPoints, player:getSkillPoints())

		local currentVitality = player:getBaseSkillLevel(Skill.Vitality)
		if newVitality < currentVitality then newVitality = currentVitality end

		newVitality = newVitality + 1
		setValueAndVisibility(newVitalityArrowLabel, newVitalityValueLabel, newVitality, player:getBaseSkillLevel(Skill.Vitality))
		vitalityMinusButton:setVisible(true)
			
		setValueAndVisibility(newHealthArrowLabel, newHealthValueLabel, getNewMaxHealth(), player:getMaxHealth())
	end
	
	setApplyButtonVisibility()
	setPlusButtonVisibility(vitalityPlusButton, vitalityInfo['cost'])
end

function onClickAddStrenght()

	if newSkillPoints - strenghtInfo['cost'] >= 0 then
			
		applyButton:setVisible(true)
		newSkillPoints = newSkillPoints - strenghtInfo['cost']
		setSkillPointsValueAndVisibility(newSkillPointsArrowLabel, newSkillPointsValueLabel, newSkillPoints, player:getSkillPoints())

		local currentStrenght = player:getBaseSkillLevel(Skill.Strenght)
		if newStrenght < currentStrenght then newStrenght = currentStrenght end

		newStrenght = newStrenght + 1
		setValueAndVisibility(newStrenghtArrowLabel, newStrenghtValueLabel, newStrenght, player:getBaseSkillLevel(Skill.Strenght))
		strenghtMinusButton:setVisible(true)
	end
	
	setValueAndVisibility(newCapacityArrowLabel, newCapacityValueLabel, getNewMaxCapacity(), player:getTotalCapacity())
	setApplyButtonVisibility()
	setPlusButtonVisibility(strenghtPlusButton, strenghtInfo['cost'])
end

function onClickAddDefence()

	if newSkillPoints - defenceInfo['cost'] >= 0 then
			
		applyButton:setVisible(true)
		newSkillPoints = newSkillPoints - defenceInfo['cost']
		setSkillPointsValueAndVisibility(newSkillPointsArrowLabel, newSkillPointsValueLabel, newSkillPoints, player:getSkillPoints())

		local currentDefence = player:getBaseSkillLevel(Skill.Defence)
		if newDefence < currentDefence then newDefence = currentDefence end

		newDefence = newDefence + 1
		setValueAndVisibility(newDefenceArrowLabel, newDefenceValueLabel, newDefence, player:getBaseSkillLevel(Skill.Defence))
		defenceMinusButton:setVisible(true)
	end
	
	setApplyButtonVisibility()
	setPlusButtonVisibility(defencePlusButton, defenceInfo['cost'])
end

function onClickAddDexterity()

	if newSkillPoints - dexterityInfo['cost'] >= 0 then
			
		applyButton:setVisible(true)
		newSkillPoints = newSkillPoints - dexterityInfo['cost']
		setSkillPointsValueAndVisibility(newSkillPointsArrowLabel, newSkillPointsValueLabel, newSkillPoints, player:getSkillPoints())

		local currentDexterity = player:getBaseSkillLevel(Skill.Dexterity)
		if newDexterity < currentDexterity then newDexterity = currentDexterity end

		newDexterity = newDexterity + 1
		setValueAndVisibility(newDexterityArrowLabel, newDexterityValueLabel, newDexterity, player:getBaseSkillLevel(Skill.Dexterity))
		dexterityMinusButton:setVisible(true)

		newAttackSpeedArrowLabel:setVisible(true)
		newAttackSpeedValueLabel:setText(getNewAttackSpeedInPercent())
		newAttackSpeedValueLabel:setWidth(newAttackSpeedValueLabel:getTextSize().width)
		newAttackSpeedValueLabel:setVisible(true)

		newWalkSpeedArrowLabel:setVisible(true)
		newWalkSpeedValueLabel:setText(getNewWalkSpeed())
		newWalkSpeedValueLabel:setWidth(newWalkSpeedValueLabel:getTextSize().width)
		newWalkSpeedValueLabel:setVisible(true)
	end
	
	setApplyButtonVisibility()
	setPlusButtonVisibility(dexterityPlusButton, dexterityInfo['cost'])
end

function onClickAddIntelligence()

	if newSkillPoints - intelligenceInfo['cost'] >= 0 then
			
		applyButton:setVisible(true)
		newSkillPoints = newSkillPoints - intelligenceInfo['cost']
		setSkillPointsValueAndVisibility(newSkillPointsArrowLabel, newSkillPointsValueLabel, newSkillPoints, player:getSkillPoints())

		local currentIntelligence = player:getBaseSkillLevel(Skill.Intelligence)
		if newIntelligence < currentIntelligence then newIntelligence = currentIntelligence end

		newIntelligence = newIntelligence + 1
		setValueAndVisibility(newIntelligenceArrowLabel, newIntelligenceValueLabel, newIntelligence, player:getBaseSkillLevel(Skill.Intelligence))
		intelligenceMinusButton:setVisible(true)

		setValueAndVisibility(newManaArrowLabel, newManaValueLabel, getNewMaxMana(), player:getMaxMana())
	end
	
	setApplyButtonVisibility()
	setPlusButtonVisibility(intelligencePlusButton, intelligenceInfo['cost'])
end

function onClickAddFaith()

	if newSkillPoints - faithInfo['cost'] >= 0 then
			
		applyButton:setVisible(true)
		newSkillPoints = newSkillPoints - faithInfo['cost']
		setSkillPointsValueAndVisibility(newSkillPointsArrowLabel, newSkillPointsValueLabel, newSkillPoints, player:getSkillPoints())

		local currentFaith = player:getBaseSkillLevel(Skill.Faith)
		if newFaith < currentFaith then newFaith = currentFaith end

		newFaith = newFaith + 1
		setValueAndVisibility(newFaithArrowLabel, newFaithValueLabel, newFaith, player:getBaseSkillLevel(Skill.Faith))
		faithMinusButton:setVisible(true)

		setValueAndVisibility(newManaArrowLabel, newManaValueLabel, getNewMaxMana(), player:getMaxMana())
	end
	
	setApplyButtonVisibility()
	setPlusButtonVisibility(faithPlusButton, faithInfo['cost'])
end

function onClickAddEndurance()

	if newSkillPoints - enduranceInfo['cost'] >= 0 then
			
		applyButton:setVisible(true)
		newSkillPoints = newSkillPoints - enduranceInfo['cost']
		setSkillPointsValueAndVisibility(newSkillPointsArrowLabel, newSkillPointsValueLabel, newSkillPoints, player:getSkillPoints())

		local currentEndurance = player:getBaseSkillLevel(Skill.Endurance)
		if newEndurance < currentEndurance then newEndurance = currentEndurance end

		newEndurance = newEndurance + 1
		setValueAndVisibility(newEnduranceArrowLabel, newEnduranceValueLabel, newEndurance, player:getBaseSkillLevel(Skill.Endurance))
		enduranceMinusButton:setVisible(true)

		setValueAndVisibility(newCapacityArrowLabel, newCapacityValueLabel, getNewMaxCapacity(), player:getTotalCapacity())
		setValueAndVisibility(newHealthArrowLabel, newHealthValueLabel, getNewMaxHealth(), player:getMaxHealth())
	end
	
	setApplyButtonVisibility()
	setPlusButtonVisibility(endurancePlusButton, enduranceInfo['cost'])
end

function setNewSkillPointsAndApplyButtonVisibility()
	if newSkillPoints >= player:getSkillPoints() then
		applyButton:setVisible(false)
		newSkillPointsArrowLabel:setVisible(false)
		newSkillPointsValueLabel:setVisible(false)
	else
		applyButton:setVisible(true)
		newSkillPointsArrowLabel:setVisible(true)
		newSkillPointsValueLabel:setVisible(true)
	end
end

function setNewManaValueAndVisibility()
	local newMaxMana = getNewMaxMana()
	newManaValueLabel:setText(newMaxMana)
	newManaValueLabel:setWidth(newManaValueLabel:getTextSize().width)
	
	if newMaxMana <= player:getMaxMana() then
		newManaArrowLabel:setVisible(false)
		newManaValueLabel:setVisible(false)
	else
		newManaArrowLabel:setVisible(true)
		newManaValueLabel:setVisible(true)
	end
end

function setNewSkillPointsValueOnRemove(cost)
	newSkillPoints = newSkillPoints + cost
	newSkillPointsValueLabel:setText(newSkillPoints)
	newSkillPointsValueLabel:setWidth(newSkillPointsValueLabel:getTextSize().width)
end

function setNewSkillPointsValueOnAdd(cost)
	newSkillPoints = newSkillPoints - cost
	newSkillPointsValueLabel:setText(newSkillPoints)
	newSkillPointsValueLabel:setWidth(newSkillPointsValueLabel:getTextSize().width)
end

function onClickRemoveMagic()

	setNewSkillPointsValueOnRemove(magicInfo['cost'])
	setNewSkillPointsAndApplyButtonVisibility()

	newMagic = newMagic - 1
	newMagicValueLabel:setText(newMagic)
	newMagicValueLabel:setWidth(newMagicValueLabel:getTextSize().width)
	
	if newMagic <= player:getBaseMagicLevel() then
		newMagicArrowLabel:setVisible(false)
		newMagicValueLabel:setVisible(false)
		magicMinusButton:setVisible(false)
	else
		newMagicArrowLabel:setVisible(true)
		newMagicValueLabel:setText(newMagic)
		newMagicValueLabel:setWidth(newMagicValueLabel:getTextSize().width)
		newMagicValueLabel:setVisible(true)
		magicMinusButton:setVisible(true)
	end
	
	setNewManaValueAndVisibility()
end

function onClickRemoveVitality()

	setNewSkillPointsValueOnRemove(vitalityInfo['cost'])		
	setNewSkillPointsAndApplyButtonVisibility()

	newVitality = newVitality - 1
	newVitalityValueLabel:setText(newVitality)
	newVitalityValueLabel:setWidth(newVitalityValueLabel:getTextSize().width)
	
	if newVitality <= player:getBaseSkillLevel(Skill.Vitality) then
		newVitalityArrowLabel:setVisible(false)
		newVitalityValueLabel:setVisible(false)
		vitalityMinusButton:setVisible(false)
	else
		newVitalityArrowLabel:setVisible(true)
		newVitalityValueLabel:setText(newVitality)
		newVitalityValueLabel:setWidth(newVitalityValueLabel:getTextSize().width)
		newVitalityValueLabel:setVisible(true)
		vitalityMinusButton:setVisible(true)
	end
	
	setApplyButtonVisibility()
	setValueAndVisibility(newHealthArrowLabel, newHealthValueLabel, getNewMaxHealth(), player:getMaxHealth())
	setMinusButtonVisibility(vitalityMinusButton, player:getBaseSkillLevel(Skill.Vitality), newVitality)
end

function onClickRemoveStrenght()

	setNewSkillPointsValueOnRemove(strenghtInfo['cost'])		
	setNewSkillPointsAndApplyButtonVisibility()

	newStrenght = newStrenght - 1
	newStrenghtValueLabel:setText(newStrenght)
	newStrenghtValueLabel:setWidth(newStrenghtValueLabel:getTextSize().width)
	
	if newStrenght <= player:getBaseSkillLevel(Skill.Strenght) then
		newStrenghtArrowLabel:setVisible(false)
		newStrenghtValueLabel:setVisible(false)
		strenghtMinusButton:setVisible(false)
	else
		newStrenghtArrowLabel:setVisible(true)
		newStrenghtValueLabel:setText(newStrenght)
		newStrenghtValueLabel:setWidth(newStrenghtValueLabel:getTextSize().width)
		newStrenghtValueLabel:setVisible(true)
		strenghtMinusButton:setVisible(true)
	end

	local newMaxCapacity = getNewMaxCapacity()
	newCapacityValueLabel:setText(newMaxCapacity / 100.0)
	newCapacityValueLabel:setWidth(newCapacityValueLabel:getTextSize().width)
	
	if newMaxCapacity <= player:getTotalCapacity() then
		newCapacityArrowLabel:setVisible(false)
		newCapacityValueLabel:setVisible(false)
	else
		newCapacityArrowLabel:setVisible(true)
		newCapacityValueLabel:setVisible(true)
	end
	
	setValueAndVisibility(newCapacityArrowLabel, newCapacityValueLabel, getNewMaxCapacity(), player:getTotalCapacity())
	setMinusButtonVisibility(strenghtMinusButton, player:getBaseSkillLevel(Skill.Strenght), newStrenght)
end

function onClickRemoveDefence()

	setNewSkillPointsValueOnRemove(defenceInfo['cost'])		
	setNewSkillPointsAndApplyButtonVisibility()

	newDefence = newDefence - 1
	newDefenceValueLabel:setText(newDefence)
	newDefenceValueLabel:setWidth(newDefenceValueLabel:getTextSize().width)
	
	if newDefence <= player:getBaseSkillLevel(Skill.Defence) then
		newDefenceArrowLabel:setVisible(false)
		newDefenceValueLabel:setVisible(false)
		defenceMinusButton:setVisible(false)
	else
		newDefenceArrowLabel:setVisible(true)
		newDefenceValueLabel:setText(newDefence)
		newDefenceValueLabel:setWidth(newDefenceValueLabel:getTextSize().width)
		newDefenceValueLabel:setVisible(true)
		defenceMinusButton:setVisible(true)
	end
	
	setMinusButtonVisibility(defenceMinusButton, player:getBaseSkillLevel(Skill.Defence), newDefence)
end

function onClickRemoveDexterity()

	setNewSkillPointsValueOnRemove(dexterityInfo['cost'])		
	setNewSkillPointsAndApplyButtonVisibility()

	newDexterity = newDexterity - 1
	newDexterityValueLabel:setText(newDexterity)
	newDexterityValueLabel:setWidth(newDexterityValueLabel:getTextSize().width)
	
	if newDexterity <= player:getBaseSkillLevel(Skill.Dexterity) then
		newDexterityArrowLabel:setVisible(false)
		newDexterityValueLabel:setVisible(false)
		newAttackSpeedArrowLabel:setVisible(false)
		newAttackSpeedValueLabel:setVisible(false)
		newWalkSpeedArrowLabel:setVisible(false)
		newWalkSpeedValueLabel:setVisible(false)
		dexterityMinusButton:setVisible(false)
	else
		newDexterityArrowLabel:setVisible(true)
		newDexterityValueLabel:setText(newDexterity)
		newDexterityValueLabel:setWidth(newDexterityValueLabel:getTextSize().width)
		newDexterityValueLabel:setVisible(true)
		newAttackSpeedArrowLabel:setVisible(true)
		newAttackSpeedValueLabel:setVisible(true)
		newAttackSpeedValueLabel:setText(getNewAttackSpeedInPercent())
		newAttackSpeedValueLabel:setWidth(newAttackSpeedValueLabel:getTextSize().width)
		newWalkSpeedArrowLabel:setVisible(true)
		newWalkSpeedValueLabel:setVisible(true)
		newWalkSpeedValueLabel:setText(getNewWalkSpeed())
		newWalkSpeedValueLabel:setWidth(newWalkSpeedValueLabel:getTextSize().width)
		dexterityMinusButton:setVisible(true)
	end

	--setValueAndVisibility(newAttackSpeedArrowLabel, newAttackSpeedValueLabel, getNewAttackSpeed(), true)
	--setValueAndVisibility(newWalkSpeedArrowLabel, newWalkSpeedValueLabel, getNewWalkSpeed(), true)
end

function onClickRemoveIntelligence()

	setNewSkillPointsValueOnRemove(intelligenceInfo['cost'])		
	setNewSkillPointsAndApplyButtonVisibility()

	newIntelligence = newIntelligence - 1
	newIntelligenceValueLabel:setText(newIntelligence)
	newIntelligenceValueLabel:setWidth(newIntelligenceValueLabel:getTextSize().width)
	
	if newIntelligence <= player:getBaseSkillLevel(Skill.Intelligence) then
		newIntelligenceArrowLabel:setVisible(false)
		newIntelligenceValueLabel:setVisible(false)
		intelligenceMinusButton:setVisible(false)
	else
		newIntelligenceArrowLabel:setVisible(true)
		newIntelligenceValueLabel:setText(newIntelligence)
		newIntelligenceValueLabel:setWidth(newIntelligenceValueLabel:getTextSize().width)
		newIntelligenceValueLabel:setVisible(true)
		intelligenceMinusButton:setVisible(true)
	end

	setValueAndVisibility(newManaArrowLabel, newManaValueLabel, getNewMaxMana(), player:getMaxMana())
end

function onClickRemoveFaith()

	setNewSkillPointsValueOnRemove(faithInfo['cost'])		
	setNewSkillPointsAndApplyButtonVisibility()

	newFaith = newFaith - 1
	newFaithValueLabel:setText(newFaith)
	newFaithValueLabel:setWidth(newFaithValueLabel:getTextSize().width)
	
	if newFaith <= player:getBaseSkillLevel(Skill.Faith) then
		newFaithArrowLabel:setVisible(false)
		newFaithValueLabel:setVisible(false)
		faithMinusButton:setVisible(false)
	else
		newFaithArrowLabel:setVisible(true)
		newFaithValueLabel:setText(newFaith)
		newFaithValueLabel:setWidth(newFaithValueLabel:getTextSize().width)
		newFaithValueLabel:setVisible(true)
		faithMinusButton:setVisible(true)
	end

	setValueAndVisibility(newManaArrowLabel, newManaValueLabel, getNewMaxMana(), player:getMaxMana())
end

function onClickRemoveEndurance()

	setNewSkillPointsValueOnRemove(enduranceInfo['cost'])		
	setNewSkillPointsAndApplyButtonVisibility()

	newEndurance = newEndurance - 1
	newEnduranceValueLabel:setText(newEndurance)
	newEnduranceValueLabel:setWidth(newEnduranceValueLabel:getTextSize().width)
	
	if newEndurance <= player:getBaseSkillLevel(Skill.Endurance) then
		newEnduranceArrowLabel:setVisible(false)
		newEnduranceValueLabel:setVisible(false)
		enduranceMinusButton:setVisible(false)
	else
		newEnduranceArrowLabel:setVisible(true)
		newEnduranceValueLabel:setText(newEndurance)
		newEnduranceValueLabel:setWidth(newEnduranceValueLabel:getTextSize().width)
		newEnduranceValueLabel:setVisible(true)
		enduranceMinusButton:setVisible(true)
	end
	
	local newMaxCapacity = getNewMaxCapacity()
	newCapacityValueLabel:setText(newMaxCapacity)
	newCapacityValueLabel:setWidth(newCapacityValueLabel:getTextSize().width)
	
	if newMaxCapacity <= player:getTotalCapacity() then
		newCapacityArrowLabel:setVisible(false)
		newCapacityValueLabel:setVisible(false)
	else
		newCapacityArrowLabel:setVisible(true)
		newCapacityValueLabel:setVisible(true)
	end
	
	setApplyButtonVisibility()
	setValueAndVisibility(newHealthArrowLabel, newHealthValueLabel, getNewMaxHealth(), player:getMaxHealth())
	setMinusButtonVisibility(enduranceMinusButton, player:getBaseSkillLevel(Skill.Endurance), newEndurance)
end	

function expForLevel(level)
  return math.floor((50*level*level*level)/3 - 100*level*level + (850*level)/3 - 200)
end

function expToAdvance(currentLevel, currentExp)
  return expForLevel(currentLevel+1) - currentExp
end

function resetSkillColor(id)
	local skill = skillsWindow:recursiveGetChildById(id)
	local widget = skill:getChildById('value')
	widget:setColor('#bbbbbb')
end

function toggleSkill(id, state)
	local skill = skillsWindow:recursiveGetChildById(id)
	skill:setVisible(state)
end

function setSkillBase(id, value, baseValue)
	if baseValue <= 0 or value < 0 then
		return
	end
	local skill = skillsWindow:recursiveGetChildById(id)
	local widget = skill:getChildById('value')

	if value > baseValue then
		widget:setColor('#008b00') -- green
		skill:setTooltip(baseValue .. ' +' .. (value - baseValue))
	elseif value < baseValue then
		widget:setColor('#b22222') -- red
		skill:setTooltip(baseValue .. ' ' .. (value - baseValue))
	else
		widget:setColor('#bbbbbb') -- default
		skill:removeTooltip()
	end
end

function setSkillValue(id, value)
	
	--if not skillWindow then return end

	local skill = skillsWindow:recursiveGetChildById(id)
	local widget = skill:getChildById('value')
	widget:setText(value)
	widget:setWidth(widget:getTextSize().width)
end

function setSkillNewValue(id, newValue, currentValue)

	local skill = skillsWindow:recursiveGetChildById(id)
	local widgetArrow = skill:getChildById('arrow')
	local widgetNewValue = skill:getChildById('newValue')
	widgetNewValue:setText(newValue)
	widgetNewValue:setWidth(widgetNewValue:getTextSize().width)
		
	if newValue <= currentValue or currentValue < 0 then
		widgetArrow:setVisible(false)
		widgetNewValue:setVisible(false)
	else
		widgetArrow:setVisible(true)
		widgetNewValue:setVisible(true)
	end
end

function setSkillColor(id, value)
  local skill = skillsWindow:recursiveGetChildById(id)
  local widget = skill:getChildById('value')
  widget:setColor(value)
end

function setSkillTooltip(id, value)
  local skill = skillsWindow:recursiveGetChildById(id)
  local widget = skill:getChildById('value')
  widget:setTooltip(value)
end

function setSkillPercent(id, percent, tooltip, color)
  local skill = skillsWindow:recursiveGetChildById(id)
  local widget = skill:getChildById('percent')
  if widget then
    widget:setPercent(math.floor(percent))

    if tooltip then
      widget:setTooltip(tooltip)
    end

    if color then
    	widget:setBackgroundColor(color)
    end
  end
end

function checkAlert(id, value, maxValue, threshold, greaterThan)
  if greaterThan == nil then greaterThan = false end
  local alert = false

  -- maxValue can be set to false to check value and threshold
  -- used for regeneration checking
  if type(maxValue) == 'boolean' then
    if maxValue then
      return
    end

    if greaterThan then
      if value > threshold then
        alert = true
      end
    else
      if value < threshold then
        alert = true
      end
    end
  elseif type(maxValue) == 'number' then
    if maxValue < 0 then
      return
    end

    local percent = math.floor((value / maxValue) * 100)
    if greaterThan then
      if percent > threshold then
        alert = true
      end
    else
      if percent < threshold then
        alert = true
      end
    end
  end

  if alert then
    setSkillColor(id, '#b22222') -- red
  else
    resetSkillColor(id)
  end
end

function refresh()

	local player = g_game.getLocalPlayer()
	if not player then return end
	
	resetChanges()
	
	onExperienceChange(player, player:getExperience())
	onLevelChange(player, player:getLevel(), player:getLevelPercent())
	onHealthChange(player, player:getHealth(), player:getMaxHealth())
	onManaChange(player, player:getMana(), player:getMaxMana())
	onSoulChange(player, player:getSoul())
	onSkillPointsChange(player, player:getSkillPoints())
	onFreeCapacityChange(player, player:getFreeCapacity())
	onStaminaChange(player, player:getStamina())
	onMagicLevelChange(player, player:getMagicLevel())
	onBaseMagicLevelChange(player, player:getBaseMagicLevel())
	onNewBaseMagicLevelChange(player, player:getNewBaseMagicLevel())
	onRegenerationChange(player, player:getRegenerationTime())
	onSpeedChange(player, player:getSpeed())
	onAttackSpeedChange(player, player:getAttackSpeed())


	if expSpeedEvent then 
		expSpeedEvent:cancel() 
	end
	
	expSpeedEvent = cycleEvent(checkExpSpeed, 30*1000)
	
	local hasAdditionalSkills = g_game.getFeature(GameAdditionalSkills)
	
	for i = Skill.Vitality, Skill.CriticalDamage do
	
		onSkillChange(player, i, player:getSkillLevel(i))
		onBaseSkillChange(player, i, player:getBaseSkillLevel(i))

		if i > Skill.Endurance then
			toggleSkill('skillId'.. i, hasAdditionalSkills)
		end
	end
	
	for i = Skill.Vitality, Skill.Endurance do
		onNewBaseSkillChange(player, i, player:getNewBaseSkillLevel(i))
	end

	skillsWindow:setContentMinimumHeight(46)
	skillsWindow:setContentMaximumHeight(374)
		
end

function offline()
  if expSpeedEvent then expSpeedEvent:cancel() expSpeedEvent = nil end
end

function toggle()
  if skillsButton:isOn() then
    skillsWindow:close()
    skillsButton:setOn(false)
  else
    skillsWindow:open()
    skillsButton:setOn(true)
  end
end

function checkExpSpeed()
  local player = g_game.getLocalPlayer()
  if not player then return end

  local currentExp = player:getExperience()
  local currentTime = g_clock.seconds()
  if player.lastExps ~= nil then
    player.expSpeed = (currentExp - player.lastExps[1][1])/(currentTime - player.lastExps[1][2])
    onLevelChange(player, player:getLevel(), player:getLevelPercent())
  else
    player.lastExps = {}
  end
  table.insert(player.lastExps, {currentExp, currentTime})
  if #player.lastExps > 30 then
    table.remove(player.lastExps, 1)
  end
end

function setExtraLabelsInvisible()

	applyButton:setVisible(false)
	
	newHealthArrowLabel:setVisible(false)
	newHealthValueLabel:setText('')
	newHealthValueLabel:setWidth(newHealthValueLabel:getTextSize().width)
	newHealthValueLabel:setVisible(false)
	
	newManaArrowLabel:setVisible(false)
	newManaValueLabel:setText('')
	newManaValueLabel:setWidth(newManaValueLabel:getTextSize().width)
	newManaValueLabel:setVisible(false)
		
	newSkillPointsArrowLabel:setVisible(false)
	newSkillPointsValueLabel:setText('')
	newSkillPointsValueLabel:setWidth(newSkillPointsValueLabel:getTextSize().width)
	newSkillPointsValueLabel:setVisible(false)
	
	newCapacityArrowLabel:setVisible(false)
	newCapacityValueLabel:setText('')
	newCapacityValueLabel:setWidth(newCapacityValueLabel:getTextSize().width)
	newCapacityValueLabel:setVisible(false)
	
	newWalkSpeedArrowLabel:setVisible(false)
	newWalkSpeedValueLabel:setText('')
	newWalkSpeedValueLabel:setWidth(newWalkSpeedValueLabel:getTextSize().width)
	newWalkSpeedValueLabel:setVisible(false)
	
	newAttackSpeedArrowLabel:setVisible(false)
	newAttackSpeedValueLabel:setText('')
	newAttackSpeedValueLabel:setWidth(newAttackSpeedValueLabel:getTextSize().width)
	newAttackSpeedValueLabel:setVisible(false)
	
	newMagicArrowLabel:setVisible(false)
	newMagicValueLabel:setText('')
	newMagicValueLabel:setWidth(newMagicValueLabel:getTextSize().width)
	newMagicValueLabel:setVisible(false)
	magicMinusButton:setVisible(false)
	magicPlusButton:setVisible(false)
	
	newVitalityArrowLabel:setVisible(false)
	newVitalityValueLabel:setText('')
	newVitalityValueLabel:setWidth(newVitalityValueLabel:getTextSize().width)
	newVitalityValueLabel:setVisible(false)
	vitalityMinusButton:setVisible(false)
	vitalityPlusButton:setVisible(false)
	
	newStrenghtArrowLabel:setVisible(false)
	newStrenghtValueLabel:setText('')
	newStrenghtValueLabel:setWidth(newStrenghtValueLabel:getTextSize().width)
	newStrenghtValueLabel:setVisible(false)
	strenghtMinusButton:setVisible(false)
	strenghtPlusButton:setVisible(false)
	
	newDefenceArrowLabel:setVisible(false)
	newDefenceValueLabel:setText('')
	newDefenceValueLabel:setWidth(newDefenceValueLabel:getTextSize().width)
	newDefenceValueLabel:setVisible(false)
	defenceMinusButton:setVisible(false)
	defencePlusButton:setVisible(false)
	
	newDexterityArrowLabel:setVisible(false)
	newDexterityValueLabel:setText('')
	newDexterityValueLabel:setWidth(newDexterityValueLabel:getTextSize().width)
	newDexterityValueLabel:setVisible(false)
	dexterityMinusButton:setVisible(false)
	dexterityPlusButton:setVisible(false)
	
	newIntelligenceArrowLabel:setVisible(false)
	newIntelligenceValueLabel:setText('')
	newIntelligenceValueLabel:setWidth(newIntelligenceValueLabel:getTextSize().width)
	newIntelligenceValueLabel:setVisible(false)
	intelligenceMinusButton:setVisible(false)
	intelligencePlusButton:setVisible(false)
	
	newFaithArrowLabel:setVisible(false)
	newFaithValueLabel:setText('')
	newFaithValueLabel:setWidth(newFaithValueLabel:getTextSize().width)
	newFaithValueLabel:setVisible(false)
	faithMinusButton:setVisible(false)
	faithPlusButton:setVisible(false)
	
	newEnduranceArrowLabel:setVisible(false)
	newEnduranceValueLabel:setText('')
	newEnduranceValueLabel:setWidth(newEnduranceValueLabel:getTextSize().width)
	newEnduranceValueLabel:setVisible(false)
	enduranceMinusButton:setVisible(false)
	endurancePlusButton:setVisible(false)
end

function resetChanges()

	if not player then 
		player = g_game.getLocalPlayer()
	end
	
	if not player then
		return 
	end

	newSkillPoints = player:getSkillPoints()
	newMagic = player:getBaseMagicLevel()
	newVitality = player:getBaseSkillLevel(Skill.Vitality)
	newStrenght = player:getBaseSkillLevel(Skill.Strenght)
	newDefence = player:getBaseSkillLevel(Skill.Defence)
	newDexterity = player:getBaseSkillLevel(Skill.Dexterity)
	newIntelligence = player:getBaseSkillLevel(Skill.Intelligence)
	newFaith = player:getBaseSkillLevel(Skill.Faith)
	newEndurance = player:getBaseSkillLevel(Skill.Endurance)

	newSkillPointsValueLabel:setText(newSkillPoints)
	magicValueLabel:setText(player:getMagicLevel())
	vitalityValueLabel:setText(player:getSkillLevel(Skill.Vitality))
	strenghtValueLabel:setText(player:getSkillLevel(Skill.Strenght))
	defenceValueLabel:setText(player:getSkillLevel(Skill.Defence))
	dexterityValueLabel:setText(player:getSkillLevel(Skill.Dexterity))
	intelligenceValueLabel:setText(player:getSkillLevel(Skill.Intelligence))
	faithValueLabel:setText(player:getSkillLevel(Skill.Faith))
	enduranceValueLabel:setText(player:getSkillLevel(Skill.Endurance))
	
	setExtraLabelsInvisible()
	checkPlusAndMinusButtons(player, newSkillPoints)
	
end

function checkPlusAndMinusButtons(localPlayer, skillPoints)

	if skillPoints >= magicInfo['cost'] then
		magicPlusButton:setVisible(true)
	else
		magicPlusButton:setVisible(false)
	end
		
	if skillPoints >= 1 then
		vitalityPlusButton:setVisible(true)
		strenghtPlusButton:setVisible(true)
		defencePlusButton:setVisible(true)
		dexterityPlusButton:setVisible(true)
		intelligencePlusButton:setVisible(true)
		faithPlusButton:setVisible(true)
		endurancePlusButton:setVisible(true)
	else
		vitalityPlusButton:setVisible(false)
		strenghtPlusButton:setVisible(false)
		defencePlusButton:setVisible(false)
		dexterityPlusButton:setVisible(false)
		intelligencePlusButton:setVisible(false)
		faithPlusButton:setVisible(false)
		endurancePlusButton:setVisible(false)
	end
			
	if newMagic > localPlayer:getBaseMagicLevel() then
		magicMinusButton:setVisible(true)
	else
		magicMinusButton:setVisible(false)
	end
	
	if newStrenght > localPlayer:getBaseSkillLevel(Skill.Strenght) then
		strenghtMinusButton:setVisible(true)
	else
		strenghtMinusButton:setVisible(false)
	end
		
	if newDefence > localPlayer:getBaseSkillLevel(Skill.Defence) then
		defenceMinusButton:setVisible(true)
	else
		defenceMinusButton:setVisible(false)
	end

	if newDexterity > localPlayer:getBaseSkillLevel(Skill.Dexterity) then
		dexterityMinusButton:setVisible(true)
	else
		dexterityMinusButton:setVisible(false)
	end
		
	if newIntelligence > localPlayer:getBaseSkillLevel(Skill.Intelligence) then
		intelligenceMinusButton:setVisible(true)
	else
		intelligenceMinusButton:setVisible(false)
	end
			
	if newDefence > localPlayer:getBaseSkillLevel(Skill.Faith) then
		faithMinusButton:setVisible(true)
	else
		faithMinusButton:setVisible(false)
	end

	if newEndurance > localPlayer:getBaseSkillLevel(Skill.Endurance) then
		enduranceMinusButton:setVisible(true)
	else
		enduranceMinusButton:setVisible(false)
	end
	
end

function onMiniWindowClose()
	skillsButton:setOn(false)	
end

function onMiniWindowOpen()

	player = g_game.getLocalPlayer()
	
	resetChanges()

	skillsButton:setOn(true)
	
	magicMinusButton:setVisible(false)
	vitalityMinusButton:setVisible(false)
	strenghtMinusButton:setVisible(false)
	defenceMinusButton:setVisible(false)
	dexterityMinusButton:setVisible(false)
	intelligenceMinusButton:setVisible(false)
	faithMinusButton:setVisible(false)
	enduranceMinusButton:setVisible(false)
	
end

function onExperienceChange(localPlayer, value)
  setSkillValue('experience', value)
end

function onLevelChange(localPlayer, value, percent)
	setSkillValue('level', value)
	local text = tr('You have %s percent to go', 100 - percent) .. '\n' ..
		tr('%s of experience left', expToAdvance(localPlayer:getLevel(), localPlayer:getExperience()))

	if localPlayer.expSpeed ~= nil then
		local expPerHour = math.floor(localPlayer.expSpeed * 3600)
		if expPerHour > 0 then
			local nextLevelExp = expForLevel(localPlayer:getLevel()+1)
			local hoursLeft = (nextLevelExp - localPlayer:getExperience()) / expPerHour
			local minutesLeft = math.floor((hoursLeft - math.floor(hoursLeft))*60)
			hoursLeft = math.floor(hoursLeft)
			text = text .. '\n' .. tr('%d of experience per hour', expPerHour)
			text = text .. '\n' .. tr('Next level in %d hours and %d minutes', hoursLeft, minutesLeft)
		end
	end

	setSkillPercent('level', percent, text)
end

function onHealthChange(localPlayer, health, maxHealth)
  setSkillValue('health', health)
  checkAlert('health', health, maxHealth, 30)
end

function onManaChange(localPlayer, mana, maxMana)
  setSkillValue('mana', mana)
  checkAlert('mana', mana, maxMana, 30)
end

function onBlessingsChange(localPlayer, blessingsCount)
  setSkillValue('blessings', blessingsCount)
end

function onResetsChange(localPlayer, resetsCount)
  setSkillValue('resets', resetsCount)
end

function onSoulChange(localPlayer, soul)
  setSkillValue('soul', soul)
end

function onSkillPointsChange(localPlayer, skillPoints)

	if not player then 
		player = g_game.getLocalPlayer()
		resetChanges()
	end
	
	if not player then
		return 
	end

	if newSkillPoints < 0 then newSkillPoints = skillPoints end
	if newMagic < 0 then newMagic = player:getBaseMagicLevel() end
	if newVitality < 0 then newVitality = player:getBaseSkillLevel(Skill.Vitality) end
	if newStrenght < 0 then newStrenght = player:getBaseSkillLevel(Skill.Strenght) end
	if newDefence < 0 then newDefence = player:getBaseSkillLevel(Skill.Defence) end
	if newDexterity < 0 then newDexterity = player:getBaseSkillLevel(Skill.Dexterity) end
	if newIntelligence < 0 then newIntelligence = player:getBaseSkillLevel(Skill.Intelligence) end
	if newFaith < 0 then newFaith = player:getBaseSkillLevel(Skill.Faith) end
	if newEndurance < 0 then newEndurance = player:getBaseSkillLevel(Skill.Endurance) end

	skillPointsValueLabel:setText(skillPoints)
	skillPointsValueLabel:setWidth(skillPointsValueLabel:getTextSize().width)
		
	if skillPoints >= magicInfo['cost'] then
		magicPlusButton:setVisible(true)
	else
		magicPlusButton:setVisible(false)
	end
		
	if skillPoints >= 1 then
		vitalityPlusButton:setVisible(true)
		strenghtPlusButton:setVisible(true)
		defencePlusButton:setVisible(true)
		dexterityPlusButton:setVisible(true)
		intelligencePlusButton:setVisible(true)
		faithPlusButton:setVisible(true)
		endurancePlusButton:setVisible(true)
	else
		vitalityPlusButton:setVisible(false)
		strenghtPlusButton:setVisible(false)
		defencePlusButton:setVisible(false)
		dexterityPlusButton:setVisible(false)
		intelligencePlusButton:setVisible(false)
		faithPlusButton:setVisible(false)
		endurancePlusButton:setVisible(false)
	end
	
	toggle()
	toggle()

end

function onFreeCapacityChange(localPlayer, freeCapacity)

  local decimals = 0
  local mult = 10^(decimals)
  local value = math.modf(freeCapacity * mult) / mult

  setSkillValue('capacity', value)
  checkAlert('capacity', freeCapacity, localPlayer:getTotalCapacity(), 20)
end

function onTotalCapacityChange(localPlayer, totalCapacity)
  checkAlert('capacity', localPlayer:getFreeCapacity(), totalCapacity, 20)
end

function onStaminaChange(localPlayer, stamina)
  local hours = math.floor(stamina / 60)
  local minutes = stamina % 60
  if minutes < 10 then
    minutes = '0' .. minutes
  end
  local percent = math.floor(100 * stamina / (42 * 60)) -- max is 42 hours --TODO not in all client versions

  setSkillValue('stamina', hours .. ":" .. minutes)

  --TODO not all client versions have premium time
  if stamina > 2400 and g_game.getClientVersion() >= 1038 and localPlayer:isPremium() then
  	local text = tr("You have %s hours and %s minutes left", hours, minutes) .. '\n' ..
		tr("Now you will gain 50%% more experience")
		setSkillPercent('stamina', percent, text, 'green')
	elseif stamina > 2400 and g_game.getClientVersion() >= 1038 and not localPlayer:isPremium() then
		local text = tr("You have %s hours and %s minutes left", hours, minutes) .. '\n' ..
		tr("You will not gain 50%% more experience because you aren't premium player, now you receive only 1x experience points")
		setSkillPercent('stamina', percent, text, '#89F013')
	elseif stamina > 2400 and g_game.getClientVersion() < 1038 then
		local text = tr("You have %s hours and %s minutes left", hours, minutes) .. '\n' ..
		tr("If you are premium player, you will gain 50%% more experience")
		setSkillPercent('stamina', percent, text, 'green')
	elseif stamina <= 2400 and stamina > 840 then
		setSkillPercent('stamina', percent, tr("You have %s hours and %s minutes left", hours, minutes), 'orange')
	elseif stamina <= 840 and stamina > 0 then
		local text = tr("You have %s hours and %s minutes left", hours, minutes) .. "\n" ..
		tr("You gain only 50%% experience and you don't may gain loot from monsters")
		setSkillPercent('stamina', percent, text, 'red')
	elseif stamina == 0 then
		local text = tr("You have %s hours and %s minutes left", hours, minutes) .. "\n" ..
		tr("You don't may receive experience and loot from monsters")
		setSkillPercent('stamina', percent, text, 'black')
	end
end

function onRegenerationChange(localPlayer, regenerationTime)
  if not g_game.getFeature(GamePlayerRegenerationTime) or regenerationTime < 0 then
    return
  end
  local minutes = math.floor(regenerationTime / 60)
  local seconds = regenerationTime % 60
  if seconds < 10 then
    seconds = '0' .. seconds
  end

  setSkillValue('regenerationTime', minutes .. ":" .. seconds)
  checkAlert('regenerationTime', regenerationTime, false, 300)
end

function onBaseSpeedChange(localPlayer, baseSpeed)

	walkSpeedValueLabel:setText(baseSpeed)
	walkSpeedValueLabel:setWidth(walkSpeedValueLabel:getTextSize().width)
	
	currentSpeed = localPlayer:getSpeed()
	
	if currentSpeed > baseSpeed then
		walkSpeedValueLabel:setColor('#008b00') -- green
		walkSpeedButton:setTooltip(baseSpeed .. ' +' .. (currentSpeed - baseSpeed))
	elseif currentSpeed < baseSpeed then
		walkSpeedValueLabel:setColor('#b22222') -- red
		walkSpeedButton:setTooltip(baseSpeed .. ' ' .. (currentSpeed - baseSpeed))
	else
		walkSpeedValueLabel:setColor('#bbbbbb') -- default
		walkSpeedButton:removeTooltip()
	end
end

function onSpeedChange(localPlayer, speed)

	walkSpeedValueLabel:setText(speed)
	walkSpeedValueLabel:setWidth(walkSpeedValueLabel:getTextSize().width)
	
	baseSpeed = localPlayer:getBaseSpeed()
	
	if speed > baseSpeed then
		walkSpeedValueLabel:setColor('#008b00') -- green
		walkSpeedButton:setTooltip(baseSpeed .. ' +' .. (speed - baseSpeed))
	elseif speed < baseSpeed then
		walkSpeedValueLabel:setColor('#b22222') -- red
		walkSpeedButton:setTooltip(baseSpeed .. ' ' .. (speed - baseSpeed))
	else
		walkSpeedValueLabel:setColor('#bbbbbb') -- default
		walkSpeedButton:removeTooltip()
	end
end

function onAttackSpeedChange(localPlayer, attackSpeed)

	local attackSpeedInPercent = ((initial.attackSpeed + (2 * (initial.attackSpeed - attackSpeed))) / initial.attackSpeed) * 100

	attackSpeedValueLabel:setText(attackSpeedInPercent .. '%')
	attackSpeedValueLabel:setWidth(attackSpeedValueLabel:getTextSize().width)
	
	if attackSpeed == dualWieldingAttackSpeed then
		attackSpeedValueLabel:setColor('#008b00') -- green
		attackSpeedButton:setTooltip('Dual wielding')
	else
		attackSpeedValueLabel:setColor('#bbbbbb') -- default
		attackSpeedButton:removeTooltip()
	end
end

function onBaseMagicLevelChange(localPlayer, baseMagicLevel)
	
	magicValueLabel:setText(baseMagicLevel)
	magicValueLabel:setWidth(magicValueLabel:getTextSize().width)
	
	setSkillsTooltips()
end

function onMagicLevelChange(localPlayer, magicLevel)
	local baseMagic = localPlayer:getBaseMagicLevel()
	
	if baseMagic <= 0 or magicLevel < 0 then
		return
	end

	magicValueLabel:setText(magicLevel)
	magicValueLabel:setWidth(magicValueLabel:getTextSize().width)

	if magicLevel > baseMagic then
		magicValueLabel:setColor('#008b00') -- green
		magicValueLabel:setTooltip(baseMagic .. ' +' .. (magicLevel - baseMagic))
	elseif magicLevel < baseMagic then
		magicValueLabel:setColor('#b22222') -- red
		magicValueLabel:setTooltip(baseMagic .. ' ' .. (magicLevel - baseMagic))
	else
		magicValueLabel:setColor('#bbbbbb') -- default
		magicValueLabel:removeTooltip()
	end
	
	setSkillsTooltips()
end

function onNewBaseMagicLevelChange(localPlayer, newBaseMagicLevel)
	setSkillNewValue('magiclevel', newBaseMagicLevel, localPlayer:getBaseMagicLevel())
end

function onBaseSkillChange(localPlayer, id, baseLevel)
	setSkillBase('skillId'..id, localPlayer:getSkillLevel(id), baseLevel)
	setSkillsTooltips()
	
	if id == Skill.Faith then
	
		local rodBonus = (localPlayer:getBaseSkillLevel(Skill.Faith) - 8) * 2
		if rodBonus > 0 then
			rodDamageValueLabel:setText(rodBonus + 100 .. '%')
		else
			rodDamageValueLabel:setText('100%')
		end
		rodDamageValueLabel:setWidth(rodDamageValueLabel:getTextSize().width)
		
	elseif id == Skill.Intelligence then
	
		local wandBonus = localPlayer:getBaseSkillLevel(Skill.Intelligence) - 8
		if wandBonus > 0 then
			wandDamageValueLabel:setText(wandBonus + 100 .. '%')
		else
			wandDamageValueLabel:setText('100%')
		end
		wandDamageValueLabel:setWidth(wandDamageValueLabel:getTextSize().width)
	end
end

function onSkillChange(localPlayer, id, level)
	
	if id > 0 and id < Skill.CriticalChance then
		setSkillValue('skillId' .. id, level)
		setSkillBase('skillId'.. id, localPlayer:getSkillLevel(id), level)
		setSkillsTooltips()
	end

	if id == Skill.Vitality then
	
		local baseVitality = localPlayer:getBaseSkillLevel(Skill.Vitality)
		
		if baseVitality <= 0 or level < 0 then
			return
		end
		
		vitalityValueLabel:setText(math.max(baseVitality, level))
		vitalityValueLabel:setWidth(vitalityValueLabel:getTextSize().width)
		
		if level > baseVitality then
			vitalityValueLabel:setColor('#008b00') -- green
			vitalityValueLabel:setTooltip(baseVitality .. ' +' .. (level - baseVitality))
		elseif level < baseVitality then
			vitalityValueLabel:setColor('#b22222') -- red
			vitalityValueLabel:setTooltip(baseVitality .. ' ' .. (level - baseVitality))
		else
			vitalityValueLabel:setColor('#bbbbbb') -- default
			vitalityValueLabel:removeTooltip()
		end
	
	elseif id == Skill.Strenght then
		
		local baseStrenght = localPlayer:getBaseSkillLevel(Skill.Strenght)

		if baseStrenght <= 0 or level < 0 then
			return
		end
		
		strenghtValueLabel:setText(math.max(baseStrenght, level))
		strenghtValueLabel:setWidth(strenghtValueLabel:getTextSize().width)
		
		if level > baseStrenght then
			strenghtValueLabel:setColor('#008b00') -- green
			strenghtValueLabel:setTooltip(baseStrenght .. ' +' .. (level - baseStrenght))
		elseif level < baseStrenght then
			strenghtValueLabel:setColor('#b22222') -- red
			strenghtValueLabel:setTooltip(baseStrenght .. ' ' .. (level - baseStrenght))
		else
			strenghtValueLabel:setColor('#bbbbbb') -- default
			strenghtValueLabel:removeTooltip()
		end
		
	elseif id == Skill.Faith then
		
		local baseFaith = localPlayer:getBaseSkillLevel(Skill.Faith)

		if baseFaith <= 0 or level < 0 then
			return
		end
		
		faithValueLabel:setText(math.max(baseFaith, level))
		faithValueLabel:setWidth(faithValueLabel:getTextSize().width)
		
		if level > baseFaith then
			faithValueLabel:setColor('#008b00') -- green
			faithValueLabel:setTooltip(baseFaith .. ' +' .. (level - baseFaith))
		elseif level < baseFaith then
			faithValueLabel:setColor('#b22222') -- red
			faithValueLabel:setTooltip(baseFaith .. ' ' .. (level - baseFaith))
		else
			faithValueLabel:setColor('#bbbbbb') -- default
			faithValueLabel:removeTooltip()
		end
		
	elseif id == Skill.Intelligence then
		
		local baseIntelligence = localPlayer:getBaseSkillLevel(Skill.Intelligence)

		if baseIntelligence <= 0 or level < 0 then
			return
		end
		
		intelligenceValueLabel:setText(math.max(baseIntelligence, level))
		intelligenceValueLabel:setWidth(intelligenceValueLabel:getTextSize().width)
		
		if level > baseIntelligence then
			intelligenceValueLabel:setColor('#008b00') -- green
			intelligenceValueLabel:setTooltip(baseIntelligence .. ' +' .. (level - baseIntelligence))
		elseif level < baseIntelligence then
			intelligenceValueLabel:setColor('#b22222') -- red
			intelligenceValueLabel:setTooltip(baseIntelligence .. ' ' .. (level - baseIntelligence))
		else
			intelligenceValueLabel:setColor('#bbbbbb') -- default
			intelligenceValueLabel:removeTooltip()
		end
		
	elseif id == Skill.Dexterity then
		
		local baseDexterity = localPlayer:getBaseSkillLevel(Skill.Dexterity)

		if baseDexterity <= 0 or level < 0 then
			return
		end
		
		dexterityValueLabel:setText(math.max(baseDexterity, level))
		dexterityValueLabel:setWidth(dexterityValueLabel:getTextSize().width)
		
		if level > baseDexterity then
			dexterityValueLabel:setColor('#008b00') -- green
			dexterityValueLabel:setTooltip(baseDexterity .. ' +' .. (level - baseDexterity))
		elseif level < baseDexterity then
			dexterityValueLabel:setColor('#b22222') -- red
			dexterityValueLabel:setTooltip(baseDexterity .. ' ' .. (level - baseDexterity))
		else
			dexterityValueLabel:setColor('#bbbbbb') -- default
			dexterityValueLabel:removeTooltip()
		end
		
	elseif id == Skill.Defence then
		
		local baseDefence = localPlayer:getBaseSkillLevel(Skill.Defence)

		if baseDefence <= 0 or level < 0 then
			return
		end
		
		defenceValueLabel:setText(math.max(baseDefence, level))
		defenceValueLabel:setWidth(defenceValueLabel:getTextSize().width)
		
		if level > baseDefence then
			defenceValueLabel:setColor('#008b00') -- green
			defenceValueLabel:setTooltip(baseDefence .. ' +' .. (level - baseDefence))
		elseif level < baseDefence then
			defenceValueLabel:setColor('#b22222') -- red
			defenceValueLabel:setTooltip(baseDefence .. ' ' .. (level - baseDefence))
		else
			defenceValueLabel:setColor('#bbbbbb') -- default
			defenceValueLabel:removeTooltip()
		end
		
	elseif id == Skill.Endurance then
		
		local baseEndurance = localPlayer:getBaseSkillLevel(Skill.Endurance)

		if baseEndurance <= 0 or level < 0 then
			return
		end
		
		enduranceValueLabel:setText(math.max(baseEndurance, level))
		enduranceValueLabel:setWidth(enduranceValueLabel:getTextSize().width)
		
		if level > baseEndurance then
			enduranceValueLabel:setColor('#008b00') -- green
			enduranceValueLabel:setTooltip(baseEndurance .. ' +' .. (level - baseEndurance))
		elseif level < baseEndurance then
			enduranceValueLabel:setColor('#b22222') -- red
			enduranceValueLabel:setTooltip(baseEndurance .. ' ' .. (level - baseEndurance))
		else
			enduranceValueLabel:setColor('#bbbbbb') -- default
			enduranceValueLabel:removeTooltip()
		end
	
	--[[elseif id == Skill.CriticalChance then
		
		local baseCriticalChance = localPlayer:getBaseSkillLevel(Skill.CriticalChance)

		if baseCriticalChance < 0 or level < 0 then
			return
		end
		
		criticalChanceValueLabel:setText(math.max(baseCriticalChance, level))
		criticalChanceValueLabel:setWidth(criticalChanceValueLabel:getTextSize().width)
		
		if level > baseCriticalChance then
			criticalChanceValueLabel:setColor('#008b00') -- green
			criticalChanceValueLabel:setTooltip(baseCriticalChance .. ' +' .. (level - baseCriticalChance))
		elseif level < baseCriticalChance then
			criticalChanceValueLabel:setColor('#b22222') -- red
			criticalChanceValueLabel:setTooltip(baseCriticalChance .. ' ' .. (level - baseCriticalChance))
		else
			criticalChanceValueLabel:setColor('#bbbbbb') -- default
			criticalChanceValueLabel:removeTooltip()
		end
	
	elseif id == Skill.CriticalDamage then
		
		local baseCriticalDamage = localPlayer:getBaseSkillLevel(Skill.CriticalDamage)

		if baseCriticalDamage < 0 or level < 0 then
			return
		end
		
		criticalDamageValueLabel:setText(math.max(baseCriticalDamage, level))
		criticalDamageValueLabel:setWidth(criticalDamageValueLabel:getTextSize().width)
		
		if level > baseCriticalDamage then
			criticalDamageValueLabel:setColor('#008b00') -- green
			criticalDamageValueLabel:setTooltip(baseCriticalDamage .. ' +' .. (level - baseCriticalDamage))
		elseif level < baseCriticalDamage then
			criticalDamageValueLabel:setColor('#b22222') -- red
			criticalDamageValueLabel:setTooltip(baseCriticalDamage .. ' ' .. (level - baseCriticalDamage))
		else
			criticalDamageValueLabel:setColor('#bbbbbb') -- default
			criticalDamageValueLabel:removeTooltip()
		end]]	
	end
	
	toggle()
	toggle()
end

function onNewBaseSkillChange(localPlayer, id, level)
	setSkillNewValue('skillId' .. id, level, localPlayer:getBaseSkillLevel(id))
end