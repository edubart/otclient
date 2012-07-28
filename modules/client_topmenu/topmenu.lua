TopMenu = {}

-- private variables
local topMenu
local leftButtonsPanel
local rightButtonsPanel
local leftGameButtonsPanel
local rightGameButtonsPanel

-- private functions
local function addButton(id, description, icon, callback, panel, toggle)
  local class
  if toggle then
    class = 'TopToggleButton'
  else
    class = 'TopButton'
  end

  local button = g_ui.createWidget(class, panel)
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

  topMenu = g_ui.displayUI('topmenu.otui')

  leftButtonsPanel = topMenu:getChildById('leftButtonsPanel')
  rightButtonsPanel = topMenu:getChildById('rightButtonsPanel')
  leftGameButtonsPanel = topMenu:getChildById('leftGameButtonsPanel')
  rightGameButtonsPanel = topMenu:getChildById('rightGameButtonsPanel')

  if g_game.isOnline() then
    leftGameButtonsPanel:show()
    rightGameButtonsPanel:show()
  end
end

function TopMenu.terminate()
  disconnect(g_game, { onGameStart = TopMenu.showGameButtons,
                       onGameEnd = TopMenu.hideGameButtons })

  leftButtonsPanel = nil
  rightButtonsPanel = nil
  leftGameButtonsPanel = nil
  rightGameButtonsPanel = nil

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

function TopMenu.addLeftGameButton(id, description, icon, callback)
  return addButton(id, description, icon, callback, leftGameButtonsPanel, false)
end

function TopMenu.addLeftGameToggleButton(id, description, icon, callback, right)
  return addButton(id, description, icon, callback, leftGameButtonsPanel, true)
end

function TopMenu.addRightGameButton(id, description, icon, callback)
  return addButton(id, description, icon, callback, rightGameButtonsPanel, false)
end

function TopMenu.addRightGameToggleButton(id, description, icon, callback, right)
  return addButton(id, description, icon, callback, rightGameButtonsPanel, true)
end

function TopMenu.hideGameButtons()
  leftGameButtonsPanel:hide()
  rightGameButtonsPanel:hide()
end

function TopMenu.showGameButtons()
  leftGameButtonsPanel:show()
  rightGameButtonsPanel:show()
end

function TopMenu.getButton(id)
  return topMenu:recursiveGetChildById(id)
end


