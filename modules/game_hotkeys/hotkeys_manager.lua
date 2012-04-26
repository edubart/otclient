HotkeysManager = {}

local hotkeysManagerLoaded = false
local hotkeysWindow
local hotkeysButton
local currentHotkeysList
local hotkeyLabelSelectedOnList
local currentItemPreview
local itemWidget
local addHotkey
local removeHotkey
local hotkeyText
local hotKeyTextLabel
local sendAutomatically
local selectObjectButton
local clearObjectButton
local useOnSelf
local useOnTarget
local useWith
local hotkeyList = {}

HOTKEY_MANAGER_USEONSELF = 1
HOTKEY_MANAGER_USEONTARGET = 2
HOTKEY_MANAGER_USEWITH = 3

local hotkeyColors = {
  text = '#888888',
  textAutoSend = '#FFFFFF',
  itemUse = '#8888FF',
  itemUseSelf = '#00FF00',
  itemUseTarget = '#FF0000',
  itemUseWith = '#CC0000',
}

-- public functions
function HotkeysManager.init()
  hotkeysWindow = displayUI('hotkeys_manager.otui')

  hotkeysWindow:setVisible(false)
  hotkeysButton = TopMenu.addGameButton('hotkeysButton', tr('Hotkeys') .. ' (Ctrl+K)', '/game_hotkeys/icon.png', HotkeysManager.toggle)
  Keyboard.bindKeyDown('Ctrl+K', HotkeysManager.toggle)

  currentHotkeysList = hotkeysWindow:getChildById('currentHotkeys')
  currentItemPreview = hotkeysWindow:getChildById('itemPreview')
  addHotkey = hotkeysWindow:getChildById('addHotkey')
  removeHotkey = hotkeysWindow:getChildById('removeHotkey')
  hotkeyText = hotkeysWindow:getChildById('hotkeyText')
  hotKeyTextLabel = hotkeysWindow:getChildById('hotKeyTextLabel')
  sendAutomatically = hotkeysWindow:getChildById('sendAutomatically')
  selectObjectButton = hotkeysWindow:getChildById('selectObjectButton')
  clearObjectButton = hotkeysWindow:getChildById('clearObjectButton')
  useOnSelf = hotkeysWindow:getChildById('useOnSelf')
  useOnTarget = hotkeysWindow:getChildById('useOnTarget')
  useWith = hotkeysWindow:getChildById('useWith')

  itemWidget = createWidget('UIItem')
  itemWidget:setVirtual(true)
  itemWidget:setVisible(false)
  itemWidget:setFocusable(false)

  connect(currentHotkeysList, { onChildFocusChange = function (self, focusedChild) HotkeysManager.checkSelectedHotkey(focusedChild) end } )

  hotkeysManagerLoaded = true

  HotkeysManager.load()
end

function HotkeysManager.load()
  local hotkeySettings = Settings.getNode('HotkeysManager')

  if hotkeySettings ~= nil then
    for i, v in pairs(hotkeySettings) do
      HotkeysManager.addKeyCombo(nil, v.keyCombo, v)
    end
  end
end

function HotkeysManager.save()
  local hotkeySettings = {}
  for i = 1, currentHotkeysList:getChildCount() do
    local child = currentHotkeysList:getChildByIndex(i)
    table.insert(hotkeySettings, {keyCombo = child.keyCombo,
                                  autoSend = child.autoSend,
                                  itemId = child.itemId,
                                  useType = child.useType,
                                  value = child.value})
  end
  Settings.setNode('HotkeysManager', hotkeySettings)
end

function HotkeysManager.terminate()
  hotkeysManagerLoaded = false

  Keyboard.unbindKeyDown('Ctrl+K')
  HotkeysManager.save()

  currentHotkeysList = nil
  hotkeyLabelSelectedOnList = nil
  currentItemPreview = nil

  hotkeyList = {}
  addHotkey = nil
  removeHotkey = nil
  hotkeyText = nil
  hotKeyTextLabel = nil
  sendAutomatically = nil
  selectObjectButton = nil
  clearObjectButton = nil
  useOnSelf = nil
  useOnTarget = nil
  useWith = nil

  itemWidget:destroy()
  itemWidget = nil
  hotkeysWindow:destroy()
  hotkeysWindow = nil
  hotkeysButton:destroy()
  hotkeysButton = nil
end

function HotkeysManager.toggle()
  if hotkeysWindow:isVisible() then
    HotkeysManager.hide()
  else
    HotkeysManager.show()
  end
end

function HotkeysManager.show()
  if g_game.isOnline() then
    hotkeysWindow:grabKeyboard()
    hotkeysWindow:show()
    hotkeysWindow:raise()
  end
end

function HotkeysManager.hide()
  hotkeysWindow:ungrabKeyboard()
  hotkeysWindow:hide()
end

-- private functions
function HotkeysManager.onChooseItemMouseRelease(self, mousePosition, mouseButton)
  local item = nil
  if mouseButton == MouseLeftButton then
    local clickedWidget = GameInterface.getRootPanel():recursiveGetChildByPos(mousePosition, false)
    if clickedWidget then
      if clickedWidget:getClassName() == 'UIMap' then
        local tile = clickedWidget:getTile(mousePosition)
        if tile then
          local thing = tile:getTopMoveThing()
          if thing then
            item = thing:asItem()
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
    HotkeysManager.changeUseType(HOTKEY_MANAGER_USEONSELF)
    HotkeysManager.checkSelectedHotkey(hotkeyLabelSelectedOnList)
    HotkeysManager:show()
  end

  Mouse.restoreCursor()
  self:ungrabMouse()
  self:destroy()
end

function HotkeysManager.startChooseItem()
  local mouseGrabberWidget = createWidget('UIWidget')
  mouseGrabberWidget:setVisible(false)
  mouseGrabberWidget:setFocusable(false)

  connect(mouseGrabberWidget, { onMouseRelease = HotkeysManager.onChooseItemMouseRelease })

  mouseGrabberWidget:grabMouse()
  Mouse.setTargetCursor()

  HotkeysManager:hide()
end

function HotkeysManager.clearObject()
  hotkeyLabelSelectedOnList.itemId = nil
  currentItemPreview:clearItem()
  HotkeysManager.changeUseType(HOTKEY_MANAGER_USEONSELF)
  HotkeysManager.sendAutomatically(false)
  hotkeyLabelSelectedOnList:setText(hotkeyLabelSelectedOnList.keyCombo .. ': ')

  HotkeysManager.checkSelectedHotkey(hotkeyLabelSelectedOnList)
end

function HotkeysManager.addHotkey()
  local widget

  messageBox = createWidget('MainWindow', rootWidget)
  messageBox:grabKeyboard()
  messageBox:setId('assignWindow')
  messageBox:setText(tr('Button Assign'))
  messageBox:setWidth(420)
  messageBox:setHeight(140)
  messageBox:setDragable(false)

  widget = createWidget('Label', messageBox)
  widget:setText(tr('Please, press the key you wish to add onto your hotkeys manager'))
  widget:resizeToText()
  widget:addAnchor(AnchorHorizontalCenter, 'parent', AnchorHorizontalCenter)
  widget:addAnchor(AnchorTop, 'parent', AnchorTop)

  widget = createWidget('Label', messageBox)
  widget:setId('comboPreview')
  widget:setText(tr('Current hotkey to add: %s', 'none'))
  widget.keyCombo = ''
  widget:resizeToText()
  widget:addAnchor(AnchorHorizontalCenter, 'parent', AnchorHorizontalCenter)
  widget:addAnchor(AnchorTop, 'prev', AnchorBottom)
  widget:setMarginTop(20)

  widget = createWidget('Button', messageBox)
  widget:setId('cancelButton')
  widget:setText(tr('Cancel'))
  widget:setWidth(64)
  widget:addAnchor(AnchorBottom, 'parent', AnchorBottom)
  widget:addAnchor(AnchorRight, 'parent', AnchorRight)
  widget.onClick =  function (self)
                      messageBox = nil
                      self:getParent():destroy()
                    end

  widget = createWidget('Button', messageBox)
  widget:setId('addButton')
  widget:setText(tr('Add'))
  widget:setWidth(64)
  widget:disable()
  widget:addAnchor(AnchorBottom, 'cancelButton', AnchorBottom)
  widget:addAnchor(AnchorRight, 'cancelButton', AnchorLeft)
  widget:setMarginRight(10)
  widget.onClick =  function (self)
                      messageBox = nil
                      HotkeysManager.addKeyCombo(self:getParent(), self:getParent():getChildById('comboPreview').keyCombo)
                    end

  connect(messageBox, { onKeyDown = HotkeysManager.hotkeyCapture }, true)
end

function HotkeysManager.addKeyCombo(messageBox, keyCombo, keySettings)
  local label = nil
  if currentHotkeysList:getChildById(keyCombo) == nil then
    local label = createWidget('HotkeyListLabel', currentHotkeysList)
    label:setId(keyCombo)
    label:setColor(hotkeyColors.text)
    label:setText(keyCombo..': ')
    if keySettings then
      hotkeyLabelSelectedOnList = label
      label.keyCombo = keyCombo
      HotkeysManager.sendAutomatically(keySettings.autoSend)
      label.itemId = keySettings.itemId
      currentItemPreview:setItemId(keySettings.itemId)
      HotkeysManager.changeUseType(tonumber(keySettings.useType))
      label.value = keySettings.value
    else
      label.keyCombo = keyCombo
      label.autoSend = false
      label.itemId = nil
      label.useType = HOTKEY_MANAGER_USEONSELF
      label.value = ''
    end

    HotkeysManager.checkSelectedHotkey(label)

    hotkeyList[keyCombo] = label
    Keyboard.bindKeyPress(keyCombo, function () HotkeysManager.call(keyCombo) end, nil, 350)
  end

  if messageBox then
    messageBox:destroy()
    messageBox = nil
  end
end

function HotkeysManager.call(keyCombo)
  if g_game.isOnline() then
    local hotKey = hotkeyList[keyCombo]
    if hotKey ~= nil and hotKey.itemId == nil and hotKey.value ~= '' then
      if hotKey.autoSend then
        g_game.talk(hotKey.value)
      else
        Console.setTextEditText(hotKey.value)
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
        GameInterface.startUseWith(itemWidget:getItem())
      end
    end
  end
end

function HotkeysManager.checkSelectedHotkey(focused)
  if hotkeysManagerLoaded then
    hotkeyLabelSelectedOnList = focused

    if hotkeyLabelSelectedOnList ~= nil then
      removeHotkey:enable()

      if hotkeyLabelSelectedOnList.itemId == nil then
        hotkeyText:enable()
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
      HotkeysManager.changeUseType(hotkeyLabelSelectedOnList.useType)
    else
      hotkeyText:clearText()
      removeHotkey:disable()
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

function HotkeysManager.changeUseType(useType, checked)
  if checked == nil or checked then
    hotkeyLabelSelectedOnList.useType = useType
    if hotkeyLabelSelectedOnList.itemId ~= nil and currentItemPreview:getItem():isMultiUse() then
      useOnSelf:enable()
      useOnTarget:enable()
      useWith:enable()

      if useType == HOTKEY_MANAGER_USEONSELF then
        hotkeyLabelSelectedOnList:setText(tr('%s: (use object on yourself)', hotkeyLabelSelectedOnList.keyCombo))
        hotkeyLabelSelectedOnList:setColor(hotkeyColors.itemUseSelf)
        useOnSelf:setChecked(true)
        useOnTarget:setChecked(false)
        useWith:setChecked(false)
      elseif useType == HOTKEY_MANAGER_USEONTARGET then
        hotkeyLabelSelectedOnList:setText(tr('%s: (use object on target)', hotkeyLabelSelectedOnList.keyCombo))
        hotkeyLabelSelectedOnList:setColor(hotkeyColors.itemUseTarget)
        useOnSelf:setChecked(false)
        useOnTarget:setChecked(true)
        useWith:setChecked(false)
      elseif useType == HOTKEY_MANAGER_USEWITH then
        hotkeyLabelSelectedOnList:setText(tr('%s: (use object with crosshair)', hotkeyLabelSelectedOnList.keyCombo))
        hotkeyLabelSelectedOnList:setColor(hotkeyColors.itemUseWith)

        useOnSelf:setChecked(false)
        useOnTarget:setChecked(false)
        useWith:setChecked(true)
      end
    elseif hotkeyLabelSelectedOnList.itemId ~= nil and not currentItemPreview:getItem():isMultiUse() then
      useOnSelf:disable()
      useOnTarget:disable()
      useWith:disable()

      hotkeyLabelSelectedOnList:setText(tr('%s: (use object)', hotkeyLabelSelectedOnList.keyCombo))
      hotkeyLabelSelectedOnList:setColor(hotkeyColors.itemUse)

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

function HotkeysManager.removeHotkey()
  if hotkeyLabelSelectedOnList ~= nil then
    hotkeyList[hotkeyLabelSelectedOnList.keyCombo] = nil
    Keyboard.unbindKeyPress(hotkeyLabelSelectedOnList.keyCombo)
    hotkeyLabelSelectedOnList:destroy()
  end
end

function HotkeysManager.onHotkeyTextChange(id, value)
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

function HotkeysManager.sendAutomatically(value)
  hotkeyLabelSelectedOnList.autoSend = value
  if value then
    hotkeyLabelSelectedOnList:setColor(hotkeyColors.autoSend)
  else
    hotkeyLabelSelectedOnList:setColor(hotkeyColors.text)
  end
end

function HotkeysManager.hotkeyCapture(widget, keyCode, keyboardModifiers)
  local keyCombo = determineKeyComboDesc(keyCode, keyboardModifiers)
  local comboPreview = rootWidget:getChildById('assignWindow'):getChildById('comboPreview')
  comboPreview:setText(tr('Current hotkey to add: %s', keyCombo))
  comboPreview.keyCombo = keyCombo
  comboPreview:resizeToText()
  rootWidget:getChildById('assignWindow'):getChildById('addButton'):enable()

  return true
end
