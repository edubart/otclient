TopMenu = {}

-- private variables
local topMenu
local leftButtonsPanel
local rightButtonsPanel
local gameButtonsPanel

-- private functions
local function addButton(id, description, icon, callback, panel, toggle)
  local class
  if toggle then
    class = 'TopToggleButton'
  else
    class = 'TopButton'
  end

  local button = createWidget(class, panel)
  button:setId(id)
  button:setTooltip(description)
  button:setIcon(resolvepath(icon, 3))
  button.onClick = callback
  return button
end

-- public functions
function TopMenu.init()
  connect(g_game, { onGameStart = TopMenu.showGameButtons,
                    onGameEnd = TopMenu.hideGameButtons })

  topMenu = displayUI('topmenu.otui')

  leftButtonsPanel = topMenu:getChildById('leftButtonsPanel')
  rightButtonsPanel = topMenu:getChildById('rightButtonsPanel')
  gameButtonsPanel = topMenu:getChildById('gameButtonsPanel')

  if g_game.isOnline() then
    gameButtonsPanel:show()
  end
end

function TopMenu.terminate()
  disconnect(g_game, { onGameStart = TopMenu.showGameButtons,
                       onGameEnd = TopMenu.hideGameButtons })

  leftButtonsPanel = nil
  rightButtonsPanel = nil
  gameButtonsPanel = nil

  topMenu:destroy()
  topMenu = nil

  TopMenu = nil
end

function TopMenu.addLeftButton(id, description, icon, callback)
  return addButton(id, description, icon, callback, leftButtonsPanel, false)
end

function TopMenu.addLeftToggleButton(id, description, icon, callback, right)
  return addButton(id, description, icon, callback, leftButtonsPanel, true)
end

function TopMenu.addRightButton(id, description, icon, callback)
  return addButton(id, description, icon, callback, rightButtonsPanel, false)
end

function TopMenu.addRightToggleButton(id, description, icon, callback, right)
  return addButton(id, description, icon, callback, rightButtonsPanel, true)
end

function TopMenu.addGameButton(id, description, icon, callback)
  return addButton(id, description, icon, callback, gameButtonsPanel, false)
end

function TopMenu.addGameToggleButton(id, description, icon, callback, right)
  return addButton(id, description, icon, callback, gameButtonsPanel, true)
end

function TopMenu.hideGameButtons()
  gameButtonsPanel:hide()
end

function TopMenu.showGameButtons()
  gameButtonsPanel:show()
end

function TopMenu.getButton(id)
  return topMenu:recursiveGetChildById(id)
end


