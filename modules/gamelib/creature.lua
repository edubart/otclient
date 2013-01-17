-- @docclass Creature

-- @docconsts @{

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

-- @}

function getSkullImagePath(skullId)
  local path
  if skullId == SkullYellow then
    path = 'icons/skull_yellow.png'
  elseif skullId == SkullGreen then
    path = 'icons/skull_green.png'
  elseif skullId == SkullWhite then
    path = 'icons/skull_white.png'
  elseif skullId == SkullRed then
    path = 'icons/skull_red.png'
  elseif skullId == SkullBlack then
    path = 'icons/skull_black.png'
  elseif skullId == SkullOrange then
    path = 'icons/skull_orange.png'
  end
  path = resolvepath(path)
  return path
end

function getShieldImagePathAndBlink(shieldId)
  local path, blink
  if shieldId == ShieldWhiteYellow then
    path, blink = 'icons/shield_yellow_white.png', false
  elseif shieldId == ShieldWhiteBlue then
    path, blink = 'icons/shield_blue_white.png', false
  elseif shieldId == ShieldBlue then
    path, blink = 'icons/shield_blue.png', false
  elseif shieldId == ShieldYellow then
    path, blink = 'icons/shield_yellow.png', false
  elseif shieldId == ShieldBlueSharedExp then
    path, blink = 'icons/shield_blue_shared.png', false
  elseif shieldId == ShieldYellowSharedExp then
    path, blink = 'icons/shield_yellow_shared.png', false
  elseif shieldId == ShieldBlueNoSharedExpBlink then
    path, blink = 'icons/shield_blue_not_shared.png', true
  elseif shieldId == ShieldYellowNoSharedExpBlink then
    path, blink = 'icons/shield_yellow_not_shared.png', true
  elseif shieldId == ShieldBlueNoSharedExp then
    path, blink = 'icons/shield_blue_not_shared.png', false
  elseif shieldId == ShieldYellowNoSharedExp then
    path, blink = 'icons/shield_yellow_not_shared.png', false
  end
  path = resolvepath(path)
  return path, blink
end

function getEmblemImagePath(emblemId)
  local path
  if emblemId == EmblemGreen then
    path = 'icons/emblem_green.png'
  elseif emblemId == EmblemRed then
    path = 'icons/emblem_red.png'
  elseif emblemId == EmblemBlue then
    path = 'icons/emblem_blue.png'
  end
  path = resolvepath(path)
  return path
end

function Creature:onSkullChange(skullId)
  local imagePath = getSkullImagePath(skullId)
  if imagePath then
    self:setSkullTexture(imagePath)
  end
end

function Creature:onShieldChange(shieldId)
  local imagePath, blink = getShieldImagePathAndBlink(shieldId)
  if imagePath then
    self:setShieldTexture(imagePath, blink)
  end
end

function Creature:onEmblemChange(emblemId)
  local imagePath = getEmblemImagePath(emblemId)
  if imagePath then
    self:setEmblemTexture(imagePath)
  end
end
