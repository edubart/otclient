function EnterGame_connectToLoginServer()
  -- create login protocol
  local protocolLogin = ProtocolLogin.create()
  print(ProtocolLogin_mt)
  print(Protocol_mt)
  print(getmetatable(protocolLogin))

  -- used to recreate enter game window
  local recreateEnterGame = function()
    loadUI("ui/entergamewindow")
  end

  -- display loading message box
  local loadBox = displayCancelBox("Please wait", "Connecting..")

  -- cancel loading callback
  loadBox.onCancel = function(protocol)
    -- cancel protocol and reacreate enter game window
    protocol:cancel()
    recreateEnterGame()
  end

  -- error callback
  protocolLogin.onError = function(protocol, error)
    -- destroy loading message box
    loadBox:destroy()

    -- display error message
    local errorBox = displayErrorBox("Login Error", error)

    -- cancel protocol and reacreate enter game window
    protocol.cancel()
    errorBox.onOk = recreateEnterGame
  end

  -- motd callback
  protocolLogin.onMotd = function(protocol, motd)
    -- destroy loading message box
    loadBox:destroy()

    -- display motd
    local motdNumber = string.sub(motd, 0, string.find(motd, "\n"))
    local motdText = string.sub(motd, string.find(motd, "\n") + 1, string.len(motd))
    local motdBox = displayInfoBox("Message of the day", motdText)

    -- cancel protocol and reacreate enter game window
    protocol.cancel()
    motdBox.onOk = recreateEnterGame
  end

  -- get account and password then login
  local enterGameWindow = rootUI:getChild("enterGameWindow")
  local account = enterGameWindow:getChild("accountNameTextEdit").text
  local password = enterGameWindow:getChild("passwordTextEdit").text
  protocolLogin:login(account, password)

  -- destroy enter game window
  enterGameWindow:destroy()
end

