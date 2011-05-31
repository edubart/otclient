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
            local msgBox = messageBox("Message of the day", motd)
            msgBox.onDestroy = function()
                    enterGameWindow.visible = true
                end
            protocolLogin = nil
        end

    local account = enterGameWindow:child("accountNameTextEdit").text
    local password = enterGameWindow:child("passwordTextEdit").text

    protocolLogin:login(account, password)
end

-- here is where everything starts
if not initialStateLoaded then
    onEnterMenuState()
    App.onClose = onApplicationClose
    initialStateLoaded = true
end
