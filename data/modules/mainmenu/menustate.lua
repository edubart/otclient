-- menu state
function onEnterMenuState()
    mainMenu = UI.load("mainmenu.yml")
end

function onLeaveMenuState()

end

function onApplicationClose()
    onLeaveMenuState()
    App.exit()
end

function enterGame_onOkClicked()
    local enterGameWindow = UI.getRootContainer():child("enterGameWindow")
    enterGameWindow.visible = false

    local loadMessageBox = messageBox("Please wait", "Connecting..")
    loadMessageBox.onDestroy = function()
            --TODO: cancel protocol
            enterGameWindow.visible = true
            protocolLogin = nil
        end

    protocolLogin = ProtocolLogin.new()
    protocolLogin.onError = function(error)
            loadMessageBox.onDestroy = nil
            loadMessageBox:destroy()
            local msgBox = messageBox("Login Error", error)
            msgBox.onDestroy = function()
                    enterGameWindow.visible = true
                end
            protocolLogin = nil
        end
    protocolLogin.onMotd = function(motd)
            loadMessageBox.onDestroy = nil
            loadMessageBox:destroy()

            local motdNumber = string.sub(motd, 0, string.find(motd, "\n"))
            local motdText = string.sub(motd, string.find(motd, "\n") + 1, string.len(motd))

            local msgBox = messageBox("Message of the day", motdText)
            msgBox.onDestroy = function()
                    enterGameWindow.visible = true
                end
            protocolLogin = nil
        end

    local account = enterGameWindow:child("accountNameTextEdit").text
    local password = enterGameWindow:child("passwordTextEdit").text
    account = "tibialua0"
    password = "lua123456"

    protocolLogin:login(account, password)
end

-- here is where everything starts
if not initialStateLoaded then
    onEnterMenuState()
    App.onClose = onApplicationClose
    initialStateLoaded = true
end
