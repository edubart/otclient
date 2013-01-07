-- @docclass Player

PlayerStates = {
  None = 0,
  Poison = 1,
  Burn = 2,
  Energy = 4,
  Drunk = 8,
  ManaShield = 16,
  Paralyze = 32,
  Haste = 64,
  Swords = 128,
  Drowning = 256,
  Freezing = 512,
  Dazzled = 1024,
  Cursed = 2048,
  PartyBuff = 4096,
  PzBlock = 8192,
  Pz = 16384,
  Bleeding = 32768,
  Hungry = 65536
}

InventorySlotOther = 0
InventorySlotHead = 1
InventorySlotNeck = 2
InventorySlotBack = 3
InventorySlotBody = 4
InventorySlotRight = 5
InventorySlotLeft = 6
InventorySlotLeg = 7
InventorySlotFeet = 8
InventorySlotFinger = 9
InventorySlotAmmo = 10
InventorySlotPurse = 11

InventorySlotFirst = 1
InventorySlotLast = 10

function Player:isPartyLeader()
  local shield = self:getShield()
  return (shield == ShieldWhiteYellow or
          shield == ShieldYellow or
          shield == ShieldYellowSharedExp or
          shield == ShieldYellowNoSharedExpBlink or
          shield == ShieldYellowNoSharedExp)
end

function Player:isPartyMember()
  local shield = self:getShield()
  return (shield == ShieldWhiteYellow or
          shield == ShieldYellow or
          shield == ShieldYellowSharedExp or
          shield == ShieldYellowNoSharedExpBlink or
          shield == ShieldYellowNoSharedExp or
          shield == ShieldBlueSharedExp or
          shield == ShieldBlueNoSharedExpBlink or
          shield == ShieldBlueNoSharedExp or
          shield == ShieldBlue)
end

function Player:isPartySharedExperienceActive()
  local shield = self:getShield()
  return (shield == ShieldYellowSharedExp or
          shield == ShieldYellowNoSharedExpBlink or
          shield == ShieldYellowNoSharedExp or
          shield == ShieldBlueSharedExp or
          shield == ShieldBlueNoSharedExpBlink or
          shield == ShieldBlueNoSharedExp)
end

function Player:hasVip(creatureName)
  for id, vip in pairs(g_game.getVips()) do
    if (vip[1] == creatureName) then return true end
  end
  return false
end

function Player:isMounted()
  local outfit = self:getOutfit()
  return outfit.mount ~= nil and outfit.mount > 0
end

function Player:toggleMount()
  if g_game.getFeature(GamePlayerMounts) then
    g_game.mount(not self:isMounted())
  end
end

function Player:mount()
  if g_game.getFeature(GamePlayerMounts) then
    g_game.mount(true)
  end
end

function Player:dismount()
  if g_game.getFeature(GamePlayerMounts) then
    g_game.mount(false)
  end
end

function Player:getLastDestination()
  return self.lastDestination
end

function Player:setLastDestination(destination)
  self.lastDestination = destination
end

function Player:getWalkSteps(destination)
  if not self.walkSteps or not destination then
    return nil
  end
  return self.walkSteps[destination]
end

function Player:setWalkStep(destination)
  if not self.walkSteps then
    self.walkSteps = {}
  end
  if destination then
    if not self.walkSteps[destination] then
      self.walkSteps[destination] = {}
    end
    table.insert(self.walkSteps[destination], true)
  end
end

function Player:clearWalkSteps()
  self.walkSteps = {}
end

function Player:autoWalk(destination)
  self:clearWalkSteps()
  self:setLastDestination(destination)
  local dirs = g_map.findPath(self:getPosition(), destination, 127, PathFindFlags.AllowNullTiles)
  
  if #dirs == 0 then
    modules.game_textmessage.displayStatusMessage(tr('There is no way.'))
    return false
  end
  g_game.autoWalk(dirs)
  return true
end