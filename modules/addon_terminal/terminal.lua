Terminal = { }

-- configs
local LogColors = { [LogInfo] = 'white',
                    [LogWarning] = 'yellow',
                    [LogError] = 'red' }
local MaxLogLines = 80
local LabelHeight = 16
local MaxHistory = 1000

-- private variables
local terminalWidget
local terminalButton
local logLocked = false
local commandEnv = newenv()
local commandLineEdit
local terminalBuffer
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

local function doCommand()
  local currentCommand = commandLineEdit:getText()
  Terminal.executeCommand(currentCommand)
  commandLineEdit:clearText()
  return true
end

local function onLog(level, message, time)
  -- debug messages are ignored
  if level == LogDebug then return end

  -- avoid logging while reporting logs (would cause a infinite loop)
  if logLocked then return end

  logLocked = true
  Terminal.addLine(message, LogColors[level])
  logLocked = false
end

-- public functions
function Terminal.init()
  terminalWidget = displayUI('terminal.otui')
  terminalWidget:setVisible(false)

  terminalButton = TopMenu.addButton('terminalButton', 'Terminal (Ctrl + T)', '/core_styles/icons/terminal.png', Terminal.toggle)
  Hotkeys.bindKeyDown('Ctrl+T', Terminal.toggle)

  commandHistory = Settings.getList('terminal-history')

  commandLineEdit = terminalWidget:getChildById('commandLineEdit')
  Hotkeys.bindKeyDown('Up', function() navigateCommand(1) end, commandLineEdit)
  Hotkeys.bindKeyDown('Down', function() navigateCommand(-1) end, commandLineEdit)
  Hotkeys.bindKeyDown('Tab', completeCommand, commandLineEdit)
  Hotkeys.bindKeyDown('Enter', doCommand, commandLineEdit)
  Hotkeys.bindKeyDown('Return', doCommand, commandLineEdit)

  terminalBuffer = terminalWidget:getChildById('terminalBuffer')
  Logger.setOnLog(onLog)
  Logger.fireOldMessages()
end

function Terminal.terminate()
  Settings.setList('terminal-history', commandHistory)
  Hotkeys.unbindKeyDown('Ctrl+T')
  Logger.setOnLog(nil)
  terminalButton:destroy()
  terminalButton = nil
  commandLineEdit = nil
  terminalBuffer = nil
  terminalWidget:destroy()
  terminalWidget = nil
  commandEnv = nil
end

function Terminal.toggle()
  if terminalWidget:isVisible() then
    Terminal.hide()
  else
    Terminal.show()
  end
end

function Terminal.show()
  terminalWidget:show()
  terminalWidget:lock()
end

function Terminal.hide()
  terminalWidget:unlock()
  terminalWidget:hide()
end

function Terminal.addLine(text, color)
  -- create new line label
  local numLines = terminalBuffer:getChildCount() + 1
  local label = createWidget('TerminalLabel', terminalBuffer)
  label:setId('terminalLabel' .. numLines)
  label:setText(text)
  label:setColor(color)

  -- delete old lines if needed
  if numLines > MaxLogLines then
    terminalBuffer:getChildByIndex(1):destroy()
  else
    terminalBuffer:setHeight(terminalBuffer:getHeight() + LabelHeight)
  end
end

function Terminal.executeCommand(command)
  if command == nil or #command == 0 then return end
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
  if #commandHistory > MaxHistory then
    table.remove(commandHistory, 1)
  end

  -- add command line
  Terminal.addLine(">> " .. command, "#ffffff")

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
