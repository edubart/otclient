-- @docclass
UICreatureButton = extends(UIWidget, "UICreatureButton")

local CreatureButtonColors = {
  onIdle = {notHovered = '#888888', hovered = '#FFFFFF' },
  onTargeted = {notHovered = '#FF0000', hovered = '#FF8888' },
  onFollowed = {notHovered = '#00FF00', hovered = '#88FF88' }
}

local LifeBarColors = {} -- Must be sorted by percentAbove
table.insert(LifeBarColors, {percentAbove = 92, color = '#00BC00' } )
table.insert(LifeBarColors, {percentAbove = 60, color = '#50A150' } )
table.insert(LifeBarColors, {percentAbove = 30, color = '#A1A100' } )
table.insert(LifeBarColors, {percentAbove = 8, color = '#BF0A0A' } )
table.insert(LifeBarColors, {percentAbove = 3, color = '#910F0F' } )
table.insert(LifeBarColors, {percentAbove = -1, color = '#850C0C' } )

function UICreatureButton.create()
  local button = UICreatureButton.internalCreate()
  button:setFocusable(false)
  button.creature = nil
  button.isHovered = false
  button.isTarget = false
  button.isFollowed = false
  return button
end

function UICreatureButton:setCreature(creature)
    self.creature = creature
end

function UICreatureButton:getCreature()
  return self.creature
end

function UICreatureButton:getCreatureId()
    return self.creature:getId()
end

function UICreatureButton:setup(creature)
  self.creature = creature

  local creatureWidget = self:getChildById('creature')
  local labelWidget = self:getChildById('label')
  local lifeBarWidget = self:getChildById('lifeBar')

  labelWidget:setText(creature:getName())
  creatureWidget:setCreature(creature)

  self:setId('CreatureButton_' .. creature:getName():gsub('%s','_'))
  self:setLifeBarPercent(creature:getHealthPercent())

  self:updateSkull(creature:getSkull())
  self:updateEmblem(creature:getEmblem())
end

function UICreatureButton:update()
  local color = CreatureButtonColors.onIdle
  if self.isTarget then
    color = CreatureButtonColors.onTargeted
  elseif self.isFollowed then
    color = CreatureButtonColors.onFollowed
  end
  color = self.isHovered and color.hovered or color.notHovered

  if self.isHovered or self.isTarget or self.isFollowed then
    self.creature:showStaticSquare(color)
    self:getChildById('creature'):setBorderWidth(1)
    self:getChildById('creature'):setBorderColor(color)
    self:getChildById('label'):setColor(color)
  else
    self.creature:hideStaticSquare()
    self:getChildById('creature'):setBorderWidth(0)
    self:getChildById('label'):setColor(color)
  end
end

function UICreatureButton:updateSkull(skullId)
  if not self.creature then
    return
  end
  local skullId = skullId or self.creature:getSkull()
  local skullWidget = self:getChildById('skull')
  local labelWidget = self:getChildById('label')

  if skullId ~= SkullNone then
    skullWidget:setWidth(skullWidget:getHeight())
    local imagePath = getSkullImagePath(skullId)
    skullWidget:setImageSource(imagePath)
    labelWidget:setMarginLeft(5)
  else
    skullWidget:setWidth(0)
    if self.creature:getEmblem() == EmblemNone then
      labelWidget:setMarginLeft(2)
    end
  end
end

function UICreatureButton:updateEmblem(emblemId)
  if not self.creature then
    return
  end
  local emblemId = emblemId or self.creature:getEmblem()
  local emblemWidget = self:getChildById('emblem')
  local labelWidget = self:getChildById('label')

  if emblemId ~= EmblemNone then
    emblemWidget:setWidth(emblemWidget:getHeight())
    local imagePath = getEmblemImagePath(emblemId)
    emblemWidget:setImageSource(imagePath)
    emblemWidget:setMarginLeft(5)
    labelWidget:setMarginLeft(5)
  else
    emblemWidget:setWidth(0)
    emblemWidget:setMarginLeft(0)
    if self.creature:getSkull() == SkullNone then
      labelWidget:setMarginLeft(2)
    end
  end
end

function UICreatureButton:setLifeBarPercent(percent)
  local lifeBarWidget = self:getChildById('lifeBar')
  lifeBarWidget:setPercent(percent)

  local color
  for i, v in pairs(LifeBarColors) do
    if percent > v.percentAbove then
      color = v.color
      break
    end
  end

  lifeBarWidget:setBackgroundColor(color)
end