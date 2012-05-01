TextBooks = {}

local function onGameEditText(id, itemId, maxLength, text, writter, time)
  local textWindow = createWidget('TextWindow', rootWidget)

  local writeable = maxLength ~= #text
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
  okButton.onClick = function()
    g_game.editText(id, textEdit:getText())
    textWindow:destroy()
  end
end

local function onGameEditList(listId, id, text)
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
