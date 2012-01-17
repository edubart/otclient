Hotkeys = {}

-- private functions
local function translateKeyCombo(keyCombo)
  if not keyCombo or #keyCombo == 0 then return nil end
  table.sort(keyCombo)
  local keyComboDesc = ''
  for k,v in pairs(keyCombo) do
    local keyDesc = KeyCodeDescs[v]
    if keyDesc == nil then return nil end
    keyComboDesc = keyComboDesc .. '+' .. keyDesc
  end
  keyComboDesc = keyComboDesc:sub(2)
  return keyComboDesc
end

local function retranslateKeyComboDesc(keyComboDesc)
  if keyComboDesc == nil then return nil end
  local keyCombo = {}
  for i,currentKeyDesc in ipairs(keyComboDesc:split('+')) do
    for keyCode, keyDesc in pairs(KeyCodeDescs) do
      if keyDesc:lower() == currentKeyDesc:trim():lower() then
        table.insert(keyCombo, keyCode)
      end
    end
  end
  return translateKeyCombo(keyCombo)
end

local function determineKeyComboDesc(keyCode, keyboardModifiers)
  local keyCombo = {}
  if keyCode == KeyCtrl or keyCode == KeyShift or keyCode == KeyAlt or keyCode == KeyAltGr then
    table.insert(keyCombo, keyCode)
  elseif KeyCodeDescs[keyCode] ~= nil then
    if keyboardModifiers == KeyboardCtrlModifier then
      table.insert(keyCombo, KeyCtrl)
    elseif keyboardModifiers == KeyboardAltModifier then
      table.insert(keyCombo, KeyAlt)
    elseif keyboardModifiers == KeyboardCtrlAltModifier then
      table.insert(keyCombo, KeyCtrl)
      table.insert(keyCombo, KeyAlt)
    elseif keyboardModifiers == KeyboardShiftModifier then
      table.insert(keyCombo, KeyShift)
    elseif keyboardModifiers == KeyboardCtrlShiftModifier then
      table.insert(keyCombo, KeyCtrl)
      table.insert(keyCombo, KeyShift)
    elseif keyboardModifiers == KeyboardAltShiftModifier then
      table.insert(keyCombo, KeyAlt)
      table.insert(keyCombo, KeyShift)
    elseif keyboardModifiers == KeyboardCtrlAltShiftModifier then
      table.insert(keyCombo, KeyCtrl)
      table.insert(keyCombo, KeyAlt)
      table.insert(keyCombo, KeyShift)
    end
    table.insert(keyCombo, keyCode)
  end
  table.sort(keyCombo)
  return translateKeyCombo(keyCombo)
end

local function onWidgetKeyDown(widget, keyCode, keyboardModifiers)
  if keyCode == KeyUnknown then return end
  local keyComboDesc = determineKeyComboDesc(keyCode, keyboardModifiers)
  local callback = widget.boundKeyDownCombos[keyComboDesc]
  if callback then
    callback()
    return true
  end
  return false
end

local function onWidgetKeyPress(widget, keyCode, keyboardModifiers, wouldFilter)
  local keyComboDesc = determineKeyComboDesc(keyCode, keyboardModifiers)
  if keyCode == KeyUnknown then return end
  local callback = widget.boundKeyPressCombos[keyComboDesc]
  if callback then
    callback()
    return true
  end
  return false
end

local function connectKeyDownEvent(widget)
  if widget.boundKeyDownCombos then return end
  connect(widget, { onKeyDown = onWidgetKeyDown })
  widget.boundKeyDownCombos = {}
end

local function connectKeyPressEvent(widget)
  if widget.boundKeyPressCombos then return end
  connect(widget, { onKeyPress = onWidgetKeyPress })
  widget.boundKeyPressCombos = {}
end

-- public functions
function Hotkeys.bindKeyDown(keyComboDesc, callback, widget)
  widget = widget or rootWidget
  connectKeyDownEvent(widget)
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  if keyComboDesc then
    widget.boundKeyDownCombos[keyComboDesc] = callback
  else
    error('key combo \'' .. keyComboDesc .. '\' is failed')
  end
end

function Hotkeys.bindKeyPress(keyComboDesc, callback, widget)
  widget = widget or rootWidget
  connectKeyPressEvent(widget)
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  if keyComboDesc then
    widget.boundKeyPressCombos[keyComboDesc] = callback
  else
    error('key combo \'' .. keyComboDesc .. '\' is failed')
  end
end

function Hotkeys.unbindKeyDown(keyComboDesc, widget)
  widget = widget or rootWidget
  if widget.boundKeyDownCombos == nil then return end
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  if keyComboDesc then
    widget.boundKeyDownCombos[keyComboDesc] = nil
  end
end

function Hotkeys.unbindKeyPress(keyComboDesc, widget)
  widget = widget or rootWidget
  if widget.boundKeyPressCombos == nil then return end
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  if keyComboDesc then
    widget.boundKeyPressCombos[keyComboDesc] = nil
  end
end
