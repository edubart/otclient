HOTKEY_MANAGER_USE = nil
HOTKEY_MANAGER_USEONSELF = 1
HOTKEY_MANAGER_USEONTARGET = 2
HOTKEY_MANAGER_USEWITH = 3

HOTKEY_ACTION_TOGGLE_WASD = 1
HOTKEY_ACTION_ATTACK_NEXT = 2
HOTKEY_ACTION_ATTACK_PREV = 3
HOTKEY_ACTION_TOGGLE_CHASE = 4

HotkeyActions = {{
    id = HOTKEY_ACTION_TOGGLE_WASD,
    text = tr('Toggle WASD chat mode')
}, {
    id = HOTKEY_ACTION_ATTACK_NEXT,
    text = tr('Attack next creature in battle list')
}, {
    id = HOTKEY_ACTION_ATTACK_PREV,
    text = tr('Attack previous creature in battle list')
}, {
    id = HOTKEY_ACTION_TOGGLE_CHASE,
    text = tr('Toggle chase mode')
}}

HotkeyColors = {
    text = '#888888',
    textAutoSend = '#FFFFFF',
    itemUse = '#8888FF',
    itemUseSelf = '#00FF00',
    itemUseTarget = '#FF0000',
    itemUseWith = '#F5B325',
    action = '#F97ACD'
}

hotkeysManagerLoaded = false
hotkeysWindow = nil
hotkeysButton = nil
currentHotkeyLabel = nil
currentItemPreview = nil
itemWidget = nil
addHotkeyButton = nil
removeHotkeyButton = nil
hotkeyActionCombo = nil
hotkeyText = nil
hotKeyTextLabel = nil
sendAutomatically = nil
selectObjectButton = nil
clearObjectButton = nil
useOnSelf = nil
useOnTarget = nil
useWith = nil
defaultComboKeys = nil
perServer = true
perCharacter = true
mouseGrabberWidget = nil
useRadioGroup = nil
currentHotkeys = nil
boundCombosCallback = {}
hotkeysList = {}
disableHotkeysCount = 0
lastHotkeyTime = g_clock.millis()

-- public functions
function init()
    hotkeysButton = modules.client_topmenu.addLeftGameButton('hotkeysButton', tr('Hotkeys') .. ' (Ctrl+K)',
                                                             '/images/topbuttons/hotkeys', toggle)
    g_keyboard.bindKeyDown('Ctrl+K', toggle)
    hotkeysWindow = g_ui.displayUI('hotkeys_manager')
    hotkeysWindow:setVisible(false)

    currentHotkeys = hotkeysWindow:getChildById('currentHotkeys')
    currentItemPreview = hotkeysWindow:getChildById('itemPreview')
    addHotkeyButton = hotkeysWindow:getChildById('addHotkeyButton')
    removeHotkeyButton = hotkeysWindow:getChildById('removeHotkeyButton')
    hotkeyText = hotkeysWindow:getChildById('hotkeyText')
    hotKeyTextLabel = hotkeysWindow:getChildById('hotKeyTextLabel')
    sendAutomatically = hotkeysWindow:getChildById('sendAutomatically')
    selectObjectButton = hotkeysWindow:getChildById('selectObjectButton')
    clearObjectButton = hotkeysWindow:getChildById('clearObjectButton')
    useOnSelf = hotkeysWindow:getChildById('useOnSelf')
    useOnTarget = hotkeysWindow:getChildById('useOnTarget')
    useWith = hotkeysWindow:getChildById('useWith')

    useRadioGroup = UIRadioGroup.create()
    useRadioGroup:addWidget(useOnSelf)
    useRadioGroup:addWidget(useOnTarget)
    useRadioGroup:addWidget(useWith)
    useRadioGroup.onSelectionChange = function(self, selected) onChangeUseType(selected) end

    hotkeyActionCombo = hotkeysWindow:getChildById('hotkeyActionCombo')

    hotkeyActionCombo:addOption('None', 0)
    for _, action in pairs(HotkeyActions) do hotkeyActionCombo:addOption(action.text, action.id) end

    hotkeyActionCombo.onOptionChange = onActionChange

    mouseGrabberWidget = g_ui.createWidget('UIWidget')
    mouseGrabberWidget:setVisible(false)
    mouseGrabberWidget:setFocusable(false)
    mouseGrabberWidget.onMouseRelease = onChooseItemMouseRelease

    currentHotkeys.onChildFocusChange = function(self, hotkeyLabel) onSelectHotkeyLabel(hotkeyLabel) end
    g_keyboard.bindKeyPress('Down', function() currentHotkeys:focusNextChild(KeyboardFocusReason) end, hotkeysWindow)
    g_keyboard.bindKeyPress('Up', function() currentHotkeys:focusPreviousChild(KeyboardFocusReason) end, hotkeysWindow)

    connect(g_game, {
        onGameStart = online,
        onGameEnd = offline
    })

    load()
end

function terminate()
    disconnect(g_game, {
        onGameStart = online,
        onGameEnd = offline
    })

    g_keyboard.unbindKeyDown('Ctrl+K')

    unload()

    hotkeysWindow:destroy()
    hotkeysButton:destroy()
    mouseGrabberWidget:destroy()
    hotkeysWindow = nil
    hotkeysButton = nil
    hotkeyActionCombo = nil
    hotKeyTextLabel = nil
    hotkeyText = nil
    sendAutomatically = nil
    selectObjectButton = nil
    clearObjectButton = nil
    mouseGrabberWidget = nil
    addHotkeyButton = nil
    removeHotkeyButton = nil
    itemPreview = nil
    useOnSelf = nil
    useOnTarget = nil
    useWith = nil
    currentHotkeys = nil
end

function configure(savePerServer, savePerCharacter)
    perServer = savePerServer
    perCharacter = savePerCharacter
    reload()
end

function online()
    reload()
    hide()
end

function offline()
    unload()
    hide()
end

function show()
    if not g_game.isOnline() then return end
    hotkeysWindow:show()
    hotkeysWindow:raise()
    hotkeysWindow:focus()
end

function hide() hotkeysWindow:hide() end

function toggle()
    if not hotkeysWindow:isVisible() then
        show()
    else
        hide()
    end
end

function ok()
    save()
    hide()
end

function cancel()
    reload()
    hide()
end

function load(forceDefaults)
    hotkeysManagerLoaded = false

    local hotkeySettings = g_settings.getNode('game_hotkeys')
    local hotkeys = {}

    if not table.empty(hotkeySettings) then hotkeys = hotkeySettings end
    if perServer and not table.empty(hotkeys) then hotkeys = hotkeys[G.host] end
    if perCharacter and not table.empty(hotkeys) then hotkeys = hotkeys[g_game.getCharacterName()] end

    hotkeyList = {}
    if not forceDefaults then
        if not table.empty(hotkeys) then
            for keyCombo, setting in pairs(hotkeys) do
                keyCombo = tostring(keyCombo)
                addKeyCombo(keyCombo, setting)
                hotkeyList[keyCombo] = setting
            end
        end
    end

    if currentHotkeys:getChildCount() == 0 then loadDefautComboKeys() end

    hotkeysManagerLoaded = true
end

function unload()
    for keyCombo, callback in pairs(boundCombosCallback) do g_keyboard.unbindKeyPress(keyCombo, callback) end
    boundCombosCallback = {}
    currentHotkeys:destroyChildren()
    currentHotkeyLabel = nil
    updateHotkeyForm(true)
    hotkeyList = {}
end

function reset()
    unload()
    load(true)
end

function reload()
    unload()
    load()
end

function save()
    local hotkeySettings = g_settings.getNode('game_hotkeys') or {}
    local hotkeys = hotkeySettings

    if perServer then
        if not hotkeys[G.host] then hotkeys[G.host] = {} end
        hotkeys = hotkeys[G.host]
    end

    if perCharacter then
        local char = g_game.getCharacterName()
        if not hotkeys[char] then hotkeys[char] = {} end
        hotkeys = hotkeys[char]
    end

    table.clear(hotkeys)

    for _, child in pairs(currentHotkeys:getChildren()) do
        hotkeys[child.keyCombo] = {
            autoSend = child.autoSend,
            itemId = child.itemId,
            subType = child.subType,
            useType = child.useType,
            value = child.value,
            action = child.action
        }
    end

    hotkeyList = hotkeys
    g_settings.setNode('game_hotkeys', hotkeySettings)
    g_settings.save()
end

function loadDefautComboKeys()
    if not defaultComboKeys then
        for i = 1, 12 do addKeyCombo('F' .. i) end
        for i = 1, 4 do addKeyCombo('Shift+F' .. i) end
    else
        for keyCombo, keySettings in pairs(defaultComboKeys) do addKeyCombo(keyCombo, keySettings) end
    end
end

function setDefaultComboKeys(combo) defaultComboKeys = combo end

function onActionChange(comboBox, option)
    local action = comboBox:getCurrentOption().data
    if currentHotkeyLabel then
        if action > 0 then
            currentHotkeyLabel.action = action
            currentHotkeyLabel.itemId = nil
            currentHotkeyLabel.value = nil
            currentHotkeyLabel.autoSend = nil
        else
            currentHotkeyLabel.action = nil
        end
        updateHotkeyLabel(currentHotkeyLabel)
        updateHotkeyForm(true, true)
    end
end

function onChooseItemMouseRelease(self, mousePosition, mouseButton)
    local item = nil
    if mouseButton == MouseLeftButton then
        local clickedWidget = modules.game_interface.getRootPanel():recursiveGetChildByPos(mousePosition, false)
        if clickedWidget then
            if clickedWidget:getClassName() == 'UIGameMap' then
                local tile = clickedWidget:getTile(mousePosition)
                if tile then
                    local thing = tile:getTopMoveThing()
                    if thing and thing:isItem() then item = thing end
                end
            elseif clickedWidget:getClassName() == 'UIItem' and not clickedWidget:isVirtual() then
                item = clickedWidget:getItem()
            end
        end
    end

    if item and currentHotkeyLabel then
        currentHotkeyLabel.itemId = item:getId()
        if item:isFluidContainer() then currentHotkeyLabel.subType = item:getSubType() end
        if item:isMultiUse() then
            currentHotkeyLabel.useType = HOTKEY_MANAGER_USEWITH
        else
            currentHotkeyLabel.useType = HOTKEY_MANAGER_USE
        end
        currentHotkeyLabel.value = nil
        currentHotkeyLabel.autoSend = false
        updateHotkeyLabel(currentHotkeyLabel)
        updateHotkeyForm(true)
    end

    show()

    g_mouse.popCursor('target')
    self:ungrabMouse()
    return true
end

function startChooseItem()
    if g_ui.isMouseGrabbed() then return end
    mouseGrabberWidget:grabMouse()
    g_mouse.pushCursor('target')
    hide()
end

function clearObject()
    currentHotkeyLabel.itemId = nil
    currentHotkeyLabel.subType = nil
    currentHotkeyLabel.useType = nil
    currentHotkeyLabel.autoSend = nil
    currentHotkeyLabel.value = nil
    currentHotkeyLabel.action = nil
    updateHotkeyLabel(currentHotkeyLabel)
    updateHotkeyForm(true)
end

function addHotkey()
    local assignWindow = g_ui.createWidget('HotkeyAssignWindow', rootWidget)
    assignWindow:grabKeyboard()

    local comboLabel = assignWindow:getChildById('comboPreview')
    comboLabel.keyCombo = ''
    assignWindow.onKeyDown = hotkeyCapture
end

function addKeyCombo(keyCombo, keySettings, focus)
    if keyCombo == nil or #keyCombo == 0 then return end
    if not keyCombo then return end
    local hotkeyLabel = currentHotkeys:getChildById(keyCombo)
    if not hotkeyLabel then
        hotkeyLabel = g_ui.createWidget('HotkeyListLabel')
        hotkeyLabel:setId(keyCombo)

        local children = currentHotkeys:getChildren()
        children[#children + 1] = hotkeyLabel
        table.sort(children, function(a, b)
            if a:getId():len() < b:getId():len() then
                return true
            elseif a:getId():len() == b:getId():len() then
                return a:getId() < b:getId()
            else
                return false
            end
        end)
        for i = 1, #children do
            if children[i] == hotkeyLabel then
                currentHotkeys:insertChild(i, hotkeyLabel)
                break
            end
        end

        if keySettings then
            currentHotkeyLabel = hotkeyLabel
            hotkeyLabel.keyCombo = keyCombo
            hotkeyLabel.autoSend = toboolean(keySettings.autoSend)
            hotkeyLabel.itemId = tonumber(keySettings.itemId)
            hotkeyLabel.subType = tonumber(keySettings.subType)
            hotkeyLabel.useType = tonumber(keySettings.useType)
            hotkeyLabel.action = tonumber(keySettings.action)
            if keySettings.value then hotkeyLabel.value = tostring(keySettings.value) end
        else
            hotkeyLabel.keyCombo = keyCombo
            hotkeyLabel.autoSend = false
            hotkeyLabel.itemId = nil
            hotkeyLabel.subType = nil
            hotkeyLabel.useType = nil
            hotkeyLabel.action = nil
            hotkeyLabel.value = ''
        end

        updateHotkeyLabel(hotkeyLabel)

        boundCombosCallback[keyCombo] = function() doKeyCombo(keyCombo) end
        g_keyboard.bindKeyPress(keyCombo, boundCombosCallback[keyCombo])
    end

    if focus then
        currentHotkeys:focusChild(hotkeyLabel)
        currentHotkeys:ensureChildVisible(hotkeyLabel)
        updateHotkeyForm(true)
    end
end

function doKeyCombo(keyCombo)
    if not g_game.isOnline() then return end
    if not canPerformKeyCombo(keyCombo) then return end
    local hotKey = hotkeyList[keyCombo]
    if not hotKey then return end

    if g_clock.millis() - lastHotkeyTime < modules.client_options.getOption('hotkeyDelay') then return end
    lastHotkeyTime = g_clock.millis()

    if hotKey.action then
        if hotKey.action == HOTKEY_ACTION_TOGGLE_WASD then
            modules.game_console.toggleChat()
        elseif hotKey.action == HOTKEY_ACTION_ATTACK_NEXT then
            modules.game_battle.attackNext()
        elseif hotKey.action == HOTKEY_ACTION_ATTACK_PREV then
            modules.game_battle.attackNext(true)
        elseif hotKey.action == HOTKEY_ACTION_TOGGLE_CHASE then
            modules.game_combatcontrols.toggleChaseMode()
        end

    elseif hotKey.itemId == nil then
        if not hotKey.value or #hotKey.value == 0 then return end
        if hotKey.autoSend then
            modules.game_console.sendMessage(hotKey.value)
        else
            scheduleEvent(function()
                if not modules.game_console.isChatEnabled() then modules.game_console.switchChatOnCall() end
                modules.game_console.setTextEditText(hotKey.value)
            end, 1)
        end
    else
        executeHotkeyItem(hotKey.useType, hotKey.itemId, hotKey.subType)
    end
end

function executeHotkeyItem(action, itemId, subType)
    if action == HOTKEY_MANAGER_USE then
        if g_game.getClientVersion() < 780 or subType then
            local item = g_game.findPlayerItem(itemId, subType or -1)
            if item then g_game.use(item) end
        else
            g_game.useInventoryItem(itemId)
        end
    elseif action == HOTKEY_MANAGER_USEONSELF then
        if g_game.getClientVersion() < 780 or subType then
            local item = g_game.findPlayerItem(itemId, subType or -1)
            if item then g_game.useWith(item, g_game.getLocalPlayer()) end
        else
            g_game.useInventoryItemWith(itemId, g_game.getLocalPlayer())
        end
    elseif action == HOTKEY_MANAGER_USEONTARGET then
        local attackingCreature = g_game.getAttackingCreature()
        if not attackingCreature then
            local item = Item.create(itemId)
            if g_game.getClientVersion() < 780 or subType then
                local tmpItem = g_game.findPlayerItem(itemId, subType or -1)
                if not tmpItem then return end
                item = tmpItem
            end

            modules.game_interface.startUseWith(item)
            return
        end

        if not attackingCreature:getTile() then return end
        if g_game.getClientVersion() < 780 or subType then
            local item = g_game.findPlayerItem(itemId, subType or -1)
            if item then g_game.useWith(item, attackingCreature) end
        else
            g_game.useInventoryItemWith(itemId, attackingCreature)
        end
    elseif action == HOTKEY_MANAGER_USEWITH then
        local item = Item.create(itemId)
        if g_game.getClientVersion() < 780 or subType then
            local tmpItem = g_game.findPlayerItem(itemId, subType or -1)
            if not tmpItem then return true end
            item = tmpItem
        end
        modules.game_interface.startUseWith(item)
    end
end

function updateHotkeyLabel(hotkeyLabel)
    if not hotkeyLabel then return end
    if hotkeyLabel.useType == HOTKEY_MANAGER_USEONSELF then
        hotkeyLabel:setText(tr('%s: (use object on yourself)', hotkeyLabel.keyCombo))
        hotkeyLabel:setColor(HotkeyColors.itemUseSelf)
    elseif hotkeyLabel.useType == HOTKEY_MANAGER_USEONTARGET then
        hotkeyLabel:setText(tr('%s: (use object on target)', hotkeyLabel.keyCombo))
        hotkeyLabel:setColor(HotkeyColors.itemUseTarget)
    elseif hotkeyLabel.useType == HOTKEY_MANAGER_USEWITH then
        hotkeyLabel:setText(tr('%s: (use object with crosshair)', hotkeyLabel.keyCombo))
        hotkeyLabel:setColor(HotkeyColors.itemUseWith)
    elseif hotkeyLabel.itemId ~= nil then
        hotkeyLabel:setText(tr('%s: (use object)', hotkeyLabel.keyCombo))
        hotkeyLabel:setColor(HotkeyColors.itemUse)
    elseif hotkeyLabel.action then
        for _, action in pairs(HotkeyActions) do
            if action.id == hotkeyLabel.action then
                hotkeyLabel:setText(tr('%s: ' .. action.text, hotkeyLabel.keyCombo))
                break
            end
        end
        hotkeyLabel:setColor(HotkeyColors.action)
    else
        local text = hotkeyLabel.keyCombo .. ': '
        if hotkeyLabel.value then text = text .. hotkeyLabel.value end
        hotkeyLabel:setText(text)
        if hotkeyLabel.autoSend then
            hotkeyLabel:setColor(HotkeyColors.autoSend)
        else
            hotkeyLabel:setColor(HotkeyColors.text)
        end
    end
end

function updateHotkeyForm(reset, dontUpdateCombo)
    if currentHotkeyLabel then
        removeHotkeyButton:enable()
        if currentHotkeyLabel.itemId ~= nil then
            if not dontUpdateCombo then hotkeyActionCombo:setCurrentIndex(1) end
            hotkeyActionCombo:disable()
            hotkeyText:clearText()
            hotkeyText:disable()
            hotKeyTextLabel:disable()
            sendAutomatically:setChecked(false)
            sendAutomatically:disable()
            selectObjectButton:disable()
            clearObjectButton:enable()
            currentItemPreview:setItemId(currentHotkeyLabel.itemId)
            if currentHotkeyLabel.subType then currentItemPreview:setItemSubType(currentHotkeyLabel.subType) end
            if currentItemPreview:getItem():isMultiUse() then
                useOnSelf:enable()
                useOnTarget:enable()
                useWith:enable()
                if currentHotkeyLabel.useType == HOTKEY_MANAGER_USEONSELF then
                    useRadioGroup:selectWidget(useOnSelf)
                elseif currentHotkeyLabel.useType == HOTKEY_MANAGER_USEONTARGET then
                    useRadioGroup:selectWidget(useOnTarget)
                elseif currentHotkeyLabel.useType == HOTKEY_MANAGER_USEWITH then
                    useRadioGroup:selectWidget(useWith)
                end
            else
                useOnSelf:disable()
                useOnTarget:disable()
                useWith:disable()
                useRadioGroup:clearSelected()
            end
        elseif currentHotkeyLabel.action then
            if not dontUpdateCombo then hotkeyActionCombo:setCurrentOptionByData(currentHotkeyLabel.action) end
            hotkeyActionCombo:enable()
            hotkeyText:clearText()
            hotkeyText:disable()
            hotKeyTextLabel:disable()
            sendAutomatically:setChecked(false)
            sendAutomatically:disable()
            selectObjectButton:disable()
            useOnSelf:disable()
            useOnTarget:disable()
            useWith:disable()
            useRadioGroup:clearSelected()
            selectObjectButton:disable()
            clearObjectButton:disable()
        else
            if not dontUpdateCombo then hotkeyActionCombo:setCurrentIndex(1) end
            hotkeyActionCombo:enable()
            useOnSelf:disable()
            useOnTarget:disable()
            useWith:disable()
            useRadioGroup:clearSelected()
            hotkeyText:enable()
            hotkeyText:focus()
            hotKeyTextLabel:enable()
            if reset then hotkeyText:setCursorPos(-1) end
            hotkeyText:setText(currentHotkeyLabel.value)
            sendAutomatically:setChecked(currentHotkeyLabel.autoSend)
            sendAutomatically:setEnabled(currentHotkeyLabel.value and #currentHotkeyLabel.value > 0)
            selectObjectButton:enable()
            clearObjectButton:disable()
            currentItemPreview:clearItem()
        end
    else
        if not dontUpdateCombo then hotkeyActionCombo:setCurrentIndex(1) end
        hotkeyActionCombo:disable()
        removeHotkeyButton:disable()
        hotkeyText:disable()
        sendAutomatically:disable()
        selectObjectButton:disable()
        clearObjectButton:disable()
        useOnSelf:disable()
        useOnTarget:disable()
        useWith:disable()
        hotkeyText:clearText()
        useRadioGroup:clearSelected()
        sendAutomatically:setChecked(false)
        currentItemPreview:clearItem()
    end
end

function removeHotkey()
    if currentHotkeyLabel == nil then return end
    g_keyboard.unbindKeyPress(currentHotkeyLabel.keyCombo, boundCombosCallback[currentHotkeyLabel.keyCombo])
    boundCombosCallback[currentHotkeyLabel.keyCombo] = nil
    currentHotkeyLabel:destroy()
    currentHotkeyLabel = nil
end

function onHotkeyTextChange(value)
    if not hotkeysManagerLoaded then return end
    if currentHotkeyLabel == nil then return end
    currentHotkeyLabel.value = value
    if value == '' then currentHotkeyLabel.autoSend = false end
    updateHotkeyLabel(currentHotkeyLabel)
    updateHotkeyForm()
end

function onSendAutomaticallyChange(autoSend)
    if not hotkeysManagerLoaded then return end
    if currentHotkeyLabel == nil then return end
    if not currentHotkeyLabel.value or #currentHotkeyLabel.value == 0 then return end
    currentHotkeyLabel.autoSend = autoSend
    updateHotkeyLabel(currentHotkeyLabel)
    updateHotkeyForm()
end

function onChangeUseType(useTypeWidget)
    if not hotkeysManagerLoaded then return end
    if currentHotkeyLabel == nil then return end
    if useTypeWidget == useOnSelf then
        currentHotkeyLabel.useType = HOTKEY_MANAGER_USEONSELF
    elseif useTypeWidget == useOnTarget then
        currentHotkeyLabel.useType = HOTKEY_MANAGER_USEONTARGET
    elseif useTypeWidget == useWith then
        currentHotkeyLabel.useType = HOTKEY_MANAGER_USEWITH
    else
        currentHotkeyLabel.useType = HOTKEY_MANAGER_USE
    end
    updateHotkeyLabel(currentHotkeyLabel)
    updateHotkeyForm()
end

function onSelectHotkeyLabel(hotkeyLabel)
    currentHotkeyLabel = hotkeyLabel
    updateHotkeyForm(true)
end

function hotkeyCapture(assignWindow, keyCode, keyboardModifiers)
    local keyCombo = determineKeyComboDesc(keyCode, keyboardModifiers)
    local comboPreview = assignWindow:getChildById('comboPreview')
    comboPreview:setText(tr('Current hotkey to add: %s', keyCombo))
    comboPreview.keyCombo = keyCombo
    comboPreview:resizeToText()
    assignWindow:getChildById('addButton'):enable()
    return true
end

function hotkeyCaptureOk(assignWindow, keyCombo)
    addKeyCombo(keyCombo, nil, true)
    assignWindow:destroy()
end

function enableHotkeys(value)
    disableHotkeysCount = disableHotkeysCount + (value and -1 or 1)

    if disableHotkeysCount < 0 then disableHotkeysCount = 0; end
end

function areHotkeysDisabled() return disableHotkeysCount > 0 end

-- Even if hotkeys are enabled, only the hotkeys containing Ctrl or Alt or F1-F12 will be enabled when
-- chat is opened (no WASD mode). This is made to prevent executing hotkeys while typing...
function canPerformKeyCombo(keyCombo)
    return disableHotkeysCount == 0 and
               (not modules.game_console:isChatEnabled() or string.match(keyCombo, 'F%d%d?') or
                   string.match(keyCombo, 'Ctrl%+') or string.match(keyCombo, 'Shift%+..+') or
                   string.match(keyCombo, 'Alt%+'))
end
