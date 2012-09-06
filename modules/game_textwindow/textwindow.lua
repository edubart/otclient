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

function onGameEditText(id, itemId, maxLength, text, writter, time)
  if textWindow then return end
  textWindow = g_ui.createWidget('TextWindow', rootWidget)

  local writeable = (maxLength ~= #text) and maxLength > 0
  local textItem = textWindow:getChildById('textItem')
  local description = textWindow:getChildById('description')
  local textEdit = textWindow:getChildById('text')
  local okButton = textWindow:getChildById('okButton')
  local cancelButton = textWindow:getChildById('cancelButton')

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
