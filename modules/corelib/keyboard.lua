-- @docclass
g_keyboard = {}

-- private functions
function translateKeyCombo(keyCombo)
  if not keyCombo or #keyCombo == 0 then return nil end
  local keyComboDesc = ''
  for k,v in pairs(keyCombo) do
    local keyDesc = KeyCodeDescs[v]
    if keyDesc == nil then return nil end
    keyComboDesc = keyComboDesc .. '+' .. keyDesc
  end
  keyComboDesc = keyComboDesc:sub(2)
  return keyComboDesc
end

local function getKeyCode(key)
  for keyCode, keyDesc in pairs(KeyCodeDescs) do
    if keyDesc:lower() == key:trim():lower() then
      return keyCode
    end
  end
end

local function retranslateKeyComboDesc(keyComboDesc)
  if keyComboDesc == nil then
    error('Unable to translate key combo \'' .. keyComboDesc .. '\'')
  end

  if type(keyComboDesc) == 'number' then
    keyComboDesc = tostring(keyComboDesc)
  end

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

function determineKeyComboDesc(keyCode, keyboardModifiers)
  local keyCombo = {}
  if keyCode == KeyCtrl or keyCode == KeyShift or keyCode == KeyAlt then
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
  return translateKeyCombo(keyCombo)
end

local function onWidgetKeyDown(widget, keyCode, keyboardModifiers)
  if keyCode == KeyUnknown then return false end
  local callback = widget.boundAloneKeyDownCombos[determineKeyComboDesc(keyCode, KeyboardNoModifier)]
  signalcall(callback, widget, keyCode)
  callback = widget.boundKeyDownCombos[determineKeyComboDesc(keyCode, keyboardModifiers)]
  return signalcall(callback, widget, keyCode)
end

local function onWidgetKeyUp(widget, keyCode, keyboardModifiers)
  if keyCode == KeyUnknown then return false end
  local callback = widget.boundAloneKeyUpCombos[determineKeyComboDesc(keyCode, KeyboardNoModifier)]
  signalcall(callback, widget, keyCode)
  callback = widget.boundKeyUpCombos[determineKeyComboDesc(keyCode, keyboardModifiers)]
  return signalcall(callback, widget, keyCode)
end

local function onWidgetKeyPress(widget, keyCode, keyboardModifiers, autoRepeatTicks)
  if keyCode == KeyUnknown then return false end
  local callback = widget.boundKeyPressCombos[determineKeyComboDesc(keyCode, keyboardModifiers)]
  return signalcall(callback, widget, keyCode, autoRepeatTicks)
end

local function connectKeyDownEvent(widget)
  if widget.boundKeyDownCombos then return end
  connect(widget, { onKeyDown = onWidgetKeyDown })
  widget.boundKeyDownCombos = {}
  widget.boundAloneKeyDownCombos = {}
end

local function connectKeyUpEvent(widget)
  if widget.boundKeyUpCombos then return end
  connect(widget, { onKeyUp = onWidgetKeyUp })
  widget.boundKeyUpCombos = {}
  widget.boundAloneKeyUpCombos = {}
end

local function connectKeyPressEvent(widget)
  if widget.boundKeyPressCombos then return end
  connect(widget, { onKeyPress = onWidgetKeyPress })
  widget.boundKeyPressCombos = {}
end

-- public functions
function g_keyboard.bindKeyDown(keyComboDesc, callback, widget, alone)
  widget = widget or rootWidget
  connectKeyDownEvent(widget)
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  if alone then
    connect(widget.boundAloneKeyDownCombos, keyComboDesc, callback)
  else
    connect(widget.boundKeyDownCombos, keyComboDesc, callback)
  end
end

function g_keyboard.bindKeyUp(keyComboDesc, callback, widget, alone)
  widget = widget or rootWidget
  connectKeyUpEvent(widget)
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  if alone then
    connect(widget.boundAloneKeyUpCombos, keyComboDesc, callback)
  else
    connect(widget.boundKeyUpCombos, keyComboDesc, callback)
  end
end

function g_keyboard.bindKeyPress(keyComboDesc, callback, widget)
  widget = widget or rootWidget
  connectKeyPressEvent(widget)
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  connect(widget.boundKeyPressCombos, keyComboDesc, callback)
end

local function getUnbindArgs(arg1, arg2)
  local callback
  local widget
  if type(arg1) == 'function' then callback = arg1
  elseif type(arg2) == 'function' then callback = arg2 end
  if type(arg1) == 'userdata' then widget = arg1
  elseif type(arg2) == 'userdata' then widget = arg2 end
  widget = widget or rootWidget
  return callback, widget
end

function g_keyboard.unbindKeyDown(keyComboDesc, arg1, arg2)
  local callback, widget = getUnbindArgs(arg1, arg2)
  if widget.boundKeyDownCombos == nil then return end
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  disconnect(widget.boundKeyDownCombos, keyComboDesc, callback)
end

function g_keyboard.unbindKeyUp(keyComboDesc, arg1, arg2)
  local callback, widget = getUnbindArgs(arg1, arg2)
  if widget.boundKeyUpCombos == nil then return end
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  disconnect(widget.boundKeyUpCombos, keyComboDesc, callback)
end

function g_keyboard.unbindKeyPress(keyComboDesc, arg1, arg2)
  local callback, widget = getUnbindArgs(arg1, arg2)
  if widget.boundKeyPressCombos == nil then return end
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  disconnect(widget.boundKeyPressCombos, keyComboDesc, callback)
end

function g_keyboard.getModifiers()
  return g_window.getKeyboardModifiers()
end

function g_keyboard.isKeyPressed(key)
  if type(key) == 'string' then
    key = getKeyCode(key)
  end
  return g_window.isKeyPressed(key)
end

function g_keyboard.isKeySetPressed(keys, all)
  all = all or false
  local result = {}
  for k,v in pairs(keys) do
    if type(v) == 'string' then
      v = getKeyCode(v)
    end
    if g_window.isKeyPressed(v) then
      if not all then
        return true
      end
      table.insert(result, true)
    end
  end
  return #result == #keys
end

function g_keyboard.isInUse()
  for i = FirstKey, LastKey do
    if g_window.isKeyPressed(key) then
      return true
    end
  end
  return false
end

function g_keyboard.isCtrlPressed()
  return bit32.band(g_window.getKeyboardModifiers(), KeyboardCtrlModifier) ~= 0
end

function g_keyboard.isAltPressed()
  return bit32.band(g_window.getKeyboardModifiers(), KeyboardAltModifier) ~= 0
end

function g_keyboard.isShiftPressed()
  return bit32.band(g_window.getKeyboardModifiers(), KeyboardShiftModifier) ~= 0
end
