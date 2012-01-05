Console = { }

-- configs
local LogColors = { [LogInfo] = 'white',
                    [LogWarning] = 'yellow',
                    [LogError] = 'red' }
local MaxLogLines = 80
local LabelHeight = 16

-- private variables
local consoleWidget
local logLocked = false
local commandEnv = newenv()
local commandLineEdit
local consoleBuffer
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

local function completeCommand()
  local cursorPos = commandLineEdit:getCursorPos()
  if cursorPos == 0 then return end

  local commandBegin = commandLineEdit:getText():sub(1, cursorPos)
  local possibleCommands = {}

  -- create a list containing all globals
  local allVars = table.copy(_G)
  table.merge(allVars, commandEnv)

  -- match commands
  for k,v in pairs(allVars) do
    if k:sub(1, cursorPos) == commandBegin then
      table.insert(possibleCommands, k)
    end
  end

  -- complete command with one match
  if #possibleCommands == 1 then
    commandLineEdit:setText(possibleCommands[1])
  -- show command matches
  elseif #possibleCommands > 0 then
    print('>> ' .. commandBegin)

    -- expand command
    local expandedComplete = commandBegin
    local done = false
    while not done do
      cursorPos = #commandBegin+1
      if #possibleCommands[1] < cursorPos then
        break
      end
      expandedComplete = commandBegin .. possibleCommands[1]:sub(cursorPos, cursorPos)
      for i,v in ipairs(possibleCommands) do
        if v:sub(1, #expandedComplete) ~= expandedComplete then
          done = true
        end
      end
      if not done then
        commandBegin = expandedComplete
      end
    end
    commandLineEdit:setText(commandBegin)

    for i,v in ipairs(possibleCommands) do
      print(v)
    end
  end
end

local function onKeyPress(widget, keyCode, keyText, keyboardModifiers)
  if keyboardModifiers == KeyboardNoModifier then
    -- execute current command
    if keyCode == KeyReturn or keyCode == keyEnter then
      local currentCommand = commandLineEdit:getText()
      Console.executeCommand(currentCommand)
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
    -- complete command
    elseif keyCode == KeyTab then
      completeCommand()
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
  Console.addLine(message, LogColors[level])
  logLocked = false
end

-- public functions
function Console.init()
  consoleWidget = displayUI('console.otui', { visible = false })
  connect(consoleWidget, { onKeyPress = onKeyPress })

  commandLineEdit = consoleWidget:getChildById('commandLineEdit')
  consoleBuffer = consoleWidget:getChildById('consoleBuffer')
  Logger.setOnLog(onLog)
  Logger.fireOldMessages()
end

function Console.terminate()
  Logger.setOnLog(nil)
  consoleWidget:destroy()
  commandLineEdit = nil
  consoleWidget = nil
end

function Console.addLine(text, color)
  -- create new line label
  local numLines = consoleBuffer:getChildCount() + 1
  local label = createWidget('ConsoleLabel', consoleBuffer)
  label:setId('consoleLabel' .. numLines)
  label:setText(text)
  label:setForegroundColor(color)

  -- delete old lines if needed
  if numLines > MaxLogLines then
    consoleBuffer:getChildByIndex(1):destroy()
  else
    consoleBuffer:setHeight(consoleBuffer:getHeight() + LabelHeight)
    consoleBuffer:updateParentLayout()
  end
end

function Console.executeCommand(command)
  -- detect and convert commands with simple syntax
  local realCommand
  if commandEnv[command] then
    if type(commandEnv[command]) == "function" then
      realCommand = command .. '()'
    else
      realCommand = 'print(' .. command .. ')'
    end
  else
    realCommand = command
  end

  -- reset current history index
  currentHistoryIndex = 0

  -- add new command to history
  table.insert(commandHistory, command)

  -- add command line
  Console.addLine(">> " .. command, "#ffffff")

  -- load command buffer
  local func, err = loadstring(realCommand, "@")

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

