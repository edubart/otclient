function init()
  g_ui.importStyle('modaldialog')

  connect(g_game, { onModalDialog = onModalDialog,
                    onGameEnd = destroy })
end

function terminate()
  disconnect(g_game, { onModalDialog = onModalDialog,
                       onGameEnd = destroy })

  destroy()
end

function destroy()
  if modalDialog then
    modalDialog:destroy()
    modalDialog = nil
  end
end

function onModalDialog(id, title, message, enterId, enterText, escapeId, escapeText, choices) 
  if modalDialog then return end
  
  modalDialog = g_ui.createWidget('ModalDialog', rootWidget)
  
  local enterButton = modalDialog:getChildById('enterButton')
  local escapeButton = modalDialog:getChildById('escapeButton')
  local messageLabel = modalDialog:getChildById('messageLabel')
  local choiceList = modalDialog:getChildById('choiceList')
  
  modalDialog:setText(title)
  messageLabel:setText(message)
  enterButton:setText(enterText)
  escapeButton:setText(escapeText)
  
  local focusLabel = nil
  for k, v in pairs(choices) do
    local choiceId = v[1]
    local choiceName = v[2]
    
    local label = g_ui.createWidget('ChoiceListLabel', choiceList)
    label.choiceId = choiceId
    label:setText(choiceName)
    label:setPhantom(false)
    
    if not focusLabel then
      focusLabel = label
    end
  end
  choiceList:focusChild(focusLabel)

  local enterFunc = function()
    g_game.answerModalDialog(id, enterId, choiceList:getFocusedChild().choiceId)
    destroy()
  end
  
  local escapeFunc = function()
    g_game.answerModalDialog(id, escapeId, choiceList:getFocusedChild().choiceId)
    destroy()
  end

  choiceList.onDoubleClick = enterFunc
  
  enterButton.onClick = enterFunc
  modalDialog.onEnter = enterFunc
  
  escapeButton.onClick = escapeFunc
  modalDialog.onEscape = escapeFunc
  return
end