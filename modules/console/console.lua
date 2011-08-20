Console = { }
local console
local logLocked = false

function Console.onLog(level, message, time)
  -- avoid logging while reporting logs (would cause a infinite loop)
  if not logLocked then
    logLocked = true

    local color
    if level == LogDebug then
      color = '#5555ff'
    elseif level == LogInfo then
      color = '#55ff55'
    elseif level == LogWarning then
      color = '#ffff00'
    else
      color = '#ff0000'
    end

    Console.addLine(message, color)

    logLocked = false
  end
end

function Console.addLine(text, color)
  -- create new label
  local label = UILabel.create()
  label:setStyle('ConsoleLabel')
  label:setText(text)
  label:setForegroundColor(color)

  console:insertChild(label, -1)
end

function Console.create()
  console = loadUI("/console/console.otui")
  rootWidget:addChild(console)
  console:hide()

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
  Console.addLine(">> " .. command, "#ffffff")
  local func, err = loadstring("return (" .. command .. ")", "@")
  if func then
    local ok, ret = pcall(func)
    if ok then
      Logger.log(LogDebug, "=> " .. tostring(ret))
    else
      Logger.log(LogError, 'command failed: ' .. ret)
    end
  else
    Logger.log(LogError, 'incorrect lua syntax: ' .. err:sub(5))
  end
end