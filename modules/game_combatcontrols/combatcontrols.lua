CombatControls = {}

local combatControlsButton

function CombatControls.init()
  combatControlsButton = TopMenu.addGameButton('combatControlsButton', 'Combat Controls', 'combatcontrols.png', CombatControls.toggle)
end


function CombatControls.terminate()
  combatControlsButton:destroy()
  combatControlsButton = nil
end
