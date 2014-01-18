modalDialog = nil

function init()
  g_ui.importStyle('modaldialog')

  connect(g_game, { onModalDialog = onModalDialog,
                    onGameEnd = destroyDialog })

  local dialog = rootWidget:recursiveGetChildById('modalDialog')
  if dialog then
    modalDialog = dialog
  end
end

function terminate()
  disconnect(g_game, { onModalDialog = onModalDialog,
                       onGameEnd = destroyDialog })
end

function destroyDialog()
  if modalDialog then
    modalDialog:destroy()
    modalDialog = nil
  end
end

function onModalDialog(id, title, message, buttons, enterButton, escapeButton, choices, priority)
  -- priority parameter is unused, not sure what its use is.
  if modalDialog then
    return
  end

  modalDialog = g_ui.createWidget('ModalDialog', rootWidget)

  local messageLabel = modalDialog:getChildById('messageLabel')
  local choiceList = modalDialog:getChildById('choiceList')
  local choiceScrollbar = modalDialog:getChildById('choiceScrollBar')
  local buttonList = modalDialog:getChildById('buttonList')

  modalDialog:setText(title)
  messageLabel:setText(message)

  local horizontalPadding = modalDialog:getPaddingLeft() + modalDialog:getPaddingRight()
  modalDialog:setWidth(math.min(modalDialog.maximumWidth, math.max(messageLabel:getWidth(), modalDialog.minimumWidth)))
  messageLabel:setWidth(math.min(modalDialog.maximumWidth, math.max(messageLabel:getWidth(), modalDialog.minimumWidth)) - horizontalPadding)

  local labelHeight = nil
  for i = 1, #choices do
    local choiceId = choices[i][1]
    local choiceName = choices[i][2]

    local label = g_ui.createWidget('ChoiceListLabel', choiceList)
    label.choiceId = choiceId
    label:setText(choiceName)
    label:setPhantom(false)
    if not labelHeight then
      labelHeight = label:getHeight()
    end
  end
  choiceList:focusNextChild()

  for i = 1, #buttons do
    local buttonId = buttons[i][1]
    local buttonText = buttons[i][2]

    local button = g_ui.createWidget('ModalButton', buttonList)
    button:setText(buttonText)
    button.onClick = function(self)
                       local focusedChoice = choiceList:getFocusedChild()
                       local choice = 0xFF
                       if focusedChoice then
                         choice = focusedChoice.choiceId
                       end
                       g_game.answerModalDialog(id, buttonId, choice)
                       destroyDialog()
                     end
  end

  local additionalHeight = 0
  if #choices > 0 then
    choiceList:setVisible(true)
    choiceScrollbar:setVisible(true)

    additionalHeight = math.min(modalDialog.maximumChoices, math.max(modalDialog.minimumChoices, #choices)) * labelHeight
    additionalHeight = additionalHeight + choiceList:getPaddingTop() + choiceList:getPaddingBottom()
  end
  modalDialog:setHeight(modalDialog:getHeight() + additionalHeight)

  addEvent(function()
             modalDialog:setHeight(modalDialog:getHeight() + messageLabel:getHeight() - 14)
           end)

  local enterFunc = function()
    local focusedChoice = choiceList:getFocusedChild()
    local choice = 0xFF
    if focusedChoice then
      choice = focusedChoice.choiceId
    end
    g_game.answerModalDialog(id, enterButton, choice)
    destroyDialog()
  end

  local escapeFunc = function()
    local focusedChoice = choiceList:getFocusedChild()
    local choice = 0xFF
    if focusedChoice then
      choice = focusedChoice.choiceId
    end
    g_game.answerModalDialog(id, escapeButton, choice)
    destroyDialog()
  end

  choiceList.onDoubleClick = enterFunc

  modalDialog.onEnter = enterFunc
  modalDialog.onEscape = escapeFunc
end