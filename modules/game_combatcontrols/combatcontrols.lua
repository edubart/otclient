CombatControls = {}

-- private variables
local combatControlsButton
local combatControlsWindow
local fightOffensiveBox
local fightBalancedBox
local fightDefensiveBox
local chaseModeButton
local safeFightButton
local fightModeRadioGroup

-- private functions
local function onFightModeChange(self, selectedFightButton)
  if selectedFightButton == nil then return end
  local buttonId = selectedFightButton:getId()
  local fightMode
  if buttonId == 'fightOffensiveBox' then
    fightMode = FightOffensive
  elseif buttonId == 'fightBalancedBox' then
    fightMode = FightBalanced
  else
    fightMode = FightDefensive
  end
  if Game.getFightMode ~= fightMode then
    Game.setFightMode(fightMode)
  end
end

local function onChaseModeChange(self, checked)
  local chaseMode
  if checked then
    chaseMode = ChaseOpponent
  else
    chaseMode = DontChase
  end
  if Game.getChaseMode() ~= chaseMode then
    Game.setChaseMode(chaseMode)
  end
end

local function onSafeFightChange(self, checked)
  local safeFight = not checked
  if Game.isSafeFight() ~= safeFight then
    Game.setSafeFight(not checked)
  end
end

-- public functions
function CombatControls.init()
  combatControlsButton = TopMenu.addGameButton('combatControlsButton', 'Combat Controls', 'combatcontrols.png', CombatControls.toggle)
  combatControlsButton:setOn(true)
  combatControlsWindow = loadUI('combatcontrols.otui')

  fightOffensiveBox = combatControlsWindow:getChildById('fightOffensiveBox')
  fightBalancedBox = combatControlsWindow:getChildById('fightBalancedBox')
  fightDefensiveBox = combatControlsWindow:getChildById('fightDefensiveBox')
  chaseModeButton = combatControlsWindow:getChildById('chaseModeBox')
  safeFightButton = combatControlsWindow:getChildById('safeFightBox')

  fightModeRadioGroup = RadioGroup.create()
  fightModeRadioGroup:addWidget(fightOffensiveBox)
  fightModeRadioGroup:addWidget(fightBalancedBox)
  fightModeRadioGroup:addWidget(fightDefensiveBox)

  connect(fightModeRadioGroup, { onSelectionChange = onFightModeChange })
  connect(chaseModeButton, { onCheckChange = onChaseModeChange })
  connect(safeFightButton, { onCheckChange = onSafeFightChange })
  connect(Game, { onGameStart = CombatControls.online })
  connect(Game, { onGameEnd = CombatControls.offline })

  if Game.isOnline() then
    CombatControls.online()
  end
end

function CombatControls.terminate()
  if Game.isOnline() then
    CombatControls.offline()
  end

  fightModeRadioGroup:destroy()
  fightModeRadioGroup = nil

  fightOffensiveBox = nil
  fightBalancedBox = nil
  fightDefensiveBox = nil
  chaseModeButton = nil
  safeFightButton = nil

  combatControlsButton:destroy()
  combatControlsButton = nil

  combatControlsWindow:destroy()
  combatControlsWindow = nil

  disconnect(Game, { onGameStart = CombatControls.online })
  disconnect(Game, { onGameEnd = CombatControls.offline })

  CombatControls = nil
end

function CombatControls.online()
  Game.gameRightPanel:addChild(combatControlsWindow)
  combatControlsWindow:setVisible(combatControlsButton:isOn())

  local fightMode = Game.getFightMode()
  if fightMode == FightOffensive then
    fightModeRadioGroup:selectWidget(fightOffensiveBox)
  elseif fightMode == FightBalanced then
    fightModeRadioGroup:selectWidget(fightBalancedBox)
  else
    fightModeRadioGroup:selectWidget(fightDefensiveBox)
  end

  local chaseMode = Game.getChaseMode()
  chaseModeButton:setChecked(chaseMode == ChaseOpponent)

  local safeFight = Game.isSafeFight()
  safeFightButton:setChecked(not safeFight)
end

function CombatControls.offline()
  Game.gameRightPanel:removeChild(combatControlsWindow)
end

function CombatControls.toggle()
  local visible = not combatControlsWindow:isExplicitlyVisible()
  combatControlsWindow:setVisible(visible)
  combatControlsButton:setOn(visible)
end

