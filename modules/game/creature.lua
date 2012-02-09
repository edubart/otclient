SkullNone = 0
SkullYellow = 1
SkullGreen = 2
SkullWhite = 3
SkullRed = 4
SkullBlack = 5
SkullOrange = 6

ShieldNone = 0
ShieldWhiteYellow = 1
ShieldWhiteBlue = 2
ShieldBlue = 3
ShieldYellow = 4
ShieldBlueSharedExp = 5
ShieldYellowSharedExp = 6
ShieldBlueNoSharedExpBlink = 7
ShieldYellowNoSharedExpBlink = 8
ShieldBlueNoSharedExp = 9
ShieldYellowNoSharedExp = 10

EmblemNone = 0
EmblemGreen = 1
EmblemRed = 2
EmblemBlue = 3

function getSkullImagePath(skullId)
  if skullId == SkullYellow then
    return 'images/skull_yellow.png'
  elseif skullId == SkullGreen then
    return 'images/skull_green.png'
  elseif skullId == SkullWhite then
    return 'images/skull_white.png'
  elseif skullId == SkullRed then
    return 'images/skull_red.png'
  elseif skullId == SkullBlack then
    return 'images/skull_black.png'
  elseif skullId == SkullOrange then
    return 'images/skull_orange.png'
  end
end

function getShieldImagePathAndBlink(shieldId)
  if shieldId == ShieldWhiteYellow then
    return 'images/shield_yellow_white.png', false
  elseif shieldId == ShieldWhiteBlue then
    return 'images/shield_blue_white.png', false
  elseif shieldId == ShieldBlue then
    return 'images/shield_blue.png', false
  elseif shieldId == ShieldYellow then
    return 'images/shield_yellow.png', false
  elseif shieldId == ShieldBlueSharedExp then
    return 'images/shield_blue_shared.png', false
  elseif shieldId == ShieldYellowSharedExp then
    return 'images/shield_yellow_shared.png', false
  elseif shieldId == ShieldBlueNoSharedExpBlink then
    return 'images/shield_blue_not_shared.png', true
  elseif shieldId == ShieldYellowNoSharedExpBlink then
    return 'images/shield_yellow_not_shared.png', true
  elseif shieldId == ShieldBlueNoSharedExp then
    return 'images/shield_blue_not_shared.png', false
  elseif shieldId == ShieldYellowNoSharedExp then
    return 'images/shield_yellow_not_shared.png', false
  end
end

function getEmblemImagePath(emblemId)
  if emblemId == EmblemGreen then
    return 'images/emblem_green.png'
  elseif emblemId == EmblemRed then
    return 'images/emblem_red.png'
  elseif emblemId == EmblemBlue then
    return 'images/emblem_blue.png'
  end
end

function Creature:onSkullChange(skullId)
  local imagePath = getSkullImagePath(skullId)
  if imagePath then
    self:setSkullTexture(resolvepath(imagePath))
  end
end

function Creature:onShieldChange(shieldId)
  local imagePath, blink = getShieldImagePathAndBlink(shieldId)
  if imagePath then
    self:setShieldTexture(resolvepath(imagePath), blink)
  end
end

function Creature:onEmblemChange(emblemId)
  local imagePath = getEmblemImagePath(emblemId)
  if imagePath then
    self:setEmblemTexture(resolvepath(imagePath))
  end
end

