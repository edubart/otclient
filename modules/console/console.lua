Console = { }

local console
local logLocked = false
local commandEnv = createEnvironment()
local maxLines = 80
local numLines = 0
local commandHistory = { }
local currentHistoryIndex = 0

function Console.onLog(level, message, time)
  -- avoid logging while reporting logs (would cause a infinite loop)
  if not logLocked then
    logLocked = true

    local color
    if level == LogDebug then
      color = '#5555ff'
    elseif level == LogInfo then
      color = '#5555ff'
    elseif level == LogWarning then
      color = '#ffff00'
    else
      color = '#ff0000'
    end

    if level ~= LogDebug then
      Console.addLine(message, color)
    end

    logLocked = false
  end
end

function Console.addLine(text, color)
  -- create new label

  local label = UILabel.create()
  console:insertChild(-2, label)
  label:setId('consoleLabel' .. numLines)
  label:setText(text)
  label:setForegroundColor(color)
  label:setStyle('ConsoleLabel')

  numLines = numLines + 1
  if numLines > maxLines then
    local firstLabel = console:getChildByIndex(1)
    firstLabel:destroy()
  end
end

function Console.navigateCommand(step)
  local numCommands = #commandHistory
  if numCommands > 0 then
    currentHistoryIndex = math.min(math.max(currentHistoryIndex + step, 0), numCommands)
    local commandLineEdit = console:getChildById('commandLineEdit')
    if currentHistoryIndex > 0 then
      local command = commandHistory[numCommands - currentHistoryIndex + 1]
      commandLineEdit:setText(command)
    else
      commandLineEdit:clearText()
    end
  end
end

function Console.create()
  console = UI.loadAndDisplay("/console/console.otui")
  console:hide()
  console.onKeyPress = function(self, keyCode, keyChar, keyboardModifiers)
    if keyboardModifiers == KeyboardNoModifier then
      if keyCode == KeyReturn or keyCode == keyEnter then
        local commandLineEdit = console:getChildById('commandLineEdit')
        local command = commandLineEdit:getText()
        Console.executeCommand(command)
        commandLineEdit:clearText()
        return true
      elseif keyCode == KeyUp then
        Console.navigateCommand(1)
        return true
      elseif keyCode == KeyDown then
        Console.navigateCommand(-1)
        return true
      end
    end
    return false
  end

  Logger.setOnLog(Console.onLog)
  Logger.fireOldMessages()
end

function Console.destroy()
  Logger.setOnLog(nil)
  console:destroy()
  console = nil
end

function Console.show()
  console.parent:lockChild(console)
  console.visible = true
end

function Console.hide()
  console.parent:unlockChild(console)
  console.visible = false
end

function Console.executeCommand(command)
  currentHistoryIndex = 0
  table.insert(commandHistory, command)
  Console.addLine(">> " .. command, "#ffffff")
  local func, err = loadstring(command, "@")
  if func then
    setfenv(func, commandEnv)
    local ok, ret = pcall(func)
    if ok then
      if ret then
        print(ret)
      end
    else
      Logger.log(LogError, 'command failed: ' .. ret)
    end
  else
    Logger.log(LogError, 'incorrect lua syntax: ' .. err:sub(5))
  end
end