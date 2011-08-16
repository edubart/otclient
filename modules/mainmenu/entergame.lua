function EnterGame_connectToLoginServer()
  -- create login protocol
  local protocolLogin = ProtocolLogin.create()

  -- used to recreate enter game window
  local recreateEnterGame = function()
    loadUI("/mainmenu/ui/entergamewindow.otui")
  end

  -- display loading message box
  local loadBox = displayCancelBox("Please wait", "Connecting..")

  -- cancel loading callback
  loadBox.onCancel = function(msgbox)
    -- cancel protocol and reacreate enter game window
    protocolLogin:cancel()
    recreateEnterGame()
  end

  -- error callback
  protocolLogin.onError = function(protocol, error)
    -- destroy loading message box
    loadBox:destroy()

    -- display error message
    local errorBox = displayErrorBox("Login Error", error)

    -- cancel protocol and reacreate enter game window
    protocol:cancel()
    errorBox.onOk = recreateEnterGame
  end

  -- motd callback
  protocolLogin.onMotd = function(protocol, motd)
    -- destroy loading message box
    loadBox:destroy()

    -- display motd
    local motdNumber = string.sub(motd, 0, string.find(motd, "\n"))
    local motdText = string.sub(motd, string.find(motd, "\n") + 1, string.len(motd))
    --local motdBox = displayInfoBox("Message of the day", motdText)

    -- hide main menu
    mainMenu.visible = false
  end

  -- get account and password then login
  local enterGameWindow = rootWidget:getChild("enterGameWindow")
  local account = enterGameWindow:getChild("accountNameLineEdit").text
  local password = enterGameWindow:getChild("accountPasswordLineEdit").text
  protocolLogin:login(account, password)

  -- destroy enter game window
  enterGameWindow:destroy()
end
