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
local function onSetFightMode(self, selectedFightButton)
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
  g_game.setFightMode(fightMode)
end

local function onSetChaseMode(self, checked)
  local chaseMode
  if checked then
    chaseMode = ChaseOpponent
  else
    chaseMode = DontChase
  end
  g_game.setChaseMode(chaseMode)
end

local function onSetSafeFight(self, checked)
  g_game.setSafeFight(not checked)
end

-- public functions
function CombatControls.init()
  combatControlsButton = TopMenu.addRightGameToggleButton('combatControlsButton', tr('Combat Controls'), 'combatcontrols.png', CombatControls.toggle)
  combatControlsButton:setOn(true)
  combatControlsWindow = g_ui.loadUI('combatcontrols.otui', GameInterface.getRightPanel())

  fightOffensiveBox = combatControlsWindow:recursiveGetChildById('fightOffensiveBox')
  fightBalancedBox = combatControlsWindow:recursiveGetChildById('fightBalancedBox')
  fightDefensiveBox = combatControlsWindow:recursiveGetChildById('fightDefensiveBox')
  chaseModeButton = combatControlsWindow:recursiveGetChildById('chaseModeBox')
  safeFightButton = combatControlsWindow:recursiveGetChildById('safeFightBox')

  fightModeRadioGroup = UIRadioGroup.create()
  fightModeRadioGroup:addWidget(fightOffensiveBox)
  fightModeRadioGroup:addWidget(fightBalancedBox)
  fightModeRadioGroup:addWidget(fightDefensiveBox)

  connect(fightModeRadioGroup, { onSelectionChange = onSetFightMode })
  connect(chaseModeButton, { onCheckChange = onSetChaseMode })
  connect(safeFightButton, { onCheckChange = onSetSafeFight })
  connect(g_game, {
    onGameStart = CombatControls.online,
    onGameEnd = CombatControls.offline,
    onFightModeChange = CombatControls.update,
    onChaseModeChange = CombatControls.update,
    onSafeFightChange = CombatControls.update,
    onWalk = CombatControls.check
  })

  if g_game.isOnline() then
    CombatControls.online()
  end
end

function CombatControls.terminate()
  if g_game.isOnline() then
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

  disconnect(g_game, {
    onGameStart = CombatControls.online,
    onGameEnd = CombatControls.offline,
    onFightModeChange = CombatControls.update,
    onChaseModeChange = CombatControls.update,
    onSafeFightChange = CombatControls.update,
    onWalk = CombatControls.check
  })

  CombatControls = nil
end

function CombatControls.update()
  local fightMode = g_game.getFightMode()
  if fightMode == FightOffensive then
    fightModeRadioGroup:selectWidget(fightOffensiveBox)
  elseif fightMode == FightBalanced then
    fightModeRadioGroup:selectWidget(fightBalancedBox)
  else
    fightModeRadioGroup:selectWidget(fightDefensiveBox)
  end

  local chaseMode = g_game.getChaseMode()
  chaseModeButton:setChecked(chaseMode == ChaseOpponent)

  local safeFight = g_game.isSafeFight()
  safeFightButton:setChecked(not safeFight)
end

function CombatControls.check()
  if(Options.getOption('autoChaseOverride')) then
    if(g_game.isAttacking() and g_game.getChaseMode() == ChaseOpponent) then
      g_game.setChaseMode(DontChase)
    end
  end
end

function CombatControls.online()
  local player = g_game.getLocalPlayer()
  if(player) then
    local char = player:getName()
    
    local lastCombatControls = g_settings.getNode('LastCombatControls')
    
    if(not table.empty(lastCombatControls)) then
      if(lastCombatControls[char]) then
        g_game.setFightMode(lastCombatControls[char].fightMode)
        g_game.setChaseMode(lastCombatControls[char].chaseMode)
        g_game.setSafeFight(lastCombatControls[char].safeFight)
      end
    end
  end
  
  combatControlsWindow:setVisible(combatControlsButton:isOn())
  CombatControls.update()
end

function CombatControls.offline()
  local lastCombatControls = g_settings.getNode('LastCombatControls')
  if(not lastCombatControls) then
    lastCombatControls = {}
  end
  
  local player = g_game.getLocalPlayer()
  if(player) then
    local char = player:getName()    
    lastCombatControls[char] = {
      fightMode = g_game.getFightMode(),
      chaseMode = g_game.getChaseMode(),
      safeFight = g_game.isSafeFight()
    }
    
    -- save last combat control settings
    g_settings.setNode('LastCombatControls', lastCombatControls)
  end
end

function CombatControls.toggle()
  if combatControlsButton:isOn() then
    combatControlsWindow:close()
    combatControlsButton:setOn(false)
  else
    combatControlsWindow:open()
    combatControlsButton:setOn(true)
  end
end

function CombatControls.onMiniWindowClose()
  combatControlsButton:setOn(false)
end
