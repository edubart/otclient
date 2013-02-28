-- configs
local LogColors = { [LogDebug] = 'pink',
                    [LogInfo] = 'white',
                    [LogWarning] = 'yellow',
                    [LogError] = 'red' }
local MaxLogLines = 512
local LabelHeight = 16
local MaxHistory = 1000

local oldenv = getfenv(0)
setfenv(0, _G)
commandEnv = runinsandbox('commands')
setfenv(0, oldenv)

-- private variables
local terminalWindow
local terminalButton
local logLocked = false
local commandTextEdit
local terminalBuffer
local commandHistory = { }
local currentHistoryIndex = 0
local poped = false
local oldPos
local oldSize
local firstShown = false

-- private functions
local function navigateCommand(step)
  local numCommands = #commandHistory
  if numCommands > 0 then
    currentHistoryIndex = math.min(math.max(currentHistoryIndex + step, 0), numCommands)
    if currentHistoryIndex > 0 then
      local command = commandHistory[numCommands - currentHistoryIndex + 1]
      commandTextEdit:setText(command)
      commandTextEdit:setCursorPos(-1)
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
    commandTextEdit:setCursorPos(-1)
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
      commandTextEdit:setCursorPos(-1)

    for i,v in ipairs(possibleCommands) do
      print(v)
    end
  end
end

local function doCommand()
  local currentCommand = commandTextEdit:getText()
  executeCommand(currentCommand)

  if commandTextEdit then
    commandTextEdit:clearText()
  end
  return true
end

local function onLog(level, message, time)
  -- avoid logging while reporting logs (would cause a infinite loop)
  if logLocked then return end

  logLocked = true
  addLine(message, LogColors[level])
  logLocked = false
end

-- public functions
function init()
  terminalWindow = g_ui.displayUI('terminal')
  terminalWindow:setVisible(false)

  terminalWindow.onDoubleClick = popWindow

  terminalButton = modules.client_topmenu.addLeftButton('terminalButton', tr('Terminal') .. ' (Ctrl + T)', '/images/topbuttons/terminal', toggle)
  g_keyboard.bindKeyDown('Ctrl+T', toggle)

  commandHistory = g_settings.getList('terminal-history')

  commandTextEdit = terminalWindow:getChildById('commandTextEdit')
  g_keyboard.bindKeyPress('Up', function() navigateCommand(1) end, commandTextEdit)
  g_keyboard.bindKeyPress('Down', function() navigateCommand(-1) end, commandTextEdit)
  g_keyboard.bindKeyPress('Ctrl+C', 
    function() 
      if commandTextEdit:hasSelection() or not terminalSelectText:hasSelection() then return false end
      g_window.setClipboardText(terminalSelectText:getSelection())
    return true 
    end, commandTextEdit)
  g_keyboard.bindKeyDown('Tab', completeCommand, commandTextEdit)
  g_keyboard.bindKeyDown('Enter', doCommand, commandTextEdit)
  g_keyboard.bindKeyDown('Escape', hide, terminalWindow)

  terminalBuffer = terminalWindow:recursiveGetChildById('terminalBuffer')
  terminalSelectText = terminalWindow:recursiveGetChildById('terminalSelectText')
  terminalSelectText.onDoubleClick = popWindow

  g_logger.setOnLog(onLog)
  g_logger.fireOldMessages()
end

function terminate()
  g_settings.setList('terminal-history', commandHistory)

  if poped then
    oldPos = terminalWindow:getPosition()
    oldSize = terminalWindow:getSize()
  end
  local settings = {
    size = oldSize,
    pos = oldPos,
    poped = poped
  }
  g_settings.setNode('terminal-window', settings)

  g_keyboard.unbindKeyDown('Ctrl+T')
  g_logger.setOnLog(nil)
  terminalWindow:destroy()
  terminalButton:destroy()
  commandEnv = nil
end

function hideButton()
  terminalButton:hide()
end

function popWindow()
  if poped then
    oldPos = terminalWindow:getPosition()
    oldSize = terminalWindow:getSize()
    terminalWindow:fill('parent')
    terminalWindow:setOn(false)
    terminalWindow:getChildById('bottomResizeBorder'):disable()
    terminalWindow:getChildById('rightResizeBorder'):disable()
    terminalWindow:getChildById('titleBar'):hide()
    terminalWindow:getChildById('terminalScroll'):setMarginTop(0)
    terminalWindow:getChildById('terminalScroll'):setMarginBottom(0)
    terminalWindow:getChildById('terminalScroll'):setMarginRight(0)
    poped = false
  else
    terminalWindow:breakAnchors()
    terminalWindow:setOn(true)
    local size = oldSize or { width = g_window.getWidth()/2, height = g_window.getHeight()/2 }
    terminalWindow:setSize(size)
    local pos = oldPos or { x = (g_window.getWidth() - terminalWindow:getWidth())/2, y = (g_window.getHeight() - terminalWindow:getHeight())/2 }
    terminalWindow:setPosition(pos)
    terminalWindow:getChildById('bottomResizeBorder'):enable()
    terminalWindow:getChildById('rightResizeBorder'):enable()
    terminalWindow:getChildById('titleBar'):show()
    terminalWindow:getChildById('terminalScroll'):setMarginTop(18)
    terminalWindow:getChildById('terminalScroll'):setMarginBottom(1)
    terminalWindow:getChildById('terminalScroll'):setMarginRight(1)
    terminalWindow:bindRectToParent()
    poped = true
  end
end

function toggle()
  if terminalWindow:isVisible() then
    hide()
  else
    if not firstShow then
      local settings = g_settings.getNode('terminal-window')
      if settings then
        if settings.size then  oldSize = size end
        if settings.pos then oldPos = settings.pos end
        if settings.poped then popWindow() end
      end
      firstShown = true
    end
    show()
  end
end

function show()
  terminalWindow:show()
  terminalWindow:raise()
  terminalWindow:focus()
end

function hide()
  terminalWindow:hide()
end

function disable()
  terminalButton:hide()
  g_keyboard.unbindKeyDown('Ctrl+T')
end

function addLine(text, color)
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

  terminalSelectText:setText(terminalSelectText:getText() .. '\n' .. text)
end

function executeCommand(command)
  if command == nil or #command == 0 then return end

  -- add command line
  addLine("> " .. command, "#ffffff")

  -- reset current history index
  currentHistoryIndex = 0

  -- add new command to history
  if #commandHistory == 0 or commandHistory[#commandHistory] ~= command then
    table.insert(commandHistory, command)
    if #commandHistory > MaxHistory then
      table.remove(commandHistory, 1)
    end
  end

  -- detect and convert commands with simple syntax
  local realCommand
  if string.sub(command, 1, 1) == '=' then
    realCommand = 'print(' .. string.sub(command,2) .. ')'
  else
    realCommand = command
  end

  local func, err = loadstring(realCommand, "@")

  -- detect terminal commands
  if not func then
    local command_name = command:match('^([%w_]+)[%s]*.*')
    if command_name then
      local args = string.split(command:match('^[%w]+[%s]*(.*)'), ' ')
      if commandEnv[command_name] and type(commandEnv[command_name]) == 'function' then
        func = function() modules.client_terminal.commandEnv[command_name](unpack(args)) end
      elseif command_name == command then
        addLine('ERROR: command not found', 'red')
        return
      end
    end
  end

  -- check for syntax errors
  if not func then
    addLine('ERROR: incorrect lua syntax: ' .. err:sub(5), 'red')
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
    addLine('ERROR: command failed: ' .. ret, 'red')
  end
end

function clear()
  terminalBuffer:destroyChildren()
  terminalSelectText:setText('')
end
