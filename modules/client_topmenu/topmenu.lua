TopMenu = {}

-- private variables
local topMenu
local leftButtonsPanel
local rightButtonsPanel
local gameButtonsPanel

-- private functions
local function onLogout()
  if Game.isOnline() then
    Game.logout(false)
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
  Hotkeys.bindKeyDown('Ctrl+Q', onLogout)
end

function TopMenu.terminate()
  Hotkeys.unbindKeyDown('Ctrl+Q')
  leftButtonsPanel = nil
  rightButtonsPanel = nil
  topMenu:destroy()
  topMenu = nil
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
  return TopMenu.addButton(id, description, icon, callback, false)
end

function TopMenu.addRightButton(id, description, icon, callback)
  return TopMenu.addButton(id, description, icon, callback, true)
end

function TopMenu.removeButton(param)
  local button
  if type(param) == 'string' then
    button = TopMenu.getButton(param)
  else
    button = param
  end
  button:destroy()
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

function TopMenu:getLogoutButton(id)
  return TopMenu.getButton('logoutButton')
end


connect(Game, { onLogin = TopMenu.showGameButtons,
                onLogout = TopMenu.hideGameButtons})