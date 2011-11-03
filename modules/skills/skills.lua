skillWindow = nil

local skills = {"Fist Fighting", "Club Fighting", "Sword Fighting", "Axe Fighting", "Distance Fighting", "Shielding", "Fishing"}

function csw()
  skillWindow = loadUI("/game/ui/skillwindow.otui", UI.root)

  local skillPanel = skillWindow:getChildById('skillPanel')

  -- create first widget cause of layout
  local widget = UIWidget.create()
  skillPanel:addChild(widget)
  widget:setStyle('SkillFirstWidget')

  -- create skills
  for i=1,#skills,1 do
    local nameLabel = UILabel.create()
    skillPanel:addChild(nameLabel)
    nameLabel:setStyle('SkillNameLabel')
    nameLabel:setText(skills[i])

    local levelLabel = UILabel.create()
    skillPanel:addChild(levelLabel)
    levelLabel:setStyle('SkillLevelLabel')
    levelLabel:setId('skillLevelId' .. i)
    levelLabel:setText('10')

    local percentPanel = UIWidget.create()
    skillPanel:addChild(percentPanel)
    percentPanel:setStyle('SkillPercentPanel')
  end
end

function Game.setSkill(id, level, percent)
  local skillPanel = skillWindow:getChildById('skillPanel')
  local levelLabel = skillPanel:getChildById('skillLevel' .. id)
end