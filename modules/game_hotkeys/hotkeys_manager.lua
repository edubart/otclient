HotkeysManager = {}

local hotkeysWindow
local hotkeysButton
local currentHotkeysList
local hotkeyLabelSelectedOnList
local currentItemPreview
local hotkeyList = {}
local itemWidget

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
	hotkeysButton = TopMenu.addButton('hotkeysButton', 'Hotkeys (Ctrl+K)', '/game_hotkeys/icon.png', HotkeysManager.toggle)
	Keyboard.bindKeyDown('Ctrl+K', HotkeysManager.toggle)
  
  currentHotkeysList = hotkeysWindow:getChildById('currentHotkeys')
  currentItemPreview = hotkeysWindow:getChildById('itemPreview')
  
  itemWidget = createWidget('UIItem')
  itemWidget:setVirtual(true)
  itemWidget:setVisible(false)
  itemWidget:setFocusable(false) 
  
  HotkeysManager.load()
end

function HotkeysManager.load()
  local hotkeySettings = Settings.getNode('HotkeysManager')
  
  for i, v in pairs(hotkeySettings) do 
    HotkeysManager.addKeyCombo(nil, v.keyCombo, v)
  end
  
  HotkeysManager.checkSelectedHotkey()
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
	Keyboard.unbindKeyDown('Ctrl+K')
  HotkeysManager.save()
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
	if Game.isOnline() then
    hotkeysWindow:grabKeyboard()
		hotkeysWindow:show()
		hotkeysWindow:lock()
	end
end

function HotkeysManager.hide()  
  hotkeysWindow:ungrabKeyboard()
	hotkeysWindow:unlock()
	hotkeysWindow:hide()
end

-- private functions
function HotkeysManager.onChooseItemMouseRelease(self, mousePosition, mouseButton)
  local item = nil
  if mouseButton == MouseLeftButton then
    local clickedWidget = Game.gameUi:recursiveGetChildByPos(mousePosition)
    if clickedWidget then
      if clickedWidget:getClassName() == 'UIMap' then
        local tile = clickedWidget:getTile(mousePosition)
        if tile then
          local thing = tile:getTopMoveThing()
          if thing then
            local uiitem = thing:asItem()
            if uiitem then
              item = uiitem:getItem()
            end
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
    HotkeysManager.checkSelectedHotkey()
    HotkeysManager:show()    
  end
  
  Mouse.restoreCursor()
  self:ungrabMouse()
  self:destroy()
end

function HotkeysManager.startChooseItem()
  mouseGrabberWidget = createWidget('UIWidget')
  mouseGrabberWidget:setVisible(false)
  mouseGrabberWidget:setFocusable(false)

  connect(mouseGrabberWidget, { onMouseRelease = HotkeysManager.onChooseItemMouseRelease })

  mouseGrabberWidget:grabMouse()
  Mouse.setTargetCursor()
  
  HotkeysManager:hide()
end

function HotkeysManager.clearObject()
  hotkeyLabelSelectedOnList.itemId = nil  
  currentItemPreview:setItemId(0) --TODO: call :clear() after get it binded
  HotkeysManager.changeUseType(HOTKEY_MANAGER_USEONSELF)
  HotkeysManager.sendAutomatically(false)
  hotkeyLabelSelectedOnList:setText(hotkeyLabelSelectedOnList.keyCombo .. ': ')
  
  HotkeysManager.checkSelectedHotkey()
end

function HotkeysManager.addHotkey()
  local messageBox = createWidget('MainWindow', hotkeysWindow)
  messageBox:setId('assignWindow')
  messageBox:setText('Button Assign')
  messageBox:setWidth(420)
  messageBox:setHeight(140)
  messageBox:setDragable(false)
  
  local label = createWidget('Label', messageBox)
  label:setText('Please, press the key you wish to add onto your hotkeys manager')
  label:resizeToText()
  label:addAnchor(AnchorHorizontalCenter, 'parent', AnchorHorizontalCenter)
  label:addAnchor(AnchorTop, 'parent', AnchorTop)
  
  local label = createWidget('Label', messageBox)
  label:setId('comboPreview')
  label:setText('Current hotkey to add: None')
  label.keyCombo = ''
  label:resizeToText()
  label:addAnchor(AnchorHorizontalCenter, 'parent', AnchorHorizontalCenter)
  label:addAnchor(AnchorTop, 'prev', AnchorBottom)
  label:setMarginTop(20)
  
  local button = createWidget('Button', messageBox)
  button:setId('cancelButton')
	button:setText('Cancel')
	button:setWidth(64)
	button:addAnchor(AnchorBottom, 'parent', AnchorBottom)
	button:addAnchor(AnchorRight, 'parent', AnchorRight)
	button.onClick = function () messageBox:destroy() end
  
  local button = createWidget('Button', messageBox)
  button:setId('addButton')
	button:setText('Add')
	button:setWidth(64)
  button:disable()
	button:addAnchor(AnchorBottom, 'cancelButton', AnchorBottom)
	button:addAnchor(AnchorRight, 'cancelButton', AnchorLeft)
  button:setMarginRight(10)
	button.onClick = function () HotkeysManager.addKeyCombo(messageBox, label.keyCombo) end
  
  connect(messageBox, { onKeyDown = HotkeysManager.hotkeyCapture }, true)
end

function HotkeysManager.addKeyCombo(messageBox, keyCombo, keySettings)
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
    connect(label, { onClick = function () HotkeysManager.checkSelectedHotkey() return true end } )
    
    HotkeysManager.checkSelectedHotkey()
    
    hotkeyList[keyCombo] = label
    Keyboard.bindKeyDown(keyCombo, function () HotkeysManager.call(keyCombo) end)
  end
  
  if messageBox then
    messageBox:destroy()
  end
end

function HotkeysManager.call(keyCombo)
  if Game.isOnline() then
    local hotKey = hotkeyList[keyCombo]
    if hotKey.itemId == nil and hotKey.value ~= '' then
      if hotKey.autoSend then
        Game.talk(hotKey.value)
      else
        rootWidget:getChildById('gameRootInterface'):getChildById('gameBottomPanel'):getChildById('consolePanel'):getChildById('consoleLineEdit'):setText(hotKey.value)
      end
    elseif hotKey.itemId ~= nil then
      if hotKey.useType == HOTKEY_MANAGER_USEONSELF then
        Game.useInventoryItem(hotKey.itemId, Game.getLocalPlayer())
      elseif hotKey.useType == HOTKEY_MANAGER_USEONTARGET then
        local attackingCreature = Game.getLocalPlayer():getAttackingCreature()
        if attackingCreature then
          Game.useInventoryItem(hotKey.itemId, attackingCreature)
        end
      elseif hotKey.useType == HOTKEY_MANAGER_USEWITH then      
        itemWidget:setItemId(hotKey.itemId)
        Game.startUseWith(itemWidget:getItem())
      end    
    end
  end
end

function HotkeysManager.checkSelectedHotkey()
  hotkeyLabelSelectedOnList = nil
  for i = 1, currentHotkeysList:getChildCount() do
    local child = currentHotkeysList:getChildByIndex(i)
    if child:isFocused() then
      hotkeyLabelSelectedOnList = child
      break -- stop looping
    end
  end
  
  if hotkeyLabelSelectedOnList ~= nil then    
    hotkeysWindow:getChildById('removeHotkey'):enable()
    
    if hotkeyLabelSelectedOnList.itemId == nil then
      hotkeysWindow:getChildById('hotkeyText'):enable()
      hotkeysWindow:getChildById('hotKeyTextLabel'):enable()
      hotkeysWindow:getChildById('hotkeyText'):setText(hotkeyLabelSelectedOnList.value)
      
      if hotkeyLabelSelectedOnList.value ~= '' then      
        hotkeysWindow:getChildById('sendAutomatically'):enable()
      else
        hotkeysWindow:getChildById('sendAutomatically'):disable()
      end
    
      hotkeysWindow:getChildById('selectObjectButton'):enable()
      hotkeysWindow:getChildById('clearObjectButton'):disable()    

      currentItemPreview:setItemId(0)
    else      
      hotkeysWindow:getChildById('hotkeyText'):clearText()
      hotkeysWindow:getChildById('hotkeyText'):disable()
      hotkeysWindow:getChildById('hotKeyTextLabel'):disable()
      hotkeysWindow:getChildById('sendAutomatically'):disable()
      
      hotkeysWindow:getChildById('selectObjectButton'):disable()
      hotkeysWindow:getChildById('clearObjectButton'):enable()
      
      currentItemPreview:setItemId(hotkeyLabelSelectedOnList.itemId)     
    end
    HotkeysManager.changeUseType(hotkeyLabelSelectedOnList.useType)   
  else
    hotkeysWindow:getChildById('hotkeyText'):clearText()
    hotkeysWindow:getChildById('removeHotkey'):disable()
    hotkeysWindow:getChildById('hotkeyText'):disable()
    hotkeysWindow:getChildById('sendAutomatically'):disable()
    hotkeysWindow:getChildById('sendAutomatically'):setChecked(false)
    
    currentItemPreview:setItemId(0)
    hotkeysWindow:getChildById('selectObjectButton'):disable()
    hotkeysWindow:getChildById('clearObjectButton'):disable()
    hotkeysWindow:getChildById('useOnSelf'):disable()
    hotkeysWindow:getChildById('useOnTarget'):disable()
    hotkeysWindow:getChildById('useWith'):disable()
    hotkeysWindow:getChildById('useOnSelf'):setChecked(false)
    hotkeysWindow:getChildById('useOnTarget'):setChecked(false)
    hotkeysWindow:getChildById('useWith'):setChecked(false)
  end  
end

function HotkeysManager.changeUseType(useType, checked)
  if checked == nil or checked then
    hotkeyLabelSelectedOnList.useType = useType 
    if hotkeyLabelSelectedOnList.itemId ~= nil and currentItemPreview:getItem():isMultiUse() then
      hotkeysWindow:getChildById('useOnSelf'):enable()
      hotkeysWindow:getChildById('useOnTarget'):enable()
      hotkeysWindow:getChildById('useWith'):enable()
      
      if useType == HOTKEY_MANAGER_USEONSELF then
        hotkeyLabelSelectedOnList:setText(hotkeyLabelSelectedOnList.keyCombo .. ': (use object on yourself)')
        hotkeyLabelSelectedOnList:setColor(hotkeyColors.itemUseSelf)
        hotkeysWindow:getChildById('useOnSelf'):setChecked(true)
        hotkeysWindow:getChildById('useOnTarget'):setChecked(false)
        hotkeysWindow:getChildById('useWith'):setChecked(false)
      elseif useType == HOTKEY_MANAGER_USEONTARGET then  
        hotkeyLabelSelectedOnList:setText(hotkeyLabelSelectedOnList.keyCombo .. ': (use object on target)')
        hotkeyLabelSelectedOnList:setColor(hotkeyColors.itemUseTarget)
        hotkeysWindow:getChildById('useOnSelf'):setChecked(false)
        hotkeysWindow:getChildById('useOnTarget'):setChecked(true)
        hotkeysWindow:getChildById('useWith'):setChecked(false)
      elseif useType == HOTKEY_MANAGER_USEWITH then
        hotkeyLabelSelectedOnList:setText(hotkeyLabelSelectedOnList.keyCombo .. ': (use object with crosshair)')
        hotkeyLabelSelectedOnList:setColor(hotkeyColors.itemUseWith)
        
        hotkeysWindow:getChildById('useOnSelf'):setChecked(false)
        hotkeysWindow:getChildById('useOnTarget'):setChecked(false)
        hotkeysWindow:getChildById('useWith'):setChecked(true)
      end
    elseif hotkeyLabelSelectedOnList.itemId ~= nil and not currentItemPreview:getItem():isMultiUse() then
      hotkeysWindow:getChildById('useOnSelf'):disable()
      hotkeysWindow:getChildById('useOnTarget'):disable()
      hotkeysWindow:getChildById('useWith'):disable()
      
      hotkeyLabelSelectedOnList:setText(hotkeyLabelSelectedOnList.keyCombo .. ': (use object)')
      hotkeyLabelSelectedOnList:setColor(hotkeyColors.itemUse)
      
      hotkeysWindow:getChildById('useOnSelf'):setChecked(false)
      hotkeysWindow:getChildById('useOnTarget'):setChecked(false)
      hotkeysWindow:getChildById('useWith'):setChecked(false)
    else    
      hotkeysWindow:getChildById('useOnSelf'):disable()
      hotkeysWindow:getChildById('useOnTarget'):disable()
      hotkeysWindow:getChildById('useWith'):disable()
      
      hotkeysWindow:getChildById('useOnSelf'):setChecked(false)
      hotkeysWindow:getChildById('useOnTarget'):setChecked(false)
      hotkeysWindow:getChildById('useWith'):setChecked(false)
    end
  end
end

function HotkeysManager.removeHotkey()
  if hotkeyLabelSelectedOnList ~= nil then
    hotkeyList[hotkeyLabelSelectedOnList.keyCombo] = nil
    Keyboard.unbindKeyDown(hotkeyLabelSelectedOnList.keyCombo)
    hotkeyLabelSelectedOnList:destroy()
    HotkeysManager.checkSelectedHotkey() 
  end
end

function HotkeysManager.onHotkeyTextChange(id, value)
  hotkeyLabelSelectedOnList:setText(hotkeyLabelSelectedOnList.keyCombo ..': ' .. value)
  hotkeyLabelSelectedOnList.value = value
  
  if value ~= '' then
    hotkeysWindow:getChildById('sendAutomatically'):enable()
  else
    hotkeysWindow:getChildById('sendAutomatically'):disable()
    hotkeysWindow:getChildById('sendAutomatically'):setChecked(false)
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
  local comboPreview = hotkeysWindow:getChildById('assignWindow'):getChildById('comboPreview')
  comboPreview:setText('Current hotkey to add: '.. keyCombo)
  comboPreview.keyCombo = keyCombo
  comboPreview:resizeToText()
  hotkeysWindow:getChildById('assignWindow'):getChildById('addButton'):enable()
  
  return true
end