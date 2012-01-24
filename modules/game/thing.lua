
function Thing:isInsideContainer()
  local pos = self:getPos()
  return (pos and pos.x == 65535 and pos.y >= 64)
end

function Thing:getContainerId()
  local pos = self:getPos()
  if not pos then return 0 end
  return pos.y - 64
end

-- public functions
function Game.processMouseAction(menuPosition, mouseButton, autoWalk, lookThing, useThing, creatureThing, multiUseThing)
  local keyboardModifiers = g_window.getKeyboardModifiers()

  if autoWalk and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseLeftButton then
    -- todo auto walk
    return true
  end

  if not Options.classicControl then
    if keyboardModifiers == KeyboardNoModifier and mouseButton == MouseRightButton then
      Game.createThingMenu(menuPosition, lookThing, useThing, creatureThing)
      return true
    elseif lookThing and keyboardModifiers == KeyboardShiftModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      Game.look(lookThing)
      return true
    elseif useThing and keyboardModifiers == KeyboardCtrlModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      if useThing:isContainer() then
        if useThing:isInsideContainer() then
          Game.open(useThing, useThing:getContainerId())
          return true
        else
          Game.open(useThing, Containers.getFreeContainerId())
        return true
        end
      elseif useThing:isMultiUse() then
        Game.startUseWith(useThing)
        return true
      else
        Game.use(useThing)
        return true
      end
      return true
    elseif creatureThing and keyboardModifiers == KeyboardAltModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      Game.attack(creatureThing)
      return true
    end
  else
    if multiUseThing and keyboardModifiers == KeyboardNoModifier and mouseButton == MouseRightButton then
      if multiUseThing:asCreature() then
        Game.attack(multiUseThing:asCreature())
        return true
      elseif multiUseThing:isContainer() then
        if multiUseThing:isInsideContainer() then
          Game.open(multiUseThing, multiUseThing:getContainerId())
          return true
        else
          Game.open(multiUseThing, Containers.getFreeContainerId())
          return true
        end
      elseif multiUseThing:isMultiUse() then
        Game.startUseWith(multiUseThing)
        return true
      else
        Game.use(multiUseThing)
      end
      return true
    elseif lookThing and keyboardModifiers == KeyboardShiftModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      Game.look(lookThing)
      return true
    elseif useThing and keyboardModifiers == KeyboardCtrlModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      Game.createThingMenu(menuPosition, lookThing, useThing, creatureThing)
      return true
    elseif creatureThing and keyboardModifiers == KeyboardAltModifier and (mouseButton == MouseLeftButton or mouseButton == MouseRightButton) then
      Game.attack(creatureThing)
      return true
    end
  end

  return false
end


function Game.createThingMenu(menuPosition, lookThing, useThing, creatureThing)
  local menu = createWidget('PopupMenu')

  if lookThing then
    menu:addOption('Look', function() Game.look(lookThing) end)
  end

  if useThing then
    if useThing:isContainer() then
      if useThing:isInsideContainer() then
        menu:addOption('Open', function() Game.open(useThing, useThing:getContainerId()) end)
        menu:addOption('Open in new window', function() Game.open(useThing, Containers.getFreeContainerId()) end)
      else
        menu:addOption('Open', function() Game.open(useThing, Containers.getFreeContainerId()) end)
      end
    else
      if useThing:isMultiUse() then
        menu:addOption('Use with ...', function() Game.startUseWith(useThing) end)
      else
        menu:addOption('Use', function() Game.use(useThing) end)
      end
    end

    if useThing:isRotateable() then
      menu:addOption('Rotate', function() Game.rotate(useThing) end)
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
      menu:addOption('Set Outfit', function() Game.requestOutfit() end)

      if creatureThing:asPlayer():isPartyMember() --[[and not fighting]] then
        if creatureThing:asPlayer():isPartyLeader() then
          if creatureThing:asPlayer():isPartySharedExperienceActive() then
            menu:addOption('Disable Shared Experience', function() Game.partyShareExperience(false) end)
          else
            menu:addOption('Enable Shared Experience', function() Game.partyShareExperience(true) end)
          end
        end
        menu:addOption('Leave Party', function() Game.partyLeave() end)
      end

    else
      local localPlayer = Game.getLocalPlayer()
      if localPlayer then
        if localPlayer:getAttackingCreature() ~= creatureThing then
          menu:addOption('Attack', function() Game.attack(creatureThing) end)
        else
          menu:addOption('Stop Attack', function() Game.cancelAttack() end)
        end

        if localPlayer:getFollowingCreature() ~= creatureThing then
          menu:addOption('Follow', function() Game.follow(creatureThing) end)
        else
          menu:addOption('Stop Follow', function() Game.cancelFollow() end)
        end

        if creatureThing:asPlayer() then
          menu:addSeparator()
          menu:addOption('Message to ' .. creatureThing:getName(), function() print('message') end)
          menu:addOption('Add to VIP list', function() Game.addVip(creatureThing:getName()) end)
          menu:addOption('Ignore ' .. creatureThing:getName(), function() print('ignore') end)

          local localPlayerShield = localPlayer:asCreature():getShield()
          local creatureShield = creatureThing:getShield()

          if localPlayerShield == ShieldNone or localPlayerShield == ShieldWhiteBlue then
            if creatureShield == ShieldWhiteYellow then
              menu:addOption('Join ' .. creatureThing:getName() .. '\'s Party', function() Game.partyJoin(creatureThing:getId()) end)
            else
              menu:addOption('Invite to Party', function() Game.partyInvite(creatureThing:getId()) end)
            end
          elseif localPlayerShield == ShieldWhiteYellow then
            if creatureShield == ShieldWhiteBlue then
              menu:addOption('Revoke ' .. creatureThing:getName() .. '\'s Invitation', function() Game.partyRevokeInvitation(creatureThing:getId()) end)
            end
          elseif localPlayerShield == ShieldYellow or localPlayerShield == ShieldYellowSharedExp or localPlayerShield == ShieldYellowNoSharedExpBlink or localPlayerShield == ShieldYellowNoSharedExp then
            if creatureShield == ShieldWhiteBlue then
              menu:addOption('Revoke ' .. creatureThing:getName() .. '\'s Invitation', function() Game.partyRevokeInvitation(creatureThing:getId()) end)
            elseif creatureShield == ShieldBlue or creatureShield == ShieldBlueSharedExp or creatureShield == ShieldBlueNoSharedExpBlink or creatureShield == ShieldBlueNoSharedExp then
              menu:addOption('Pass Leadership to ' .. creatureThing:getName(), function() Game.partyPassLeadership(creatureThing:getId()) end)
            else
              menu:addOption('Invite to Party', function() Game.partyInvite(creatureThing:getId()) end)
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

