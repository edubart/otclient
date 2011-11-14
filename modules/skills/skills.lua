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
    local skillButton = UIButton.create()
    skillPanel:addChild(skillButton)
    skillButton:setStyle('SkillButton')

    local nameLabel = UILabel.create()
    skillButton :addChild(nameLabel)
    nameLabel:setStyle('SkillNameLabel')
    nameLabel:setText(skills[i])
    nameLabel:resizeToText()

    local levelLabel = UILabel.create()
    skillButton :addChild(levelLabel)
    levelLabel:setStyle('SkillLevelLabel')
    levelLabel:setId('skillLevelId' .. i)
    levelLabel:setText('0')

    local percentBar = UIProgressBar.create()
    skillPanel:addChild(percentBar)
    percentBar:setStyle('SkillPercentPanel')
    percentBar:setId('skillPercentId' .. i)

    skillButton.onClick = function(self)
      percentBar:setVisible(not percentBar:isVisible())
      self:updateParentLayout()
      return true
    end
  end
end

function Skills.destroy()
  skillWindow:destroy()
  skillWindow = nil
end

-- hooked events
function Game.onSkillUpdate(id, level, percent)
  local skillPanel = skillWindow:recursiveGetChildById('skillPanel')

  local levelLabel = skillPanel:recursiveGetChildById('skillLevelId' .. (id + 1))
  levelLabel:setText(level)

  local percentBar = skillPanel:getChildById('skillPercentId' .. (id + 1))
  percentBar:setPercent(percent)
  percentBar:setTooltip(percent .. "% to go")
end

connect(Game, { onLogin = Skills.create,
                onLogout = Skills.destroy })
