-- @docclass Player

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
