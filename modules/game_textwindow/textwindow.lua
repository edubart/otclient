local windows = {}

function init()
  g_ui.importStyle('textwindow')

  connect(g_game, { onEditText = onGameEditText,
                    onEditList = onGameEditList,
                    onGameEnd = destroyWindows })
end

function terminate()
  disconnect(g_game, { onEditText = onGameEditText,
                       onEditList = onGameEditList,
                       onGameEnd = destroyWindows })

  destroyWindows()
end

function destroyWindows()
  for _,window in pairs(windows) do
    window:destroy()
  end
  windows = {}
end

function onGameEditText(id, itemId, maxLength, text, writer, time)
  local textWindow = g_ui.createWidget('TextWindow', rootWidget)

  local writeable = #text < maxLength and maxLength > 0
  local textItem = textWindow:getChildById('textItem')
  local description = textWindow:getChildById('description')
  local textEdit = textWindow:getChildById('text')
  local okButton = textWindow:getChildById('okButton')
  local cancelButton = textWindow:getChildById('cancelButton')

  local textScroll = textWindow:getChildById('textScroll')

  if textItem:isHidden() then
    textItem:show()
  end

  textItem:setItemId(itemId)
  textEdit:setMaxLength(maxLength)
  textEdit:setText(text)
  textEdit:setEditable(writeable)
  textEdit:setCursorVisible(writeable)

  local desc = ''
  if #writer > 0 then
    desc = tr('You read the following, written by \n%s\n', writer)
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
    cancelButton:hide()
    cancelButton:setWidth(0)
    okButton:setMarginRight(0)
  else
    textWindow:setText(tr('Edit Text'))
  end

  if description:getHeight() < 64 then
    description:setHeight(64)
  end

  local function destroy()
    textWindow:destroy()
    table.removevalue(windows, textWindow)
  end

  local doneFunc = function()
    if writeable then
      g_game.editText(id, textEdit:getText())
    end
    destroy()
  end

  okButton.onClick = doneFunc
  cancelButton.onClick = destroy

  if not writeable then
    textWindow.onEnter = doneFunc
  end

  textWindow.onEscape = destroy

  table.insert(windows, textWindow)
end

function onGameEditList(id, doorId, text)
  local textWindow = g_ui.createWidget('TextWindow', rootWidget)

  local textEdit = textWindow:getChildById('text')
  local description = textWindow:getChildById('description')
  local okButton = textWindow:getChildById('okButton')
  local cancelButton = textWindow:getChildById('cancelButton')

  local textItem = textWindow:getChildById('textItem')
  if textItem and not textItem:isHidden() then
    textItem:hide()
  end

  textEdit:setMaxLength(8192)
  textEdit:setText(text)
  textEdit:setEditable(true)
  description:setText(tr('Enter one name per line.'))
  textWindow:setText(tr('Edit List'))

  if description:getHeight() < 64 then
    description:setHeight(64)
  end

  local function destroy()
    textWindow:destroy()
    table.removevalue(windows, textWindow)
  end

  local doneFunc = function()
    g_game.editList(id, doorId, textEdit:getText())
    destroy()
  end

  okButton.onClick = doneFunc
  cancelButton.onClick = destroy
  textWindow.onEscape = destroy

  table.insert(windows, textWindow)
end
