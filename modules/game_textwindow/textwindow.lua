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
  local i = 0
  for n = 1, count-1 do
    local tPos = string.find(str, '\n', i)
    if tPos then
      i = tPos+1
    end
  end

  return i - 1
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
  
  local _, newLineCount = string.gsub(text, '\n', '\n')
  if(newLineCount >= 9) then
    textScroll:setMaximum(newLineCount-9)
  end
    
  local _prev, _next, _current = 0, 11, 0
  local onValueChange = function()
  local diff = textScroll:getValue() - _current
  
  if(diff > 0) then
    textEdit:setCursorPos(getCursorPosByNewLine(text, _next+(diff-1)))
  else
    textEdit:setCursorPos(getCursorPosByNewLine(text, _prev+(diff+1)))
  end
  
  _current = textScroll:getValue()
  _next = _next + diff
  _prev = _prev + diff
  end
  
  textScroll.onValueChange = onValueChange
  g_keyboard.bindKeyPress("Up", function() textScroll:setValue(textScroll:getValue()-1) end, textWindow, 400)
  g_keyboard.bindKeyPress("Down", function() textScroll:setValue(textScroll:getValue()+1) end, textWindow, 400)
  
  if(not writeable) then
    textEdit:setCursorPos(0)
  else
    textScroll:setValue(textScroll:getMaximum())
    textEdit:setCursorPos(text:len())
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
