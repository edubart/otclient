HOTKEY_MANAGER_USEONSELF = 1
HOTKEY_MANAGER_USEONTARGET = 2
HOTKEY_MANAGER_USEWITH = 3

HotkeyColors = {
  text = '#888888',
  textAutoSend = '#FFFFFF',
  itemUse = '#8888FF',
  itemUseSelf = '#00FF00',
  itemUseTarget = '#FF0000',
  itemUseWith = '#F5B325',
}

hotkeysManagerLoaded = false
hotkeysWindow = nil
hotkeysButton = nil
currentHotkeysList = nil
hotkeyLabelSelectedOnList = nil
currentItemPreview = nil
itemWidget = nil
addHotkeyButton = nil
removeHotkeyButton = nil
hotkeyText = nil
hotKeyTextLabel = nil
sendAutomatically = nil
selectObjectButton = nil
clearObjectButton = nil
useOnSelf = nil
useOnTarget = nil
useWith = nil
hotkeyList = {}

-- public functions
function init()
  hotkeysWindow = g_ui.displayUI('hotkeys_manager')
  local hotkeyListPanel = hotkeysWindow:getChildById('currentHotkeys')

  hotkeysWindow:setVisible(false)
  hotkeysButton = modules.client_topmenu.addLeftGameButton('hotkeysButton', tr('Hotkeys') .. ' (Ctrl+K)', '/images/topbuttons/hotkeys', toggle)
  g_keyboard.bindKeyDown('Ctrl+K', toggle)
  g_keyboard.bindKeyPress('Down', function() hotkeyListPanel:focusNextChild(KeyboardFocusReason) end, hotkeysWindow)
  g_keyboard.bindKeyPress('Up', function() hotkeyListPanel:focusPreviousChild(KeyboardFocusReason) end, hotkeysWindow)

  currentHotkeysList = hotkeysWindow:getChildById('currentHotkeys')
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

  itemWidget = g_ui.createWidget('UIItem')
  itemWidget:setVirtual(true)
  itemWidget:setVisible(false)
  itemWidget:setFocusable(false)

  connect(g_game, { 
    onGameStart = onGameStart,
    onGameEnd = hide
  })
  connect(currentHotkeysList, { onChildFocusChange = function (self, focusedChild) checkSelectedHotkey(focusedChild) end } )

  hotkeysManagerLoaded = true
end

function terminate()
  hotkeysManagerLoaded = false

  disconnect(g_game, {
    onGameStart = onGameStart,
    onGameEnd = hide
  })
  g_keyboard.unbindKeyDown('Ctrl+K')

  for keyCombo,v in pairs(hotkeyList) do
    g_keyboard.unbindKeyPress(keyCombo)
  end
  hotkeyList = {}

  itemWidget:destroy()
  hotkeysWindow:destroy()
  hotkeysButton:destroy()
end

function load()
  local hotkeySettings = g_settings.getNode('HotkeysManager')
  local hasCombos = false
  if not table.empty(hotkeySettings) then
    local serverHotkeys = hotkeySettings[G.host]

    local hotkeys
    if not table.empty(serverHotkeys) then
      hotkeys = serverHotkeys[g_game.getLocalPlayer():getName()]
    end
    if not table.empty(hotkeys) then
      for k, setting in pairs(hotkeys) do
        addKeyCombo(nil, setting.keyCombo, setting)
        hasCombos = true
      end
    end
  end

  -- add default F keys combos
  if not hasCombos then
    loadDefautComboKeys()
  end
end

function save()
  local char = g_game.getLocalPlayer():getName()
  local server = G.host

  local hotkeySettings = g_settings.getNode('HotkeysManager') or {}
  hotkeySettings[server] = {}
  hotkeySettings[server][char] = {}

  for i=1, currentHotkeysList:getChildCount() do
    local child = currentHotkeysList:getChildByIndex(i)
    table.insert(hotkeySettings[server][char], {
      keyCombo = child.keyCombo,
      autoSend = child.autoSend,
      itemId = child.itemId,
      useType = child.useType,
      value = child.value
    })
  end
  
  g_settings.setNode('HotkeysManager', hotkeySettings)
end

function toggle()
  if not hotkeysWindow:isVisible() then
    show()
  end
end

function show()
  if g_game.isOnline() then
    hotkeysWindow:grabKeyboard()
    hotkeysWindow:show()
    hotkeysWindow:raise()
  end
end

function hide()
  hotkeysWindow:ungrabKeyboard()
  hotkeysWindow:hide()
end

function ok()
  save()
  hide()
end

function reload()
  local children = currentHotkeysList:getChildren()
  for i=1,#children do
    hotkeyList[children[i].keyCombo] = nil
    g_keyboard.unbindKeyPress(children[i].keyCombo)
  end
  currentHotkeysList:destroyChildren()
  hotkeyLabelSelectedOnList = nil
  load()
  hide()
end

function loadDefautComboKeys()
  for i=1,12 do
    addKeyCombo(nil, 'F' .. i)
  end
  for i=1,4 do
    addKeyCombo(nil, 'Shift+F' .. i)
  end
end

-- private functions
function onGameStart()
  reload()
end

function onChooseItemMouseRelease(self, mousePosition, mouseButton)
  local item = nil
  if mouseButton == MouseLeftButton then
    local clickedWidget = modules.game_interface.getRootPanel():recursiveGetChildByPos(mousePosition, false)
    if clickedWidget then
      if clickedWidget:getClassName() == 'UIMap' then
        local tile = clickedWidget:getTile(mousePosition)
        if tile then
          local thing = tile:getTopMoveThing()
          if thing and thing:isItem() then
            item = thing
          end
        end
      elseif clickedWidget:getClassName() == 'UIItem' and not clickedWidget:isVirtual() then
        item = clickedWidget:getItem()
      end
    end
  end

  if item then
    currentItemPreview:setItemId(item:getId())
    hotkeyLabelSelectedOnList.itemId = item:getId()
    changeUseType(HOTKEY_MANAGER_USEONSELF)
    checkSelectedHotkey(hotkeyLabelSelectedOnList)
  end

  show()

  g_mouse.restoreCursor()
  self:ungrabMouse()
  self:destroy()
end

function startChooseItem()
  local mouseGrabberWidget = g_ui.createWidget('UIWidget')
  mouseGrabberWidget:setVisible(false)
  mouseGrabberWidget:setFocusable(false)

  connect(mouseGrabberWidget, { onMouseRelease = onChooseItemMouseRelease })

  mouseGrabberWidget:grabMouse()
  g_mouse.setTargetCursor()

  hide()
end

function clearObject()
  hotkeyLabelSelectedOnList.itemId = nil
  currentItemPreview:clearItem()
  changeUseType(HOTKEY_MANAGER_USEONSELF)
  setSendAutomatically(false)
  hotkeyLabelSelectedOnList:setText(hotkeyLabelSelectedOnList.keyCombo .. ': ')

  checkSelectedHotkey(hotkeyLabelSelectedOnList)
end

function addHotkey()
  local assignWindow = g_ui.createWidget('HotkeyAssignWindow', rootWidget)
  assignWindow:grabKeyboard()

  local comboLabel = assignWindow:getChildById('comboPreview')
  comboLabel.keyCombo = ''

  connect(assignWindow, { onKeyDown = hotkeyCapture }, true)
end

function addKeyCombo(messageBox, keyCombo, keySettings)
  local label = nil
  if currentHotkeysList:getChildById(keyCombo) == nil then
    local label = g_ui.createWidget('HotkeyListLabel')
    label:setId(keyCombo)
    label:setColor(HotkeyColors.text)
    label:setText(keyCombo .. ': ')

    local children = currentHotkeysList:getChildren()
    children[#children+1] = label
    table.sort(children, function(a,b)
      if a:getId():len() < b:getId():len() then
        return true
      elseif a:getId():len() == b:getId():len() then
        return a:getId() < b:getId()
      else
        return false
      end
    end)
    for i=1,#children do
      if children[i] == label then
        currentHotkeysList:insertChild(i, label)
        break
      end
    end

    if keySettings then
      hotkeyLabelSelectedOnList = label
      label.keyCombo = keyCombo
      setSendAutomatically(keySettings.autoSend)
      label.itemId = keySettings.itemId
      currentItemPreview:setItemId(keySettings.itemId)
      changeUseType(tonumber(keySettings.useType))
      label.value = keySettings.value
    else
      label.keyCombo = keyCombo
      label.autoSend = false
      label.itemId = nil
      label.useType = HOTKEY_MANAGER_USEONSELF
      label.value = ''
    end

    checkSelectedHotkey(label)

    hotkeyList[keyCombo] = label
    g_keyboard.bindKeyPress(keyCombo, function () call(keyCombo) end, nil, 350)
  end

  if messageBox then
    messageBox:destroy()
    messageBox = nil
  end
end

function call(keyCombo)
  if g_game.isOnline() then
    local hotKey = hotkeyList[keyCombo]
    if hotKey ~= nil and hotKey.itemId == nil and hotKey.value ~= '' then
      if hotKey.autoSend then
        modules.game_console.sendMessage(hotKey.value)
      else
        modules.game_console.setTextEditText(hotKey.value)
      end
    elseif hotKey.itemId ~= nil then
      if hotKey.useType == HOTKEY_MANAGER_USEONSELF then
        g_game.useInventoryItemWith(hotKey.itemId, g_game.getLocalPlayer())
      elseif hotKey.useType == HOTKEY_MANAGER_USEONTARGET then
        local attackingCreature = g_game.getAttackingCreature()
        if attackingCreature then
          g_game.useInventoryItemWith(hotKey.itemId, attackingCreature)
        end
      elseif hotKey.useType == HOTKEY_MANAGER_USEWITH then
        itemWidget:setItemId(hotKey.itemId)
        modules.game_interface.startUseWith(itemWidget:getItem())
      end
    end
  end
end

function checkSelectedHotkey(focused)
  if not focused then return end
  if hotkeysManagerLoaded then
    hotkeyLabelSelectedOnList = focused

    if hotkeyLabelSelectedOnList ~= nil then
      removeHotkeyButton:enable()

      if hotkeyLabelSelectedOnList.itemId == nil then
        hotkeyText:enable()
        hotkeyText:focus()
        hotKeyTextLabel:enable()
        hotkeyText:setText(hotkeyLabelSelectedOnList.value)

        if hotkeyLabelSelectedOnList.value ~= '' then
          sendAutomatically:setChecked(hotkeyLabelSelectedOnList.autoSend)
          sendAutomatically:enable()
        else
          sendAutomatically:disable()
        end

        selectObjectButton:enable()
        clearObjectButton:disable()

        currentItemPreview:setItemId(0)
      else
        hotkeyText:clearText()
        hotkeyText:disable()
        hotKeyTextLabel:disable()
        sendAutomatically:disable()

        selectObjectButton:disable()
        clearObjectButton:enable()

        currentItemPreview:setItemId(hotkeyLabelSelectedOnList.itemId)
      end
      changeUseType(hotkeyLabelSelectedOnList.useType)
    else
      hotkeyText:clearText()
      removeHotkeyButton:disable()
      hotkeyText:disable()
      sendAutomatically:disable()
      sendAutomatically:setChecked(false)

      currentItemPreview:setItemId(0)
      selectObjectButton:disable()
      clearObjectButton:disable()
      useOnSelf:disable()
      useOnTarget:disable()
      useWith:disable()
      useOnSelf:setChecked(false)
      useOnTarget:setChecked(false)
      useWith:setChecked(false)
    end
  end
end

function changeUseType(useType, checked)
  if checked == nil or checked then
    hotkeyLabelSelectedOnList.useType = useType
    if hotkeyLabelSelectedOnList.itemId ~= nil and currentItemPreview:getItem():isMultiUse() then
      useOnSelf:enable()
      useOnTarget:enable()
      useWith:enable()

      if useType == HOTKEY_MANAGER_USEONSELF then
        hotkeyLabelSelectedOnList:setText(tr('%s: (use object on yourself)', hotkeyLabelSelectedOnList.keyCombo))
        hotkeyLabelSelectedOnList:setColor(HotkeyColors.itemUseSelf)
        useOnSelf:setChecked(true)
        useOnTarget:setChecked(false)
        useWith:setChecked(false)
      elseif useType == HOTKEY_MANAGER_USEONTARGET then
        hotkeyLabelSelectedOnList:setText(tr('%s: (use object on target)', hotkeyLabelSelectedOnList.keyCombo))
        hotkeyLabelSelectedOnList:setColor(HotkeyColors.itemUseTarget)
        useOnSelf:setChecked(false)
        useOnTarget:setChecked(true)
        useWith:setChecked(false)
      elseif useType == HOTKEY_MANAGER_USEWITH then
        hotkeyLabelSelectedOnList:setText(tr('%s: (use object with crosshair)', hotkeyLabelSelectedOnList.keyCombo))
        hotkeyLabelSelectedOnList:setColor(HotkeyColors.itemUseWith)

        useOnSelf:setChecked(false)
        useOnTarget:setChecked(false)
        useWith:setChecked(true)
      end
    elseif hotkeyLabelSelectedOnList.itemId ~= nil and not currentItemPreview:getItem():isMultiUse() then
      useOnSelf:disable()
      useOnTarget:disable()
      useWith:disable()

      hotkeyLabelSelectedOnList:setText(tr('%s: (use object)', hotkeyLabelSelectedOnList.keyCombo))
      hotkeyLabelSelectedOnList:setColor(HotkeyColors.itemUse)

      useOnSelf:setChecked(false)
      useOnTarget:setChecked(false)
      useWith:setChecked(false)
    else
      useOnSelf:disable()
      useOnTarget:disable()
      useWith:disable()

      useOnSelf:setChecked(false)
      useOnTarget:setChecked(false)
      useWith:setChecked(false)
    end
  end
end

function removeHotkey()
  if hotkeyLabelSelectedOnList ~= nil then
    hotkeyList[hotkeyLabelSelectedOnList.keyCombo] = nil
    g_keyboard.unbindKeyPress(hotkeyLabelSelectedOnList.keyCombo)
    hotkeyLabelSelectedOnList:destroy()
  end
end

function onHotkeyTextChange(id, value)
  if hotkeyLabelSelectedOnList ~= nil and hotkeyLabelSelectedOnList.keyCombo ~= nil then
    hotkeyLabelSelectedOnList:setText(hotkeyLabelSelectedOnList.keyCombo .. ': ' .. value)
    hotkeyLabelSelectedOnList.value = value

    if value ~= '' then
      sendAutomatically:enable()
    else
      sendAutomatically:disable()
      sendAutomatically:setChecked(false)
    end
  end
end

function setSendAutomatically(value)
  hotkeyLabelSelectedOnList.autoSend = value
  if value then
    hotkeyLabelSelectedOnList:setColor(HotkeyColors.autoSend)
  else
    hotkeyLabelSelectedOnList:setColor(HotkeyColors.text)
  end
end

function hotkeyCapture(widget, keyCode, keyboardModifiers)
  local keyCombo = determineKeyComboDesc(keyCode, keyboardModifiers)
  local comboPreview = rootWidget:getChildById('assignWindow'):getChildById('comboPreview')
  comboPreview:setText(tr('Current hotkey to add: %s', keyCombo))
  comboPreview.keyCombo = keyCombo
  comboPreview:resizeToText()
  rootWidget:getChildById('assignWindow'):getChildById('addButton'):enable()

  return true
end
