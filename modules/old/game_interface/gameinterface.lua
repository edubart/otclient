GameInterface = {}

-- private variables
local WALK_AUTO_REPEAT_DELAY = 90
local gameRootPanel
local gameMapPanel
local gameRightPanel
local gameLeftPanel
local gameBottomPanel

-- private functions
function onGameStart()
  -- hook window close event
  setonclose(GameInterface.tryLogout)
  GameInterface.show()
end

function onGameEnd()
  setonclose(exit)
  GameInterface.hide()
end

-- public functions
function GameInterface.init()
  gameRootPanel = displayUI('gameinterface.otui')
  gameRootPanel:lower()
  connect(g_game, { onGameStart = onGameStart }, true)
  connect(g_game, { onGameEnd = onGameEnd })

  Keyboard.bindKeyPress('Up', function() g_game.walk(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Right', function() g_game.walk(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Down', function() g_game.walk(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Left', function() g_game.walk(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad8', function() g_game.walk(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad9', function() g_game.walk(NorthEast) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad6', function() g_game.walk(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad3', function() g_game.walk(SouthEast) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad2', function() g_game.walk(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad1', function() g_game.walk(SouthWest) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad4', function() g_game.walk(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Numpad7', function() g_game.walk(NorthWest) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Up', function() g_game.turn(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Right', function() g_game.turn(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Down', function() g_game.turn(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Left', function() g_game.turn(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Numpad8', function() g_game.turn(North) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Numpad6', function() g_game.turn(East) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Numpad2', function() g_game.turn(South) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Ctrl+Numpad4', function() g_game.turn(West) end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
  Keyboard.bindKeyPress('Esc', function() g_game.cancelAttackAndFollow() end, gameRootPanel, WALK_AUTO_REPEAT_DELAY)
end

function GameInterface.terminate()
  disconnect(g_game, { onGameStart = onGameStart }, true)
  disconnect(g_game, { onGameEnd = onGameEnd })
end

function GameInterface.show()
  gameRootPanel:show()
end

function GameInterface.hide()
  gameRootPanel:hide()
end

function GameInterface.tryLogout()
  if g_game.isOnline() then
    g_game.forceLogout()
  else
    exit()
  end
end

function GameInterface.getRootPanel()
  return gameRootPanel
end

function GameInterface.getMapPanel()
  return gameMapPanel
end

function GameInterface.getRightPanel()
  return gameRightPanel
end

function GameInterface.getLeftPanel()
  return gameLeftPanel
end

function GameInterface.getBottomPanel()
  return gameBottomPanel
end

function GameInterface.createThingMenu(menuPosition, lookThing, useThing, creatureThing)
  local menu = createWidget('PopupMenu')

  if lookThing then
    menu:addOption('Look', function() g_game.look(lookThing) end)
  end

  if useThing then
    if useThing:isContainer() then
      if useThing:isInsideContainer() then
        menu:addOption('Open', function() g_game.open(useThing, useThing:getContainerId()) end)
        menu:addOption('Open in new window', function() g_game.open(useThing, Containers.getFreeContainerId()) end)
      else
        menu:addOption('Open', function() g_game.open(useThing, Containers.getFreeContainerId()) end)
      end
    else
      if useThing:isMultiUse() then
        menu:addOption('Use with ...', function() g_game.startUseWith(useThing) end)
      else
        menu:addOption('Use', function() g_game.use(useThing) end)
      end
    end

    if useThing:isRotateable() then
      menu:addOption('Rotate', function() g_game.rotate(useThing) end)
    end

  end

  if lookThing and not lookThing:asCreature() and not lookThing:isNotMoveable() and lookThing:isPickupable() then
    menu:addSeparator()
    menu:addOption('Trade with ...', function() print('trade with') end)
  end

  -- check for move up

  if creatureThing then
    menu:addSeparator()

    if creatureThing:asLocalPlayer() then
      menu:addOption('Set Outfit', function() g_game.requestOutfit() end)

      if creatureThing:asPlayer():isPartyMember() --[[and not fighting]] then
        if creatureThing:asPlayer():isPartyLeader() then
          if creatureThing:asPlayer():isPartySharedExperienceActive() then
            menu:addOption('Disable Shared Experience', function() g_game.partyShareExperience(false) end)
          else
            menu:addOption('Enable Shared Experience', function() g_game.partyShareExperience(true) end)
          end
        end
        menu:addOption('Leave Party', function() g_game.partyLeave() end)
      end

    else
      local localPlayer = g_game.getLocalPlayer()
      if localPlayer then
        if g_game.getAttackingCreature() ~= creatureThing then
          menu:addOption('Attack', function() g_game.attack(creatureThing) end)
        else
          menu:addOption('Stop Attack', function() g_game.cancelAttack() end)
        end

        if g_game.getFollowingCreature() ~= creatureThing then
          menu:addOption('Follow', function() g_game.follow(creatureThing) end)
        else
          menu:addOption('Stop Follow', function() g_game.cancelFollow() end)
        end

        if creatureThing:asPlayer() then
          menu:addSeparator()
          menu:addOption('Message to ' .. creatureThing:getName(), function() print('message') end)
          menu:addOption('Add to VIP list', function() g_game.addVip(creatureThing:getName()) end)

          local localPlayerShield = localPlayer:asCreature():getShield()
          local creatureShield = creatureThing:getShield()

          if localPlayerShield == ShieldNone or localPlayerShield == ShieldWhiteBlue then
            if creatureShield == ShieldWhiteYellow then
              menu:addOption('Join ' .. creatureThing:getName() .. '\'s Party', function() g_game.partyJoin(creatureThing:getId()) end)
            else
              menu:addOption('Invite to Party', function() g_game.partyInvite(creatureThing:getId()) end)
            end
          elseif localPlayerShield == ShieldWhiteYellow then
            if creatureShield == ShieldWhiteBlue then
              menu:addOption('Revoke ' .. creatureThing:getName() .. '\'s Invitation', function() g_game.partyRevokeInvitation(creatureThing:getId()) end)
            end
          elseif localPlayerShield == ShieldYellow or localPlayerShield == ShieldYellowSharedExp or localPlayerShield == ShieldYellowNoSharedExpBlink or localPlayerShield == ShieldYellowNoSharedExp then
            if creatureShield == ShieldWhiteBlue then
              menu:addOption('Revoke ' .. creatureThing:getName() .. '\'s Invitation', function() g_game.partyRevokeInvitation(creatureThing:getId()) end)
            elseif creatureShield == ShieldBlue or creatureShield == ShieldBlueSharedExp or creatureShield == ShieldBlueNoSharedExpBlink or creatureShield == ShieldBlueNoSharedExp then
              menu:addOption('Pass Leadership to ' .. creatureThing:getName(), function() g_game.partyPassLeadership(creatureThing:getId()) end)
            else
              menu:addOption('Invite to Party', function() g_game.partyInvite(creatureThing:getId()) end)
            end
          end
        end
      end
    end

    menu:addSeparator()
    menu:addOption('Copy Name', function() g_window.setClipboardText(creatureThing:getName()) end)

  end

  menu:display(menuPosition)
end
