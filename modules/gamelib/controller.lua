Controller = {
	events = nil,
	externalEvents = nil,
	widgets = nil,
	keyboardEvents = nil,
	attributes = nil,
	opcodes = nil
}

function Controller:new()
	local obj = {events = {}, externalEvents = {}, widgets = {}, keyboardEvents = {}, attributes = {}, opcodes = {}}
	setmetatable(obj, self)
	self.__index = self
	return obj
end

function Controller:init()
	if self.onInit then
		self:onInit()
	end

	local gameEvent = self.events.game
	if gameEvent then
		connect(g_game, gameEvent)
		if gameEvent.onGameStart and g_game.isOnline() then
			gameEvent.onGameStart()
		end
	end
end

function Controller:terminate()
	if self.onTerminate then
		self:onTerminate()
	end

	local gameEvent = self.events.game
	if gameEvent then
		disconnect(g_game, gameEvent)
		if gameEvent.onGameEnd and g_game.isOnline() then
			gameEvent.onGameEnd()
		end
		self.events.game = nil
	end

	for actor, events in pairs(self.events) do
		disconnect(actor, events)
	end

	for name, widget in pairs(self.widgets) do
		widget:destroy()
	end

	for i, event in pairs(self.keyboardEvents) do
		g_keyboard['unbind' .. event.name](event.args)
	end

	for i, opcode in pairs(self.opcodes) do
		ProtocolGame.unregisterExtendedOpcode(opcode)
	end

	self:disconnectExternalEvents()

	self.events = nil
	self.widgets = nil
	self.keyboardEvents = nil
	self.attributes = nil
	self.opcodes = nil
	self.externalEvents = nil
end

function Controller:gameEvent(name, callback)
	local gameEvent = self.events.game
	if not gameEvent then
		gameEvent = {}
		self.events.game = gameEvent
	end

	gameEvent[name] = callback
end

function Controller:onGameStart(callback)
	self:gameEvent('onGameStart', callback)
end

function Controller:onGameEnd(callback)
	self:gameEvent('onGameEnd', callback)
end

function Controller:attachExternalEvent(event)
	table.insert(self.externalEvents, event)
end

function Controller:connectExternalEvents()
	for i, event in pairs(self.externalEvents) do
		event:connect()
	end
end

function Controller:disconnectExternalEvents()
	for i, event in pairs(self.externalEvents) do
		event:disconnect()
	end
end

function Controller:registerEvents(actor, events)
	self.events[actor] = events
end

function Controller:registerWidget(name, widget)
	self.widgets[name] = widget
end

function Controller:getWidget(name)
	return self.widgets[name]
end

function Controller:connectEvents(actor, events)
	if type(actor) == 'table' then
		for _, target in pairs(actor) do
			self:connectEvents(target, events)
		end
		return
	end

	if not events then
		events = self.events[actor]
	else
		self.events[actor] = events
	end

	assert(events ~= nil, 'Events are empty')
	connect(actor, events)
end

function Controller:disconnectEvents(actor, destroy)
	if type(actor) == 'table' then
		for _, target in pairs(actor) do
			self:disconnectEvents(target, destroy)
		end
		return
	end

	local events = self.events[actor]
	if events then
		disconnect(actor, events)
		if destroy ~= false then
			self.events[actor] = nil
		end
	end
end

function Controller:registerExtendedOpcode(opcode, fnc)
	ProtocolGame.registerExtendedOpcode(opcode, fnc)
	table.insert(self.opcodes, opcode)
end

function Controller:sendExtendedOpcode(opcode, ...)
	local protocol = g_game.getProtocolGame()
	if protocol then
		protocol:sendExtendedOpcode(opcode, ...)
	end
end

function Controller:setAttribute(name, value)
	self.attributes[name] = value
end

function Controller:getAttribute(name)
	return self.attributes[name]
end

function Controller:bindKeyDown(...)
	table.insert(self.keyboardEvents, {name = 'KeyDown', args = ...})
	g_keyboard.bindKeyDown(...)
end

function Controller:bindKeyUp(...)
	table.insert(self.keyboardEvents, {name = 'KeyUp', args = ...})
	g_keyboard.bindKeyUp(...)
end

function Controller:bindKeyPress(...)
	table.insert(self.keyboardEvents, {name = 'KeyPress', args = ...})
	g_keyboard.bindKeyPress(...)
end
