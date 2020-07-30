local minimapWidget = nil
local otmm = true
local fullmapView = false
local oldZoom = nil
local oldPos = nil

local function updateCameraPosition()
	local player = g_game.getLocalPlayer()
	if not player then
		return
	end

	local pos = player:getPosition()
	if not pos then
		return
	end

	if minimapWidget:isDragging() then
		return
	end

	if not fullmapView then
		minimapWidget:setCameraPosition(pos)
	end

	minimapWidget:setCrossPosition(pos)
end

local function toggle()
	local minimapWindow = controller.widgets.minimapWindow

	if controller.widgets.minimapButton:isOn() then
		minimapWindow:close()
	else
		minimapWindow:open()
	end
end

local function toggleFullMap()
	local minimapWindow = controller.widgets.minimapWindow

	if fullmapView then
		minimapWidget:setParent(minimapWindow:getChildById('contentsPanel'))
		minimapWidget:fill('parent')
		minimapWindow:show()
	else
		minimapWindow:hide()
		minimapWidget:setParent(modules.game_interface.getRootPanel())
		minimapWidget:fill('parent')
	end

	fullmapView = not fullmapView
	minimapWidget:setAlternativeWidgetsVisible(fullmapView)

	local zoom = oldZoom or 0
	local pos = oldPos or minimapWidget:getCameraPosition()
	oldZoom = minimapWidget:getZoom()
	oldPos = minimapWidget:getCameraPosition()
	minimapWidget:setZoom(zoom)
	minimapWidget:setCameraPosition(pos)
end

local localPlayerEvent = EventController:new(LocalPlayer, {onPositionChange = updateCameraPosition})

controller = Controller:new()
controller:attachExternalEvent(localPlayerEvent)

function controller:onInit()
	local minimapButton =
		modules.client_topmenu.addRightGameToggleButton('minimapButton', tr('Minimap') .. ' (Ctrl+M)', '/images/topbuttons/minimap', toggle)
	minimapButton:setOn(true)

	local minimapWindow = g_ui.loadUI('minimap', modules.game_interface.getRightPanel())
	minimapWindow:setContentMinimumHeight(80)

	minimapWidget = minimapWindow:recursiveGetChildById('minimap')

	local gameRootPanel = modules.game_interface.getRootPanel()
	self:bindKeyPress('Alt+Left', function() minimapWidget:move(1, 0)	end, gameRootPanel)
	self:bindKeyPress('Alt+Right', function() minimapWidget:move(-1, 0) end, gameRootPanel)
	self:bindKeyPress('Alt+Up', function() minimapWidget:move(0, 1) end, gameRootPanel)
	self:bindKeyPress('Alt+Down', function() minimapWidget:move(0, -1) end,	gameRootPanel)

	self:bindKeyDown('Ctrl+M', toggle)
	self:bindKeyDown('Ctrl+Shift+M', toggleFullMap)

	self:registerWidget('minimapButton', minimapButton)
	self:registerWidget('minimapWindow', minimapWindow)

	minimapWindow:setup()
	localPlayerEvent:connect()
end

controller:onGameStart(function()
		-- Load Map
		g_minimap.clean()

		local minimapFile = '/minimap'
		local loadFnc = nil

		if otmm then
			minimapFile = minimapFile .. '.otmm'
			loadFnc = g_minimap.loadOtmm
		else
			minimapFile = minimapFile .. '_' .. g_game.getClientVersion() .. '.otcm'
			loadFnc = g_map.loadOtcm
		end

		if g_resources.fileExists(minimapFile) then
			loadFnc(minimapFile)
		end

		minimapWidget:load()
end)

controller:onGameEnd(function()
		-- Save Map
		if otmm then
			g_minimap.saveOtmm('/minimap.otmm')
		else
			g_map.saveOtcm('/minimap_' .. g_game.getClientVersion() .. '.otcm')
		end

		minimapWidget:save()
end)

function onMiniWindowOpen()
	controller.widgets.minimapButton:setOn(true)
	localPlayerEvent:connect()
	localPlayerEvent:execute('onPositionChange')
end

function onMiniWindowClose()
	controller.widgets.minimapButton:setOn(false)
	localPlayerEvent:disconnect()
end
