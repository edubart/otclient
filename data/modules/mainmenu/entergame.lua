function EnterGame_connectToLoginServer()
  -- create login protocol
  local protocolLogin = ProtocolLogin.new()

  -- used to recreate enter game window
  local recreateEnterGame = function()
    loadUI("ui/entergamewindow")
  end

  -- display loading message box
  local loadBox = displayCancelBox("Please wait", "Connecting..")

  -- cancel loading callback
  loadBox.onCancel = function()
    -- cancel protocol and reacreate enter game window
    protocolLogin:cancel()
    recreateEnterGame()
  end

  -- error callback
  protocolLogin.onError = function(error)
    -- destroy loading message box
    loadBox:destroy()

    -- display error message
    local errorBox = displayErrorBox("Login Error", error)

    -- cancel protocol and reacreate enter game window
    self.cancel()
    errorBox.onOk = recreateEnterGame
  end

  -- motd callback
  protocolLogin.onMotd = function(motd)
    -- destroy loading message box
    loadBox:destroy()

    -- display motd
    local motdNumber = string.sub(motd, 0, string.find(motd, "\n"))
    local motdText = string.sub(motd, string.find(motd, "\n") + 1, string.len(motd))
    local motdBox = displayInfoBox("Message of the day", motdText)

    -- cancel protocol and reacreate enter game window
    self.cancel()
    motdBox.onOk = recreateEnterGame
  end

  -- get account and password and login
  local enterGameWindow = rootUI:child("enterGameWindow")
  local account = enterGameWindow:child("accountNameTextEdit").text
  local password = enterGameWindow:child("passwordTextEdit").text
  protocolLogin:login(account, password)

  -- destroy enter game window
  enterGameWindow:destroy()
end

