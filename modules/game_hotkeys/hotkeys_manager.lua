HOTKEY_MANAGER_USEONSELF = 1
HOTKEY_MANAGER_USEONTARGET = 2
HOTKEY_MANAGER_USEWITH = 3

HotkeyColors = {
  text = '#888888',
  textAutoSend = '#FFFFFF',
  itemUse = '#8888FF',
  itemUseSelf = '#00FF00',
  itemUseTarget = '#FF0000',
  itemUseWith = '#CC0000',
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
  hotkeysWindow = g_ui.displayUI('hotkeys_manager.otui')
  local hotkeyListPanel = hotkeysWindow:getChildById('currentHotkeys')

  hotkeysWindow:setVisible(false)
  hotkeysButton = TopMenu.addLeftGameButton('hotkeysButton', tr('Hotkeys') .. ' (Ctrl+K)', '/game_hotkeys/icon.png', toggle)
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

  connect(g_game, { onGameEnd = hide })
  connect(currentHotkeysList, { onChildFocusChange = function (self, focusedChild) checkSelectedHotkey(focusedChild) end } )

  hotkeysManagerLoaded = true

  load()
end

function terminate()
  hotkeysManagerLoaded = false

  disconnect(g_game, { onGameEnd = hide })
  g_keyboard.unbindKeyDown('Ctrl+K')

  save()

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
  if hotkeySettings ~= nil then
    for i, v in pairs(hotkeySettings) do
      addKeyCombo(nil, v.keyCombo, v)
      hasCombos = true
    end
  end

  -- add default F keys combos
  if not hasCombos then
    for i=1,12 do
      addKeyCombo(nil, 'F' .. i)
    end
  end
end

function save()
  local hotkeySettings = {}
  for i=1, currentHotkeysList:getChildCount() do
    local child = currentHotkeysList:getChildByIndex(i)
    table.insert(hotkeySettings, {keyCombo = child.keyCombo,
                                  autoSend = child.autoSend,
                                  itemId = child.itemId,
                                  useType = child.useType,
                                  value = child.value})
  end

  g_settings.setNode('HotkeysManager', hotkeySettings)
end

function toggle()
  if hotkeysWindow:isVisible() then
    hide()
  else
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

-- private functions
function onChooseItemMouseRelease(self, mousePosition, mouseButton)
  local item = nil
  if mouseButton == MouseLeftButton then
    local clickedWidget = modules.game_interface.getRootPanel():recursiveGetChildByPos(mousePosition, false)
    if clickedWidget then
      if clickedWidget:getClassName() == 'UIMap' then
        local tile = clickedWidget:getTile(mousePosition)
        if tile then
          local thing = tile:getTopMoveThing()
          if thing then
            item = thing:isItem()
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
    show()
  end

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
  local widget

  messageBox = g_ui.createWidget('MainWindow', rootWidget)
  messageBox:grabKeyboard()
  messageBox:setId('assignWindow')
  messageBox:setText(tr('Button Assign'))
  messageBox:setWidth(420)
  messageBox:setHeight(140)
  messageBox:setDragable(false)

  widget = g_ui.createWidget('Label', messageBox)
  widget:setText(tr('Please, press the key you wish to add onto your hotkeys manager'))
  widget:resizeToText()
  widget:addAnchor(AnchorHorizontalCenter, 'parent', AnchorHorizontalCenter)
  widget:addAnchor(AnchorTop, 'parent', AnchorTop)

  widget = g_ui.createWidget('Label', messageBox)
  widget:setId('comboPreview')
  widget:setText(tr('Current hotkey to add: %s', 'none'))
  widget.keyCombo = ''
  widget:resizeToText()
  widget:addAnchor(AnchorHorizontalCenter, 'parent', AnchorHorizontalCenter)
  widget:addAnchor(AnchorTop, 'prev', AnchorBottom)
  widget:setMarginTop(20)

  widget = g_ui.createWidget('Button', messageBox)
  widget:setId('cancelButton')
  widget:setText(tr('Cancel'))
  widget:setWidth(64)
  widget:addAnchor(AnchorBottom, 'parent', AnchorBottom)
  widget:addAnchor(AnchorRight, 'parent', AnchorRight)
  widget.onClick =  function (self)
                      messageBox = nil
                      self:getParent():destroy()
                    end

  widget = g_ui.createWidget('Button', messageBox)
  widget:setId('addButton')
  widget:setText(tr('Add'))
  widget:setWidth(64)
  widget:disable()
  widget:addAnchor(AnchorBottom, 'cancelButton', AnchorBottom)
  widget:addAnchor(AnchorRight, 'cancelButton', AnchorLeft)
  widget:setMarginRight(10)
  widget.onClick =  function (self)
                      messageBox = nil
                      addKeyCombo(self:getParent(), self:getParent():getChildById('comboPreview').keyCombo)
                    end

  connect(messageBox, { onKeyDown = hotkeyCapture }, true)
end

function addKeyCombo(messageBox, keyCombo, keySettings)
  local label = nil
  if currentHotkeysList:getChildById(keyCombo) == nil then
    local label = g_ui.createWidget('HotkeyListLabel', currentHotkeysList)
    label:setId(keyCombo)
    label:setColor(HotkeyColors.text)
    label:setText(keyCombo..': ')
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
        g_game.talk(hotKey.value)
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
