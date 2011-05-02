function messageBox(title, text)
    local msgbox = UI.load("modules/messagebox/messagebox.yml")
    local label = msgbox:getChildById("messageBoxLabel")
    local okButton = msgbox:getChildById("messageBoxOkButton")
    msgbox.locked = true
    msgbox.title = title
    label.text = text
    okButton.onClick = function() msgbox:destroy() end
end
