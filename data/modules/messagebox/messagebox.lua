MessageBox = {}
MessageBox.__index = MessageBox

function MessageBox.create(title, text)
    local msgBox = {}
    setmetatable(msgBox, MessageBox)

    local window = UI.load("messagebox.yml")
    window.locked = true
    window.title = title
    window:child("textLabel").text = text
    window:child("okButton").onClick = function()
            self.parent:destroy()
        end
    window.onDestroy = function()
            if msgBox.onDestroy then
                msgBox.onDestroy()
            end
        end

    msgBox.window = window
    return msgBox
end

function MessageBox:destroy()
    if self.window then
        self.window:destroy()
        self.window = nil
    end
end

function messageBox(title, text)
    return MessageBox.create(title, text)
end
