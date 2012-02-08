TopMenu = {}

-- private variables
local topMenu
local leftButtonsPanel
local rightButtonsPanel
local gameButtonsPanel

-- private functions
local function onLogout()
  if g_game.isOnline() then
    g_game.safeLogout()
  else
    exit()
  end
end

-- public functions
function TopMenu.init()
  topMenu = displayUI('topmenu.otui')
  leftButtonsPanel = topMenu:getChildById('leftButtonsPanel')
  rightButtonsPanel = topMenu:getChildById('rightButtonsPanel')
  gameButtonsPanel = topMenu:getChildById('gameButtonsPanel')

  TopMenu.addRightButton('logoutButton', 'Logout (Ctrl+Q)', '/core_styles/icons/logout.png', onLogout)
  Keyboard.bindKeyDown('Ctrl+Q', onLogout)

  connect(g_game, { onGameStart = TopMenu.showGameButtons,
                  onGameEnd = TopMenu.hideGameButtons })
end

function TopMenu.terminate()
  Keyboard.unbindKeyDown('Ctrl+Q')
  leftButtonsPanel = nil
  rightButtonsPanel = nil
  gameButtonsPanel = nil
  topMenu:destroy()
  topMenu = nil

  disconnect(g_game, { onGameStart = TopMenu.showGameButtons,
                     onGameEnd = TopMenu.hideGameButtons })

  TopMenu = nil
end

function TopMenu.addButton(id, description, icon, callback, right)
  local panel
  local class
  if right then
    panel = rightButtonsPanel
    class = 'TopRightButton'
  else
    panel = leftButtonsPanel
    class = 'TopLeftButton'
  end

  local button = createWidget(class, panel)
  button:setId(id)
  button:setTooltip(description)
  button:setIcon(resolvepath(icon, 2))
  button.onClick = callback
  return button
end

function TopMenu.addGameButton(id, description, icon, callback)
  local button = createWidget('GameTopButton', gameButtonsPanel)
  button:setId(id)
  button:setTooltip(description)
  button:setIcon(resolvepath(icon, 2))
  button.onClick = callback
  return button
end

function TopMenu.addLeftButton(id, description, icon, callback)
  return TopMenu.addButton(id, description, resolvepath(icon, 2), callback, false)
end

function TopMenu.addRightButton(id, description, icon, callback)
  return TopMenu.addButton(id, description, resolvepath(icon, 2), callback, true)
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
