function messageBox(title, text)
    local messageBoxWindow = UI.load("modules/messagebox/messagebox.yml")
    local messageBoxLabel = messageBoxWindow:getChildById("messageBoxLabel")
    local messageBoxOkButton = messageBoxWindow:getChildById("messageBoxOkButton")
    messageBoxWindow.locked = true
    messageBoxWindow.title = title
    messageBoxLabel.text = text
    --messageBoxWindow:setSize(messageBoxLabel:getSize() + Size{20, 20})
    messageBoxWindow.onDestroy = function() self.locked = false end
    messageBoxOkButton.onClick = function() messageBoxWindow:destroy() end
end
