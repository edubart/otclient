Skills = {}

-- private variables
local skillWindow = nil
local skills = {"Fist Fighting", "Club Fighting", "Sword Fighting", "Axe Fighting", "Distance Fighting", "Shielding", "Fishing"}

-- public functions
function Skills.create()
  skillWindow = loadUI("/skills/skills.otui", Game.gameRightPanel)

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
    levelLabel:setText('0')

    local percentBar = UIProgressBar.create()
    skillPanel:addChild(percentBar)
    percentBar:setStyle('SkillPercentPanel')
    percentBar:setId('skillPercentId' .. i)
  end
end

function Skills.destroy()
  skillWindow:destroy()
  skillWindow = nil
end

-- hooked events
function Game.onSkillUpdate(id, level, percent)
  local skillPanel = skillWindow:getChildById('skillPanel')

  local levelLabel = skillPanel:getChildById('skillLevelId' .. (id + 1))
  levelLabel:setText(level)

  local percentBar = skillPanel:getChildById('skillPercentId' .. (id + 1))
  percentBar:setPercent(percent)
  percentBar:setTooltip(percent .. "% to go")
end

connect(Game, { onLogin = Skills.create,
                onLogout = Skills.destroy })
