Terminal = { }

-- configs
local LogColors = { [LogInfo] = 'white',
                    [LogWarning] = 'yellow',
                    [LogError] = 'red' }
local MaxLogLines = 80
local LabelHeight = 16
local MaxHistory = 1000

-- private variables
local terminalWindow
local terminalButton
local logLocked = false
local commandEnv = {}
setmetatable(commandEnv, { __index = getfenv() } )
local commandTextEdit
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
      commandTextEdit:setText(command)
    else
      commandTextEdit:clearText()
    end
  end
end

local function completeCommand()
  local cursorPos = commandTextEdit:getCursorPos()
  if cursorPos == 0 then return end

  local commandBegin = commandTextEdit:getText():sub(1, cursorPos)
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
    commandTextEdit:setText(possibleCommands[1])
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
    commandTextEdit:setText(commandBegin)

    for i,v in ipairs(possibleCommands) do
      print(v)
    end
  end
end

local function doCommand()
  local currentCommand = commandTextEdit:getText()
  Terminal.executeCommand(currentCommand)

  if commandTextEdit then
    commandTextEdit:clearText()
  end
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
  terminalWindow = g_ui.displayUI('terminal.otui')
  terminalWindow:setVisible(false)

  local poped = false
  terminalWindow.onDoubleClick = function(self)
    if poped then
      self:fill('parent')
      self:getChildById('bottomResizeBorder'):disable()
      self:getChildById('rightResizeBorder'):disable()
      poped = false
    else
      self:breakAnchors()
      self:resize(g_window.getWidth()/2, g_window.getHeight()/2)
      self:move(g_window.getWidth()/2, g_window.getHeight()/2)
      self:getChildById('bottomResizeBorder'):enable()
      self:getChildById('rightResizeBorder'):enable()
      poped = true
    end
  end

  terminalButton = TopMenu.addLeftButton('terminalButton', tr('Terminal') .. ' (Ctrl + T)', 'terminal.png', Terminal.toggle)
  g_keyboard.bindKeyDown('Ctrl+T', Terminal.toggle)

  commandHistory = g_settings.getList('terminal-history')

  commandTextEdit = terminalWindow:getChildById('commandTextEdit')
  g_keyboard.bindKeyPress('Up', function() navigateCommand(1) end, commandTextEdit)
  g_keyboard.bindKeyPress('Down', function() navigateCommand(-1) end, commandTextEdit)
  g_keyboard.bindKeyDown('Tab', completeCommand, commandTextEdit)
  g_keyboard.bindKeyDown('Enter', doCommand, commandTextEdit)
  g_keyboard.bindKeyDown('Escape', Terminal.hide, terminalWindow)

  terminalBuffer = terminalWindow:getChildById('terminalBuffer')
  g_logger.setOnLog(onLog)
  g_logger.fireOldMessages()
end

function Terminal.terminate()
  g_settings.setList('terminal-history', commandHistory)
  g_keyboard.unbindKeyDown('Ctrl+T')
  g_logger.setOnLog(nil)
  terminalButton:destroy()
  terminalButton = nil
  commandTextEdit = nil
  terminalBuffer = nil
  terminalWindow:destroy()
  terminalWindow = nil
  commandEnv = nil
  Terminal = nil
end

function Terminal.toggle()
  if terminalWindow:isVisible() then
    Terminal.hide()
  else
    Terminal.show()
  end
end

function Terminal.show()
  terminalWindow:show()
  terminalWindow:raise()
  terminalWindow:focus()
end

function Terminal.hide()
  terminalWindow:hide()
end

function Terminal.addLine(text, color)
  -- delete old lines if needed
  local numLines = terminalBuffer:getChildCount() + 1
  if numLines > MaxLogLines then
    terminalBuffer:getChildByIndex(1):destroy()
  end

  -- create new line label
  local label = g_ui.createWidget('TerminalLabel', terminalBuffer)
  label:setId('terminalLabel' .. numLines)
  label:setText(text)
  label:setColor(color)
end

function Terminal.executeCommand(command)
  if command == nil or #command == 0 then return end

  logLocked = true
  g_logger.log(LogInfo, '> ' .. command)
  logLocked = false

  -- detect and convert commands with simple syntax
  local realCommand
  if string.sub(command, 1, 1) == '=' then
    realCommand = 'print(' .. string.sub(command,2) .. ')'
  else
    realCommand = command
  end

  -- reset current history index
  currentHistoryIndex = 0

  -- add new command to history
  if #commandHistory == 0 or commandHistory[#commandHistory] ~= command then
    table.insert(commandHistory, command)
    if #commandHistory > MaxHistory then
      table.remove(commandHistory, 1)
    end
  end

  -- add command line
  --Terminal.addLine(">> " .. command, "#ffffff")

  -- load command buffer
  local func, err = loadstring(realCommand, "@")

  -- check for syntax errors
  if not func then
    g_logger.log(LogError, 'incorrect lua syntax: ' .. err:sub(5))
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
    g_logger.log(LogError, 'command failed: ' .. ret)
  end
end
