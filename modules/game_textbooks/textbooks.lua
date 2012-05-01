TextBooks = {}

local function onGameEditText(id, itemId, maxLength, text, writter, time)
  local textWindow = createWidget('TextWindow', rootWidget)

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

  local desc = tr('You read the following')
  if #writter > 0 then
    desc = desc .. tr(', written by \n%s\n', writter)

    if #time > 0 then
      desc = desc .. tr('on %s.\n', time)
    end
  elseif #time > 0 then
    desc = desc .. tr(', written on %s.\n', time)
  end

  if #text == 0 and not writeable then
    desc = tr("It is empty.\n")
  elseif writeable then
    desc = desc .. tr('You can enter new text.')
  end

  description:setText(desc)

  if not writeable then
    textWindow:setText(tr('Show Text'))
    cancelButton:hide()
  else
    textWindow:setText(tr('Edit Text'))
  end

  okButton.onClick = function()
    if writeable then
      g_game.editText(id, textEdit:getText())
    end
    textWindow:destroy()
  end
end

local function onGameEditList(id, doorId, text)
  local textWindow = createWidget('TextWindow', rootWidget)

  local textEdit = textWindow:getChildById('text')
  local description = textWindow:getChildById('description')
  local okButton = textWindow:getChildById('okButton')
  local cancelButton = textWindow:getChildById('cancelButton')

  textEdit:setMaxLength(8192)
  textEdit:setText(text)
  textEdit:setEnabled(true)
  description:setText(tr('Enter one text per line.'))
  textWindow:setText(tr('Edit List'))

  okButton.onClick = function()
    g_game.editList(id, doorId, textEdit:getText())
    textWindow:destroy()
  end
end

function TextBooks.init()
  importStyle 'textwindow.otui'

  connect(g_game, { onEditText = onGameEditText })
  connect(g_game, { onEditList = onGameEditList })
end

function TextBooks.terminate()
  disconnect(g_game, { onEditText = onGameEditText })
  disconnect(g_game, { onEditList = onGameEditList })
end
