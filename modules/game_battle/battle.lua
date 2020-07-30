-- Global Tables
local binaryTree = {} -- BST
local battleButtons = {} -- map of creature id

-- Global variables that will inherit from init
local battleWindow, battleButton, battlePanel, mouseWidget, filterPanel, toggleFilterButton
local lastBattleButtonSwitched

-- Hide Buttons ("hidePlayers", "hideNPCs", "hideMonsters", "hideSkulls", "hideParty")
local hideButtons = {}

local function connecting()
	-- TODO: Just connect when you will be using
	connect(g_game, {
		onAttackingCreatureChange = onAttack,
		onFollowingCreatureChange = onFollow,
		onGameEnd = removeAllCreatures
	})

	connect(LocalPlayer, {
		onPositionChange = onCreaturePositionChange
	})

	connect(Creature, {
		onSkullChange = updateCreatureSkull,
		onEmblemChange = updateCreatureEmblem,
		onOutfitChange = onCreatureOutfitChange,
		onHealthPercentChange = onCreatureHealthPercentChange,
		onPositionChange = onCreaturePositionChange,
		onAppear = onCreatureAppear,
		onDisappear = onCreatureDisappear
	})

	-- Check creatures around you
	checkCreatures()
	return true
end

local function disconnecting()
	-- TODO: Just disconnect what you're not using
	disconnect(g_game, {
		onAttackingCreatureChange = onAttack,
		onFollowingCreatureChange = onFollow,
		onGameEnd = removeAllCreatures
	})

	disconnect(LocalPlayer, {
		onPositionChange = onCreaturePositionChange
	})

	disconnect(Creature, {
		onSkullChange = updateCreatureSkull,
		onEmblemChange = updateCreatureEmblem,
		onOutfitChange = onCreatureOutfitChange,
		onHealthPercentChange = onCreatureHealthPercentChange,
		onPositionChange = onCreaturePositionChange,
		onAppear = onCreatureAppear,
		onDisappear = onCreatureDisappear
	})
	return true
end

function init() -- Initiating the module (load)
	g_ui.importStyle('battlebutton')
	battleButton = modules.client_topmenu.addRightGameToggleButton('battleButton', tr('Battle') .. ' (Ctrl+B)', '/images/topbuttons/battle', toggle)
	battleButton:setOn(true)
	battleWindow = g_ui.loadUI('battle', modules.game_interface.getRightPanel())

	-- Binding Ctrl + B shortcut
	g_keyboard.bindKeyDown('Ctrl+B', toggle)

	-- Disabling scrollbar auto hiding
	local scrollbar = battleWindow:getChildById('miniwindowScrollBar')
	scrollbar:mergeStyle({ ['$!on'] = { }})

	battlePanel = battleWindow:recursiveGetChildById('battlePanel')
	filterPanel = battleWindow:recursiveGetChildById('filterPanel')
	toggleFilterButton = battleWindow:recursiveGetChildById('toggleFilterButton')

	-- Hide/Show Filter Options
	local settings = g_settings.getNode('BattleList')
	if settings and settings['hidingFilters'] then
		hideFilterPanel()
	end

	-- Adding Filter options
	local options = {"hidePlayers", "hideNPCs", "hideMonsters", "hideSkulls", "hideParty"}
	for i, v in ipairs(options) do
		hideButtons[v] = battleWindow:recursiveGetChildById(v)
	end

	-- Adding SortType and SortOrder options
	local sortTypeOptions = {"Name", "Distance", "Age", "Health"}
	local sortOrderOptions = {"Asc.", "Desc."}

	local sortTypeBox = battleWindow:recursiveGetChildById('sortTypeBox')
	for i, v in ipairs(sortTypeOptions) do sortTypeBox:addOption(v, v:lower()) end

	local sortOrderBox = battleWindow:recursiveGetChildById('sortOrderBox')
	for i, v in ipairs(sortOrderOptions) do sortOrderBox:addOption(v, v:lower()) end
	sortTypeBox:setCurrentOptionByData(getSortType())
	sortTypeBox.onOptionChange = onChangeSortType
	sortOrderBox:setCurrentOptionByData(getSortOrder())
	sortOrderBox.onOptionChange = onChangeSortOrder

	-- Adding mouse Widget
	mouseWidget = g_ui.createWidget('UIButton')
	mouseWidget:setVisible(false)
	mouseWidget:setFocusable(false)
	mouseWidget.cancelNextRelease = false

	-- Setting Connectors
	connecting()

	-- Determining Height and Setting up!
	battleWindow:setContentMinimumHeight(80)
	battleWindow:setup()
end

-- Binary Search, Insertion and Resort functions
local function debugTables(sortType) -- Print both battlebutton and binarytree tables

	local function getInfo(v, sortType)
		local returnedInfo = v.id
		if sortType then
			if sortType == "distance" then
				returnedInfo = v.distance
			elseif sortType == "health" then
				returnedInfo = v.healthpercent
			elseif sortType == "age" then
				returnedInfo = v.age
			else
				returnedInfo = v.name
			end
		end
		return returnedInfo
	end

	print('-----------------------------')
	local msg = "printing binaryTree: {"
	for i, v in pairs(binaryTree) do
		msg = msg.."["..i.."] = ".. getInfo(v, "name")  .." [".. getInfo(v, sortType) .."],"
	end
	msg = msg.."}"
	print(msg)

	msg = "printing battleButtons: {"
	for i, v in pairs(battleButtons) do
		msg = msg.."[".. getInfo(v.data, "name").."] = ".. getInfo(v.data, sortType) ..","
	end
	msg = msg.."}"
	print(msg)


return true
end

local function BSComparator(a, b) -- Default comparator function, we probably won't use it here.
	if a > b then return -1
	elseif a < b then return 1
	else return 0
	end
end

local function BSComparatorSortType(a, b, sortType, id) -- Comparator function by sortType (and id optionally)
    local comparatorA, comparatorB
	if sortType == 'distance' then
		comparatorA, comparatorB = a.distance, (type(b) == "table" and b.distance or b)
	elseif sortType == 'health' then
		comparatorA, comparatorB = a.healthpercent, type(b) == "table" and b.healthpercent or b
	elseif sortType == 'age' then
		comparatorA, comparatorB = a.age, type(b) == "table" and b.age or b
    elseif sortType == 'name' then
        comparatorA, comparatorB = (a.name):lower(), type(b) == "table" and (b.name):lower() or b
	end
	if comparatorA > comparatorB then return -1
	elseif comparatorA < comparatorB then return 1
	else
		if id then
			if b and b.id and a.id > b.id then return -1
			elseif b and b.id and a.id < b.id then return 1
			end
		end
		return 0
	end
end

local function binarySearch(tbl, value, comparator, ...) -- Binary Search function, to search a value in our binaryTree
	if not comparator then
		comparator = BSComparator
	end

	local mini = 1
	local maxi = #tbl
	local mid = 1

	while mini <= maxi do
		mid = math.floor((maxi+mini)/2)
		local tmp_value = comparator(tbl[mid], value, ...)

		if tmp_value == 0 then
			return mid
		elseif tmp_value < 0 then
			maxi = mid - 1
		else
			mini = mid + 1
		end
	end
	return nil
end

local function binaryInsert(tbl, value, comparator, ...) -- Binary Insertion function, to insert a value in our binaryTree
	if not comparator then
		comparator = BSComparator
	end

	local mini = 1
	local maxi = #tbl
	local state = 0
	local mid = 1

	while mini <= maxi do
		mid = math.floor((maxi+mini)/2)

		if comparator(tbl[mid], value, ...) < 0 then
			maxi, state = mid - 1, 0
		else
			mini, state = mid + 1, 1
		end
	end
    table.insert(tbl, mid + state, value)
	return (mid + state)
end

local function swap(index, newIndex) -- Swap indexes of a given table
	local highest = newIndex
	local lowest = index

	if index > newIndex then
		highest = index
		lowest = newIndex
	end

	local tmp = binaryTree[lowest]
	binaryTree[lowest] = binaryTree[highest]
	binaryTree[highest] = tmp
end

local function correctBattleButtons(sortOrder) -- Update battleButton index based upon our binary tree
	local sortOrder = sortOrder or getSortOrder()

	local start = sortOrder == "A" and 1 or #binaryTree
	local finish = #binaryTree - start + 1
	local increment = start <= finish and 1 or -1

	local index = 1
	for i = start, finish, increment do
		local v = binaryTree[i]
		local battleButton = battleButtons[v.id]
		if battleButton ~= nil then
			battlePanel:moveChildToIndex(battleButton, index)
			index = index + 1
		end
	end
return true
end

local function reSort(oldSortType, newSortType, oldSortOrder, newSortOrder) -- Resort the binaryTree and update battlebuttons
	if #binaryTree > 1 then
		if newSortType and newSortType ~= oldSortType then
			-- Unfortunately we cannot use this as we have no guarantees that other sort types have their information updated
			--table.sort(binaryTree, function(a, b) if a and b then return BSComparatorSortType(a, b, newSortType, true) == 1 end end)
			checkCreatures()
		end

		if newSortOrder then --and newSortOrder ~= oldSortOrder then: we need to move regardless of oldSortOrder
			correctBattleButtons(newSortOrder)
		end
	end

return true
end

-- Sort Type Methods
function getSortType() -- Return the current sort type (distance, age, name, health)
	local settings = g_settings.getNode('BattleList')
	if not settings then
		return 'name'
	end
	return settings['sortType']
end

function setSortType(state, oldSortType) -- Setting the current sort type (distance, age, name, health)
	settings = {}
	settings['sortType'] = state
	g_settings.mergeNode('BattleList', settings)

	local order = getSortOrder()
	reSort(oldSortType, state, order, order)
end

function onChangeSortType(comboBox, option) -- Callback when change the sort type (distance, age, name, health)
	local loption = option:lower()
	local oldType = getSortType()

	if loption ~= oldType then
		setSortType(loption, oldType)
	end
end

-- Sort Order Methods
function getSortOrder() -- Return the current sort ordenation (asc/desc)
	local settings = g_settings.getNode('BattleList')
	if not settings then
		return 'A'
	end
	return settings['sortOrder']
end

function setSortOrder(state, oldSortOrder) -- Setting the current sort ordenation (desc/asc)
	settings = {}
	settings['sortOrder'] = state
	g_settings.mergeNode('BattleList', settings)

	reSort(false, false, oldSortOrder, state)
end

function isSortAsc() -- Return true if sorted Asc
    return getSortOrder() == 'A'
end

function isSortDesc() -- Return true if sorted Desc
    return getSortOrder() == 'D'
end

function onChangeSortOrder(comboBox, option) -- Callback when change the sort ordenation
	local soption = option:sub(1, 1)
	local oldOrder = getSortOrder()

	if soption ~= oldOrder then
		setSortOrder(option:sub(1, 1), oldOrder)
	end
end

-- Initially checking creatures
function checkCreatures() -- Function that initially populates our tree once the module is initialized
	if not battlePanel or not g_game.isOnline() then
		return false
	end

	local player = g_game.getLocalPlayer()
	if not player then
		return false
	end

	removeAllCreatures() -- Remove all cache if there's any

	--[[ TODO: just search inside the dimensions of screen
	local dimension = modules.game_interface.getMapPanel():getVisibleDimension() -- not being used yet
	local spectators = g_map.getSpectatorsInRangeEx(player:getPosition(), false, math.floor(dimension.width / 2), math.floor(dimension.width / 2), math.floor(dimension.height / 2), math.floor(dimension.height / 2))
	-- Will also require an event to update the list when the zoom in screen is increased/reduced.
	]]

	local spectators = g_map.getSpectators(player:getPosition(), false)
	local sortType = getSortType()

	for _, creature in ipairs(spectators) do
		if doCreatureFitFilters(creature) then
			addCreature(creature, sortType)
		end
	end
end

function doCreatureFitFilters(creature) -- Check if creature fit current applied filters (By changing the filter we will call checkCreatures(true) to recreate the tree)
	if creature:isLocalPlayer() then
		return false
	end

	local pos = creature:getPosition()
	if not pos then return false end

	local localPlayer = g_game.getLocalPlayer()
	if pos.z ~= localPlayer:getPosition().z or not creature:canBeSeen() then return false end -- or not localPlayer:hasSight(pos)
	for i, v in pairs(hideButtons) do
		if v:isChecked() then
			if 	(i == "hidePlayers" and creature:isPlayer()) or
				(i == "hideNPCs" and creature:isNpc()) or
				(i == "hideMonsters" and creature:isMonster()) or
				(i == "hideSkulls" and (creature:isPlayer() and creature:getSkull() == SkullNone)) or
				(i == "hideParty" and creature:getShield() > ShieldWhiteBlue) then
				return false
			end
		end
	end

	return true
end

local function getDistanceBetween(p1, p2) -- Calculate distance
    return math.max(math.abs(p1.x - p2.x), math.abs(p1.y - p2.y))
end

-- Adding and Removing creatures
local function getAttributeByOrderType(battleButton, orderType) -- Return the attribute of battleButton based on the orderType
	if battleButton.data then
		local battleButton = battleButton.data
		if orderType == 'distance' then
			return {distance = battleButton.distance}
		elseif orderType == 'health' then
			return {healthpercent = battleButton.healthpercent}
		elseif orderType == 'age' then
			return {age = battleButton.age}
		else
			return {name = battleButton.name}
		end
	end
	return false
end

local lastAge = 0
function addCreature(creature, sortType) -- Insert a creature in our binary tree
	local creatureId = creature:getId()
	local battleButton = battleButtons[creatureId]
	if battleButton then
		-- I don't think this situation will exist but let's keep it here.
		battleButton:setLifeBarPercent(creature:getHealthPercent())
	else
		local newCreature = {}
		newCreature.id = creatureId
		newCreature.name = creature:getName():lower()
		newCreature.healthpercent = creature:getHealthPercent()
		newCreature.distance = getDistanceBetween(g_game.getLocalPlayer():getPosition(), creature:getPosition())
		newCreature.age = lastAge + 1
		lastAge = lastAge + 1

		--Binary Insertion
		local newIndex = binaryInsert(binaryTree, newCreature, BSComparatorSortType, sortType, true)

		battleButton = g_ui.createWidget('BattleButton')
		battleButton:setup(creature)
		battleButton:show()
		battleButton:setOn(true)

		battleButton.data = {}
		--Batle Button insertion
		for i, v in pairs(newCreature) do
			battleButton.data[i] = v
		end

		battleButton.onHoverChange = onBattleButtonHoverChange
		battleButton.onMouseRelease = onBattleButtonMouseRelease
		battleButtons[creatureId] = battleButton

		if creature == g_game.getAttackingCreature() then
			onAttack(creature)
		end

		if creature == g_game.getFollowingCreature() then
		  onFollow(creature)
		end


		if isSortAsc() then
			battlePanel:insertChild(newIndex, battleButton)
		else
			battlePanel:insertChild((#binaryTree - newIndex + 1), battleButton)
		end
	end

	local localPlayer = g_game.getLocalPlayer()
	battleButton:setVisible(localPlayer:hasSight(creature:getPosition()) and creature:canBeSeen())
	battlePanel:getLayout():update()
end

function removeAllCreatures() -- Remove all creatures from our binary tree
	removeCreature(false, true)
return true
end

function removeCreature(creature, all) -- Remove a single creature or all
	if all then
		binaryTree = {}
		lastBattleButtonSwitched = nil
		for i, v in pairs(battleButtons) do
			v.creature:hideStaticSquare() -- Is this correct?
			v:destroy()
		end
		battleButtons = {}
		return true
	end

	local creatureId = creature:getId()
	local battleButton = battleButtons[creatureId]

	if battleButton then
		if lastBattleButtonSwitched == battleButton then
			lastBattleButtonSwitched = nil
		end

		local sortType = getSortType()
		local valuetoSearch = getAttributeByOrderType(battleButton, sortType) --Search for the current ordered attribute to get O(log2(N))
		assert(valuetoSearch, "Could not find information (data) in sent battleButton")
		valuetoSearch.id = creatureId

		local index = binarySearch(binaryTree, valuetoSearch, BSComparatorSortType, sortType, creatureId)
		if index ~= nil and creatureId == binaryTree[index].id then -- Safety first :)
			local creatureListSize = #binaryTree
			if index < creatureListSize then
				for i = index, creatureListSize - 1 do
					swap(i, i+1)
				end
			end
			binaryTree[creatureListSize] = nil
			battleButton.creature:hideStaticSquare()
			battleButton:destroy()
			battleButtons[creatureId] = nil
			return true
		else
			local msg = ""
			for i, p in pairs(valuetoSearch) do msg = msg..p end
			assert(index ~= nil, "Not able to remove creature: id ".. creatureId .." not found in binary search using ".. sortType .." to find value ".. msg ..".")
		end
	end
	return false
end

-- Hide/Show Filter Options
function isHidingFilters() -- Return true if filters are hidden
	local settings = g_settings.getNode('BattleList')
	if not settings then
		return false
	end
	return settings['hidingFilters']
end

function setHidingFilters(state) -- Setting hiding filters
	settings = {}
	settings['hidingFilters'] = state
	g_settings.mergeNode('BattleList', settings)
end

function hideFilterPanel() -- Hide Filter panel
	filterPanel.originalHeight = filterPanel:getHeight()
	filterPanel:setHeight(0)
	toggleFilterButton:getParent():setMarginTop(0)
	toggleFilterButton:setImageClip(torect("0 0 21 12"))
	setHidingFilters(true)
	filterPanel:setVisible(false)
end

function showFilterPanel() -- Show Filter panel
	toggleFilterButton:getParent():setMarginTop(5)
	filterPanel:setHeight(filterPanel.originalHeight)
	toggleFilterButton:setImageClip(torect("21 0 21 12"))
	setHidingFilters(false)
	filterPanel:setVisible(true)
end

function toggleFilterPanel() -- Switching modes of filter panel (hide/show)
	if filterPanel:isVisible() then
		hideFilterPanel()
	else
		showFilterPanel()
	end
end

-- Connector Callbacks
function onAttack(creature) -- Update battleButton once you're attacking a target
	local battleButton = creature and battleButtons[creature:getId()] or lastBattleButtonSwitched
	if battleButton then
		battleButton.isTarget = creature and true or false
		updateBattleButton(battleButton)
	end
end

function onFollow(creature) -- Update battleButton once you're following a target
	local battleButton = creature and battleButtons[creature:getId()] or lastBattleButtonSwitched
	if battleButton then
		battleButton.isFollowed = creature and true or false
		updateBattleButton(battleButton)
	end
end

function onCreatureOutfitChange(creature, outfit, oldOutfit) -- Insert/Remove creature when it becomes visible/invisible
	local battleButton = battleButtons[creature:getId()]
	local fit = doCreatureFitFilters(creature)

	if battleButton ~= nil and not fit then
		removeCreature(creature)
	elseif battleButton == nil and fit then
		addCreature(creature, getSortType())
	end
end

function updateCreatureSkull(creature, skullId) -- Update skull
	local battleButton = battleButtons[creature:getId()]

	if battleButton then
		battleButton:updateSkull(skullId)
	end
end

function updateCreatureEmblem(creature, emblemId) -- Update emblem
	local battleButton = battleButtons[creature:getId()]

	if battleButton then
		battleButton:updateEmblem(emblemId)
	end
end

function onCreaturePositionChange(creature, newPos, oldPos) -- Update battleButton once you or monsters move
	local sortType = getSortType()
	-- If it's the local player moving
	if creature:isLocalPlayer() then
		if oldPos and newPos and newPos.z ~= oldPos.z then
			checkCreatures() -- Changing the z makes easier to just recalculate everything
		elseif oldPos and newPos and (newPos.x ~= oldPos.x or newPos.y ~= oldPos.y) then
			-- Distance will change when moving, recalculate and move to correct index
			if #binaryTree > 0 and sortType == 'distance' then
				-- TODO: If the amount of creatures is higher than a given number, instead of using this approach we simply recalculate each 200ms.
				for i, v in ipairs(binaryTree) do
					local oldDistance = v.distance
					local battleButton = battleButtons[v.id]
					local mob = battleButton.creature or g_map.getCreatureById(v.id)
					local newDistance = getDistanceBetween(newPos, mob:getPosition())
					if oldDistance ~= newDistance then
						v.distance = newDistance
						battleButton.data.distance = newDistance
					end
				end
				table.sort(binaryTree, function(a, b) return BSComparatorSortType(a, b, 'distance', true) == 1 end)
				correctBattleButtons()
			end

			for i, v in pairs(battleButtons) do
				local mob = v.creature
				if mob and mob:getPosition() then
					v:setVisible(creature:hasSight(mob:getPosition()) and mob:canBeSeen())
				end
			end
			--battlePanel:getLayout():update()
		end
	else
		-- If it's a creature moving
		local creatureId = creature:getId()
		local battleButton = battleButtons[creatureId]
		local fit = doCreatureFitFilters(creature)

		if battleButton ~= nil then
			if not fit and newPos then -- if there's no newPos the creature is dead, let onCreatureDisappear handles that.
				removeCreature(creature)
			elseif fit then
				if oldPos and newPos and (newPos.x ~= oldPos.x or newPos.y ~= oldPos.y) then
					if sortType == 'distance' then
						local localPlayer = g_game.getLocalPlayer()
						local newDistance = getDistanceBetween(localPlayer:getPosition(), newPos)
						local oldDistance = battleButton.data.distance

						local index = binarySearch(binaryTree, {distance = oldDistance, id = creatureId}, BSComparatorSortType, 'distance', true)
						if index ~= nil and creatureId == binaryTree[index].id then -- Safety first :)
							binaryTree[index].distance = newDistance
							battleButton.data.distance = newDistance
							if newDistance > oldDistance then
								if index < #binaryTree then
									for i = index, #binaryTree - 1 do
										local a = binaryTree[i]
										local b = binaryTree[i + 1]
										if a.distance > b.distance or (a.distance == b.distance and a.id > b.id) then
											swap(i, i+1)
										end
									end
								end
							elseif newDistance < oldDistance then
								battleButton:setVisible(localPlayer:hasSight(newPos) and creature:canBeSeen())
								--battlePanel:getLayout():update()
								if index > 1 then
									for i = index, 2, -1 do
										local a = binaryTree[i-1]
										local b = binaryTree[i]
										if a.distance > b.distance or (a.distance == b.distance and a.id > b.id) then
											swap(i-1, i)
										end
									end
								end
							end
							correctBattleButtons()
						else
							assert(index ~= nil, "Not able to update Position Change. Creature: ".. creature:getName() .." id ".. creatureId .." not found in binary search using ".. sortType .." to find value ".. oldDistance ..".\n")
						end
					end
				end
			end
		end
	end
end

function onCreatureHealthPercentChange(creature, healthPercent, oldHealthPercent) -- Update battleButton mobs lose/gain health
	local creatureId = creature:getId()
	local battleButton = battleButtons[creatureId]
	if battleButton then
		local sortType = getSortType()
		if sortType == 'health' then
			if healthPercent == oldHealthPercent then return false end -- Sanity Check
			if healthPercent == 0 then return false end -- if healthpercent is 0 the creature is dead, let onCreatureDisappear handles that.

			local index = binarySearch(binaryTree, {healthpercent = oldHealthPercent, id = creatureId}, BSComparatorSortType, 'health', true)
			if index ~= nil and creatureId == binaryTree[index].id then -- Safety first :)
				binaryTree[index].healthpercent = healthPercent
				battleButton.data.healthpercent = healthPercent
				if healthPercent > oldHealthPercent then -- Check if health is positive or negative to update it more efficently.
					if index < #binaryTree then
						for i = index, #binaryTree - 1 do
							local a = binaryTree[i]
							local b = binaryTree[i + 1]
							if a.healthpercent > b.healthpercent or (a.healthpercent == b.healthpercent and a.id > b.id) then
								swap(i, i+1)
							end
						end
					end
				else
					if index > 1 then
						for i = index, 2, -1 do
							local a = binaryTree[i-1]
							local b = binaryTree[i]
							if a.healthpercent > b.healthpercent or (a.healthpercent == b.healthpercent and a.id > b.id) then
								swap(i-1, i)
							end
						end
					end
				end
				correctBattleButtons()
			else
				assert(index ~= nil, "Not able to update HealthPercent Change. Creature: id ".. creatureId .." not found in binary search using ".. sortType .." to find value ".. oldHealthPercent ..".")
			end

		end
		battleButton:setLifeBarPercent(healthPercent)
	end
end

function onCreatureAppear(creature) -- Update battleButton once a creature appear (add)
	if creature:isLocalPlayer() then
		addEvent(function()
			updateStaticSquare()
		end)
	end

	local sortType = getSortType()

	if doCreatureFitFilters(creature) then
		addCreature(creature, sortType)
	end
end

function onCreatureDisappear(creature) -- Update battleButton once a creature disappear (remove/dead)
	removeCreature(creature)
end

-- BattleWindow controllers
function onBattleButtonMouseRelease(self, mousePosition, mouseButton) -- Interactions with mouse (right, left, right + left and shift interactions)
	if mouseWidget.cancelNextRelease then
		mouseWidget.cancelNextRelease = false
		return false
	end

	if ((g_mouse.isPressed(MouseLeftButton) and mouseButton == MouseRightButton) or (g_mouse.isPressed(MouseRightButton) and mouseButton == MouseLeftButton)) then
		mouseWidget.cancelNextRelease = true
		g_game.look(self.creature, true)
		return true
	elseif mouseButton == MouseLeftButton and g_keyboard.isShiftPressed() then
		g_game.look(self.creature, true)
		return true
	elseif mouseButton == MouseRightButton and not g_mouse.isPressed(MouseLeftButton) then
		modules.game_interface.createThingMenu(mousePosition, nil, nil, self.creature)
		return true
	elseif mouseButton == MouseLeftButton and not g_mouse.isPressed(MouseRightButton) then
		if self.isTarget then
			g_game.cancelAttack()
		else
			g_game.attack(self.creature)
		end
		return true
	end
	return false
end

function updateStaticSquare(battleButton) -- Update all static squares upon appearing the screen (login)
	for _, battleButton in pairs(battleButtons) do
		if battleButton.isTarget then
			battleButton:update()
		end
	end
end

function updateBattleButton(battleButton) -- Update battleButton with attack/follow squares
	battleButton:update()
	if battleButton.isTarget or battleButton.isFollowed then
		-- set new last battle button switched
		if lastBattleButtonSwitched and lastBattleButtonSwitched ~= battleButton then
			lastBattleButtonSwitched.isTarget = false
			lastBattleButtonSwitched.isFollowed = false
			updateBattleButton(lastBattleButtonSwitched)
		end
		lastBattleButtonSwitched = battleButton
	end
end

function onBattleButtonHoverChange(battleButton, hovered) -- Interaction with mouse (hovering)
	if battleButton.isBattleButton then
		battleButton.isHovered = hovered
		updateBattleButton(battleButton)
	end
end

function toggle() -- Close/Open the battle window or Pressing Ctrl + B
	if battleButton:isOn() then
		battleWindow:close()
		battleButton:setOn(false)
		-- TODO: Not working to disconnect when closing module window
		disconnecting()
	else
		battleWindow:open()
		battleButton:setOn(true)
		connecting()
	end
end

function terminate() -- Terminating the Module (unload)
	binaryTree = {}
	battleButtons = {}

	battleButton:destroy()
	battleWindow:destroy()
	mouseWidget:destroy()
	g_keyboard.unbindKeyDown('Ctrl+B')

	-- Removing the connectors
	disconnecting()
end

function onMiniWindowClose() -- Callback when we close the module window
	battleButton:setOn(false)
end