
function Thing:isInsideContainer()
  local pos = self:getPosition()
  return (pos and pos.x == 65535 and pos.y >= 64)
end

function Thing:getContainerId()
  local pos = self:getPosition()
  if not pos then return 0 end
  return pos.y - 64
end

-- public functions
function g_game.processMouseAction(menuPosition, mouseButton, autoWalk, lookThing, useThing, creatureThing, multiUseThing)
  local keyboardModifiers = g_window.getKeyboardModifiers()

  if autoWalk and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseLeftButton then
    -- todo auto walk
    return true
  end

  if not Options.getOption('classicControl') then
    if keyboardModifiers == KeyboardNoModifier and mouseButton == MouseRightButton then
      g_game.createThingMenu(menuPosition, lookThing, useThing, creatureThing)
      return true
    elseif lookThing and keyboardModifiers == KeyboardShiftModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.look(lookThing)
      return true
    elseif useThing and keyboardModifiers == KeyboardCtrlModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      if useThing:isContainer() then
        if useThing:isInsideContainer() then
          g_game.open(useThing, useThing:getContainerId())
          return true
        else
          g_game.open(useThing, Containers.getFreeContainerId())
        return true
        end
      elseif useThing:isMultiUse() then
        g_game.startUseWith(useThing)
        return true
      else
        g_game.use(useThing)
        return true
      end
      return true
    elseif creatureThing and keyboardModifiers == KeyboardAltModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.attack(creatureThing)
      return true
    end
  else
    if multiUseThing and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseRightButton then
      if multiUseThing:asCreature() then
        g_game.attack(multiUseThing:asCreature())
        return true
      elseif multiUseThing:isContainer() then
        if multiUseThing:isInsideContainer() then
          g_game.open(multiUseThing, multiUseThing:getContainerId())
          return true
        else
          g_game.open(multiUseThing, Containers.getFreeContainerId())
          return true
        end
      elseif multiUseThing:isMultiUse() then
        g_game.startUseWith(multiUseThing)
        return true
      else
        g_game.use(multiUseThing)
      end
      return true
    elseif lookThing and keyboardModifiers == KeyboardShiftModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.look(lookThing)
      return true
    elseif useThing and keyboardModifiers == KeyboardCtrlModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.createThingMenu(menuPosition, lookThing, useThing, creatureThing)
      return true
    elseif creatureThing and keyboardModifiers == KeyboardAltModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      g_game.attack(creatureThing)
      return true
    end
  end

  return false
end


function g_game.createThingMenu(menuPosition, lookThing, useThing, creatureThing)
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
          menu:addOption('Ignore ' .. creatureThing:getName(), function() print('ignore') end)

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

