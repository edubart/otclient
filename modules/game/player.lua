-- @docclass Player

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
