Console = createEnvironment()
setfenv(1, Console)

-- public variables
LogColors = { [LogInfo] = 'white',
              [LogWarning] = 'yellow',
              [LogError] = 'red' }
MaxLogLines = 80

-- private variables
local consoleWidget
local logLocked = false
local commandEnv = createEnvironment()
local commandLineEdit
local commandHistory = { }
local currentHistoryIndex = 0

-- private functions
local function navigateCommand(step)
  local numCommands = #commandHistory
  if numCommands > 0 then
    currentHistoryIndex = math.min(math.max(currentHistoryIndex + step, 0), numCommands)
    if currentHistoryIndex > 0 then
      local command = commandHistory[numCommands - currentHistoryIndex + 1]
      commandLineEdit:setText(command)
    else
      commandLineEdit:clearText()
    end
  end
end

local function onKeyPress(widget, keyCode, keyChar, keyboardModifiers)
  if keyboardModifiers == KeyboardNoModifier then
    -- execute current command
    if keyCode == KeyReturn or keyCode == keyEnter then
      local currentCommand = commandLineEdit:getText()
      executeCommand(currentCommand)
      commandLineEdit:clearText()
      return true
    -- navigate history up
    elseif keyCode == KeyUp then
      navigateCommand(1)
      return true
    -- navigate history down
    elseif keyCode == KeyDown then
      navigateCommand(-1)
      return true
    end
  end
  return false
end

local function onLog(level, message, time)
  -- debug messages are ignored
  if level == LogDebug then return end

  -- avoid logging while reporting logs (would cause a infinite loop)
  if logLocked then return end

  logLocked = true
  addLine(message, LogColors[level])
  logLocked = false
end

-- public functions
function init()
  consoleWidget = UI.loadAndDisplay("/console/console.otui")
  consoleWidget:hide()
  consoleWidget.onKeyPress = onKeyPress

  commandLineEdit = consoleWidget:getChildById('commandLineEdit')
  Logger.setOnLog(onLog)
  Logger.fireOldMessages()
end

function terminate()
  Logger.setOnLog(nil)
  consoleWidget:destroy()
  commandLineEdit = nil
  consoleWidget = nil
end

function addLine(text, color)
  -- create new line label
  local numLines = consoleWidget:getChildCount() - 2
  local label = UILabel.create()
  consoleWidget:insertChild(-2, label)
  label:setId('consoleLabel' .. numLines)
  label:setStyle('ConsoleLabel')
  label:setText(text)
  label:setForegroundColor(color)

  -- delete old lines if needed
  if numLines > MaxLogLines then
    consoleWidget:getChildByIndex(1):destroy()
  end
end

function executeCommand(command)
  -- reset current history index
  currentHistoryIndex = 0

  -- add new command to history
  table.insert(commandHistory, command)

  -- add command line
  addLine(">> " .. command, "#ffffff")

  -- load command buffer
  local func, err = loadstring(command, "@")

  -- check for syntax errors
  if not func then
    Logger.log(LogError, 'incorrect lua syntax: ' .. err:sub(5))
    return
  end

  -- setup func env to commandEnv
  setfenv(func, commandEnv)

  -- execute the command
  local ok, ret = pcall(func)
  if ok then
    -- if the command returned a value, print it
    if ret then print(ret) end
  else
    Logger.log(LogError, 'command failed: ' .. ret)
  end
end

