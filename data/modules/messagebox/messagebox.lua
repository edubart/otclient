function autoDestroyParent()
    self.parent:destroy()
end

function messageBox(title, text)
    local msgBox = UI.load("messagebox.yml")
    msgBox.locked = true
    msgBox.title = title
    msgBox:child("textLabel").text = text
    msgBox:child("okButton").onClick = autoDestroyParentz
end
