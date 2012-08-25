-- @docclass
g_keyboard = {}

-- private functions
function translateKeyCombo(keyCombo)
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
  table.sort(keyCombo)
  return translateKeyCombo(keyCombo)
end

local function onWidgetKeyDown(widget, keyCode, keyboardModifiers)
  if keyCode == KeyUnknown then return false end
  local keyComboDesc = determineKeyComboDesc(keyCode, keyboardModifiers)
  local callback = widget.boundKeyDownCombos[keyComboDesc]
  if callback then
    callback()
    return true
  end
  return false
end

local function onWidgetKeyPress(widget, keyCode, keyboardModifiers, autoRepeatTicks)
  if keyCode == KeyUnknown then return false end
  local keyComboDesc = determineKeyComboDesc(keyCode, keyboardModifiers)
  local comboConf = widget.boundKeyPressCombos[keyComboDesc]
  if comboConf and (autoRepeatTicks >= comboConf.autoRepeatDelay or autoRepeatTicks == 0) and comboConf.callback then
    comboConf.callback()
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
function g_keyboard.bindKeyDown(keyComboDesc, callback, widget)
  widget = widget or rootWidget
  connectKeyDownEvent(widget)
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  if widget.boundKeyDownCombos[keyComboDesc] then
    pwarning('KeyDown event \'' .. keyComboDesc .. '\' redefined on widget ' .. widget:getId())
  end
  widget.boundKeyDownCombos[keyComboDesc] = callback
end

function g_keyboard.bindKeyPress(keyComboDesc, callback, widget, autoRepeatDelay)
  autoRepeatDelay = autoRepeatDelay or 500
  widget = widget or rootWidget
  connectKeyPressEvent(widget)
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  if widget.boundKeyPressCombos[keyComboDesc] then
    pwarning('KeyPress event \'' .. keyComboDesc .. '\' redefined on widget ' .. widget:getId())
  end
  widget.boundKeyPressCombos[keyComboDesc] = { callback = callback, autoRepeatDelay = autoRepeatDelay }
  widget:setAutoRepeatDelay(math.min(autoRepeatDelay, widget:getAutoRepeatDelay()))
end

function g_keyboard.unbindKeyDown(keyComboDesc, widget)
  widget = widget or rootWidget
  if widget.boundKeyDownCombos == nil then return end
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  if keyComboDesc then
    widget.boundKeyDownCombos[keyComboDesc] = nil
  end
end

function g_keyboard.unbindKeyPress(keyComboDesc, widget)
  widget = widget or rootWidget
  if widget.boundKeyPressCombos == nil then return end
  local keyComboDesc = retranslateKeyComboDesc(keyComboDesc)
  if keyComboDesc then
    widget.boundKeyPressCombos[keyComboDesc] = nil
  end
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

function g_keyboard.isCtrlPressed()
  return bit32.band(g_window.getKeyboardModifiers(), KeyboardCtrlModifier) ~= 0
end

function g_keyboard.isAltPressed()
  return bit32.band(g_window.getKeyboardModifiers(), KeyboardAltModifier) ~= 0
end

function g_keyboard.isShiftPressed()
  return bit32.band(g_window.getKeyboardModifiers(), KeyboardShiftModifier) ~= 0
end
