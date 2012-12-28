function init()
  g_ui.importStyle('textwindow.otui')

  connect(g_game, { onEditText = onGameEditText,
                    onEditList = onGameEditList,
                    onGameEnd = destroy })
end

function terminate()
  disconnect(g_game, { onEditText = onGameEditText,
                       onEditList = onGameEditList,
                       onGameEnd = destroy })

  destroy()
end

function destroy()
  if textWindow then
    textWindow:destroy()
    textWindow = nil
  end
end

function getCursorPosByNewLine(str, count)
  if count <= 1 then return 0 end
  
  local i = 0
  for n = 1, count-1 do
    local tPos = string.find(str, '\n', i)
    if tPos then
      i = tPos+1
    end
  end

  return i - 1
end

function getLineByCursorPos(str, pos, maxLine)
  for i = 1, maxLine do
    if pos <= getCursorPosByNewLine(str, i) then
      return i
    end
  end
  
  return maxLine + 1
end


function getLineSizeByCursorPos(str, pos, maxLine)
  for i = 1, maxLine + 1 do
    if pos < getCursorPosByNewLine(str, i) then
      return {minPos = getCursorPosByNewLine(str, i-1), maxPos = (getCursorPosByNewLine(str, i) - 1)}
    end
  end
  
  return {minPos = getCursorPosByNewLine(str, maxLine + 1), maxPos = str:len()}
end

function string.count(str, pattern)
  local _, _count = string.gsub(str, pattern, pattern)
  return _count
end

function onGameEditText(id, itemId, maxLength, text, writter, time)
  if textWindow then return end
  textWindow = g_ui.createWidget('TextWindow', rootWidget)

  local writeable = (maxLength ~= #text) and maxLength > 0
  local textItem = textWindow:getChildById('textItem')
  local description = textWindow:getChildById('description')
  local textEdit = textWindow:getChildById('text')
  local okButton = textWindow:getChildById('okButton')
  local cancelButton = textWindow:getChildById('cancelButton')

  local textScroll = textWindow:getChildById('textScroll')

  textItem:setItemId(itemId)
  textEdit:setMaxLength(maxLength)
  textEdit:setText(text)
  textEdit:setEnabled(writeable)
  
  local desc = ''
  if #writter > 0 then
    desc = tr('You read the following, written by \n%s\n', writter)
    if #time > 0 then
      desc = desc .. tr('on %s.\n', time)
    end
  elseif #time > 0 then
    desc = tr('You read the following, written on \n%s.\n', time)
  end

  if #text == 0 and not writeable then
    desc = tr("It is empty.")
  elseif writeable then
    desc = desc .. tr('You can enter new text.')
  end

  local lines = #{string.find(desc, '\n')}
  if lines < 2 then desc = desc .. '\n' end

  description:setText(desc)

  if not writeable then
    textWindow:setText(tr('Show Text'))
    --textEdit:wrapText()
    cancelButton:hide()
    cancelButton:setWidth(0)
    okButton:setMarginRight(0)
  else
    textWindow:setText(tr('Edit Text'))
  end

  local doneFunc = function()
    if writeable then
      g_game.editText(id, textEdit:getText())
    end
    destroy()
  end
  
  local newLineCount = string.count(textEdit:getText(), '\n')
  if newLineCount >= 9 then
    textScroll:setMaximum(newLineCount-9)
  end
    
  local _prev, _next = 0, 11
  local scrollOnValueChange = function(widget, value, delta)
    local line = getLineByCursorPos(textEdit:getText(), textEdit:getCursorPos(), newLineCount)
    if delta > 0 then
      textEdit:setCursorPos(getCursorPosByNewLine(textEdit:getText(), _next + delta - 1))
      if writeable then textEdit:setCursorPos(getCursorPosByNewLine(textEdit:getText(), line + delta)) end
    else
      textEdit:setCursorPos(getCursorPosByNewLine(textEdit:getText(), _prev + delta + 1) - 1)
      if writeable then textEdit:setCursorPos(getCursorPosByNewLine(textEdit:getText(), line + delta)) end
    end
  
    _next = _next + delta
    _prev = _prev + delta
  end
  
  textScroll.onValueChange = scrollOnValueChange
  
  local navigateVertical = function(up)   -- Pressing Up / Down when scrollbar is at min / max value
    local line = getLineByCursorPos(textEdit:getText(), textEdit:getCursorPos(), newLineCount)
    if up and textScroll:getValue() == textScroll:getMinimum() then
      textEdit:setCursorPos(getCursorPosByNewLine(textEdit:getText(), line - 1))
    elseif not up and textScroll:getValue() == textScroll:getMaximum() then
      textEdit:setCursorPos(getCursorPosByNewLine(textEdit:getText(), line + 1))
    end
  end
  
  local navigateHorizontal = function(right)  -- Pressing Left / Right to navigate in a line
    local currentCursor = textEdit:getCursorPos()
    local lineSize = getLineSizeByCursorPos(textEdit:getText(), currentCursor, newLineCount)
    if right and currentCursor < lineSize.maxPos then
      textEdit:setCursorPos(currentCursor+1)
    elseif not right and currentCursor > lineSize.minPos then
      textEdit:setCursorPos(currentCursor-1)
    end
  end
  
  local onKeyPress = function(widget, keyCode, keyModifiers)
    if keyModifiers ~= 0 then
      return false
    end

    if keyCode == 16 or keyCode == 17 then  -- Left / Right
      navigateHorizontal((keyCode == 17))  
      return true
    elseif keyCode == 14 or keyCode == 15 then -- Up / Down
      local up = (keyCode == 14)
      navigateVertical(up)
      
      if up then
        textScroll:setValue(textScroll:getValue() - 1)
      else
        textScroll:setValue(textScroll:getValue() + 1)
      end
      return true
    end
    return false
  end
  
  if not writeable then
    textEdit:setCursorPos(0)
    textWindow.onKeyPress = onKeyPress  -- textEdit won't receive focus
  else
    textScroll:setValue(textScroll:getMaximum())
    textEdit:setCursorPos(text:len())
    textEdit.onKeyPress = onKeyPress
  end
  
  okButton.onClick = doneFunc
  cancelButton.onClick = destroy
  --textWindow.onEnter = doneFunc -- this should be '\n'
  textWindow.onEscape = destroy
end

function onGameEditList(id, doorId, text)
  if textWindow then return end
  textWindow = g_ui.createWidget('TextWindow', rootWidget)

  local textEdit = textWindow:getChildById('text')
  local description = textWindow:getChildById('description')
  local okButton = textWindow:getChildById('okButton')
  local cancelButton = textWindow:getChildById('cancelButton')

  textEdit:setMaxLength(8192)
  textEdit:setText(text)
  textEdit:setEnabled(true)
  description:setText(tr('Enter one name per line.'))
  textWindow:setText(tr('Edit List'))

  doneFunc = function()
    g_game.editList(id, doorId, textEdit:getText())
    destroy()
  end

  okButton.onClick = doneFunc
  textWindow.onEnter = doneFunc
  textWindow.onEscape = destroy
end


function onGameEditList(id, doorId, text)
  if textWindow then return end
  textWindow = g_ui.createWidget('TextWindow', rootWidget)

  local textEdit = textWindow:getChildById('text')
  local description = textWindow:getChildById('description')
  local okButton = textWindow:getChildById('okButton')
  local cancelButton = textWindow:getChildById('cancelButton')

  textEdit:setMaxLength(8192)
  textEdit:setText(text)
  textEdit:setEnabled(true)
  description:setText(tr('Enter one name per line.'))
  textWindow:setText(tr('Edit List'))

  doneFunc = function()
    g_game.editList(id, doorId, textEdit:getText())
    destroy()
  end

  okButton.onClick = doneFunc
  textWindow.onEnter = doneFunc
  textWindow.onEscape = destroy
end
