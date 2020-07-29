mapPanel = modules.game_interface.getMapPanel()
gameRootPanel = modules.game_interface.gameBottomPanel
gameLeftPanel = modules.game_interface.getLeftPanel()
gameTopMenu = modules.client_topmenu.getTopMenu()

function currentViewMode()
  return modules.game_interface.currentViewMode
end

healthCircle = nil
manaCircle = nil
expCircle = nil
skillCircle = nil

g_ui.loadUI('game_healthcircle')

healthCircleFront = nil
manaCircleFront = nil
expCircleFront = nil
skillCircleFront = nil

optionPanel = nil

isHealthCircle = not g_settings.getBoolean('healthcircle_hpcircle')
isManaCircle = not g_settings.getBoolean('healthcircle_mpcircle')
isExpCircle = g_settings.getBoolean('healthcircle_expcircle')
isSkillCircle = g_settings.getBoolean('healthcircle_skillcircle')
skillType = g_settings.getString('healthcircle_skilltype')
skillsLoaded = false

distanceFromCenter = g_settings.getNumber('healthcircle_distfromcenter')
opacityCircle = g_settings.getNumber('healthcircle_opacity', 0.7)

if not (skillType == 'magic' or skillType == 'fist' or skillType == 'club' or skillType == 'sword' or skillType == 'axe' or skillType == 'distance' or skillType == 'shielding' or skillType == 'fishing') then
  skillType = 'magic'
end

function init()
  healthCircle = g_ui.createWidget('HealthCircle', mapPanel)
  manaCircle = g_ui.createWidget('ManaCircle', mapPanel)
  expCircle = g_ui.createWidget('ExpCircle', mapPanel)
  skillCircle = g_ui.createWidget('SkillCircle', mapPanel)

  healthCircleFront = g_ui.createWidget('HealthCircleFront', mapPanel)
  manaCircleFront = g_ui.createWidget('ManaCircleFront', mapPanel)
  expCircleFront = g_ui.createWidget('ExpCircleFront', mapPanel)
  skillCircleFront = g_ui.createWidget('SkillCircleFront', mapPanel)

  whenMapResizeChange()
  initOnHpAndMpChange()
  initOnGeometryChange()
  initOnLoginChange()

  if not isHealthCircle then
    healthCircle:setVisible(false)
    healthCircleFront:setVisible(false)
  end

  if not isManaCircle then
    manaCircle:setVisible(false)
    manaCircleFront:setVisible(false)
  end

  if not isExpCircle then
    expCircle:setVisible(false)
    expCircleFront:setVisible(false)
  end

  if not isSkillCircle then
    skillCircle:setVisible(false)
    skillCircleFront:setVisible(false)
  end

  --Add option window in options module
  addToOptionsModule()
end

function terminate()
  healthCircle:destroy()
  healthCircle = nil
  manaCircle:destroy()
  manaCircle = nil
  expCircle:destroy()
  expCircle = nil
  skillCircle:destroy()
  skillCircle = nil

  healthCircleFront:destroy()
  healthCircleFront = nil
  manaCircleFront:destroy()
  manaCircleFront = nil
  expCircleFront:destroy()
  expCircleFront = nil
  skillCircleFront:destroy()
  skillCircleFront = nil

  terminateOnHpAndMpChange()
  terminateOnGeometryChange()
  terminateOnLoginChange()

  --Delete from options module
  destroyOptionsModule()
end

-------------------------------------------------
--Scripts----------------------------------------
-------------------------------------------------

function initOnHpAndMpChange()
  connect(LocalPlayer, { onHealthChange = whenHealthChange,
                         onManaChange = whenManaChange,
                         onSkillChange = whenSkillsChange,
                         onMagicLevelChange = whenSkillsChange,
                         onLevelChange = whenSkillsChange })
end

function terminateOnHpAndMpChange()
  disconnect(LocalPlayer, { onHealthChange = whenHealthChange,
                            onManaChange = whenManaChange,
                            onSkillChange = whenSkillsChange,
                            onMagicLevelChange = whenSkillsChange,
                            onLevelChange = whenSkillsChange })
end

function initOnGeometryChange()
  connect(mapPanel, { onGeometryChange = whenMapResizeChange })
end

function terminateOnGeometryChange()
  disconnect(mapPanel, { onGeometryChange = whenMapResizeChange })
end

function initOnLoginChange()
  connect(g_game, { onGameStart = whenMapResizeChange })
end

function terminateOnLoginChange()
  disconnect(g_game, { onGameStart = whenMapResizeChange })
end

function whenHealthChange()
  if g_game.isOnline() then
    local healthPercent = math.floor(g_game.getLocalPlayer():getHealthPercent())
    local Yhppc = math.floor(208 * (1 - (healthPercent / 100)))
    local rect = { x = 0, y = Yhppc, width = 63, height = 208 }
    healthCircleFront:setImageClip(rect)

    if healthPercent > 92 then
      healthCircleFront:setImageColor("#00BC00")
    elseif healthPercent > 60 then
      healthCircleFront:setImageColor("#50A150")
    elseif healthPercent > 30 then
      healthCircleFront:setImageColor("#A1A100")
    elseif healthPercent > 8 then
      healthCircleFront:setImageColor("#BF0A0A")
    elseif healthPercent > 3 then
      healthCircleFront:setImageColor("#910F0F")
    else
      healthCircleFront:setImageColor("#850C0C")
    end

    healthCircleFront:setY(healthCircle:getY() + Yhppc)
  end
end

function whenManaChange()
  if g_game.isOnline() then
    local manaPercent = math.floor(g_game.getLocalPlayer():getMaxMana() - (g_game.getLocalPlayer():getMaxMana() - g_game.getLocalPlayer():getMana())) * 100 / g_game.getLocalPlayer():getMaxMana()
    local Ymppc = math.floor(208 * (1 - (manaPercent / 100)))
    local rect = { x = 0, y = Ymppc, width = 63, height = 208 }
    manaCircleFront:setImageClip(rect)
    
    manaCircleFront:setY(manaCircle:getY() + Ymppc)
  end
end

function whenSkillsChange()
  if g_game.isOnline() then
    if isExpCircle then
      local player = g_game.getLocalPlayer()
      local Xexpc = math.floor(208 * (1 - player:getLevelPercent() / 100))
      local rect = { x = -1 * Xexpc, y = 0, width = 208, height = 63 }
      expCircleFront:setImageClip(rect)

      expCircleFront:setX(expCircle:getX() - Xexpc)
    end

    if isSkillCircle then
      local player = g_game.getLocalPlayer()
      Xskillpc = 0

      if skillType == 'magic' then
        Xskillpc = math.floor(208 * (1 - player:getMagicLevelPercent() / 100))
        skillCircleFront:setImageColor('#00ffcc')
      elseif skillType == 'fist' then
        Xskillpc = math.floor(208 * (1 - player:getSkillLevelPercent(0) / 100))
        skillCircleFront:setImageColor('#9900cc')
      elseif skillType == 'club' then
        Xskillpc = math.floor(208 * (1 - player:getSkillLevelPercent(1) / 100))
        skillCircleFront:setImageColor('#cc3399')
      elseif skillType == 'sword' then
        Xskillpc = math.floor(208 * (1 - player:getSkillLevelPercent(2) / 100))
        skillCircleFront:setImageColor('#FF7F00')
      elseif skillType == 'axe' then
        Xskillpc = math.floor(208 * (1 - player:getSkillLevelPercent(3) / 100))
        skillCircleFront:setImageColor('#696969')
      elseif skillType == 'distance' then
        Xskillpc = math.floor(208 * (1 - player:getSkillLevelPercent(4) / 100))
        skillCircleFront:setImageColor('#A62A2A')
      elseif skillType == 'shielding' then
        Xskillpc = math.floor(208 * (1 - player:getSkillLevelPercent(5) / 100))
        skillCircleFront:setImageColor('#663300')
      elseif skillType == 'fishing' then
        Xskillpc = math.floor(208 * (1 - player:getSkillLevelPercent(6) / 100))
        skillCircleFront:setImageColor('#ffff33')
      end
      
      local rect = { x = -1 * Xskillpc, y = 0, width = 208, height = 63 }
      skillCircleFront:setImageClip(rect)

      skillCircleFront:setX(expCircle:getX() - Xskillpc)
    end
  end
end

function whenMapResizeChange()
  if g_game.isOnline() then

    local barDistance = 90
    if not (math.floor(mapPanel:getHeight() / 2 * 0.2) < 100) then --0.381
      barDistance = math.floor(mapPanel:getHeight() / 2 * 0.2)
    end

    if currentViewMode() == 2 then
      healthCircleFront:setX(math.floor(mapPanel:getWidth() / 2 - barDistance - healthCircle:getWidth()) - distanceFromCenter)
      manaCircleFront:setX(math.floor(mapPanel:getWidth() / 2 + barDistance) + distanceFromCenter)

      healthCircle:setX(math.floor(mapPanel:getWidth() / 2 - barDistance - healthCircle:getWidth()) - distanceFromCenter)
      manaCircle:setX(math.floor((mapPanel:getWidth() / 2 + barDistance)) + distanceFromCenter)

      healthCircle:setY(mapPanel:getHeight() / 2 - healthCircle:getHeight() / 2 + 0)
      manaCircle:setY(mapPanel:getHeight() / 2 - manaCircle:getHeight() / 2 + 0)

      if isExpCircle then
        expCircleFront:setY(math.floor(mapPanel:getHeight() / 2 - barDistance - expCircle:getHeight()) - distanceFromCenter)

        expCircle:setX(math.floor(mapPanel:getWidth() / 2 - expCircle:getWidth() / 2))
        expCircle:setY(math.floor(mapPanel:getHeight() / 2 - barDistance - expCircle:getHeight()) - distanceFromCenter)
      end

      if isSkillCircle then
        skillCircleFront:setY(math.floor(mapPanel:getHeight() / 2 + barDistance) + distanceFromCenter)

        skillCircle:setX(math.floor(mapPanel:getWidth() / 2 - skillCircle:getWidth() / 2))
        skillCircle:setY(math.floor(mapPanel:getHeight() / 2 + barDistance) + distanceFromCenter)
      end
    else
      healthCircleFront:setX(mapPanel:getX() + mapPanel:getWidth() / 2 - healthCircle:getWidth() - barDistance - distanceFromCenter)
      manaCircleFront:setX(mapPanel:getX() + mapPanel:getWidth() / 2 + barDistance + distanceFromCenter)
      
      healthCircle:setX(mapPanel:getX() + mapPanel:getWidth() / 2 - healthCircle:getWidth() - barDistance - distanceFromCenter)
      manaCircle:setX(mapPanel:getX() + mapPanel:getWidth() / 2 + barDistance + distanceFromCenter)

      healthCircle:setY(mapPanel:getY() + mapPanel:getHeight() / 2 - healthCircle:getHeight() / 2)
      manaCircle:setY(mapPanel:getY() + mapPanel:getHeight() / 2 - manaCircle:getHeight() / 2)

      if isExpCircle then
        expCircleFront:setY(mapPanel:getY() + mapPanel:getHeight() / 2 - expCircle:getHeight() - barDistance - distanceFromCenter)

        expCircle:setX(mapPanel:getX() + mapPanel:getWidth() / 2 - expCircle:getWidth() / 2)
        expCircle:setY(mapPanel:getY() + mapPanel:getHeight() / 2 - expCircle:getHeight() - barDistance - distanceFromCenter)
      end

      if isSkillCircle then
        skillCircleFront:setY(mapPanel:getY() + mapPanel:getHeight() / 2 + barDistance + distanceFromCenter)

        skillCircle:setX(mapPanel:getX() + mapPanel:getWidth() / 2 - skillCircle:getWidth() / 2)
        skillCircle:setY(mapPanel:getY() + mapPanel:getHeight() / 2 + barDistance + distanceFromCenter)
      end
    end

    whenHealthChange()
    whenManaChange()
    if isExpCircle or isSkillCircle then
      whenSkillsChange()
    end
  end
end

-------------------------------------------------
--Controls---------------------------------------
-------------------------------------------------

function setHealthCircle(value)
  value = toboolean(value)
  isHealthCircle = value
  if value then
    healthCircle:setVisible(true)
    healthCircleFront:setVisible(true)
    whenMapResizeChange()
  else
    healthCircle:setVisible(false)
    healthCircleFront:setVisible(false)
  end

  g_settings.set('healthcircle_hpcircle', not value)
end

function setManaCircle(value)
  value = toboolean(value)
  isManaCircle = value
  if value then
    manaCircle:setVisible(true)
    manaCircleFront:setVisible(true)
    whenMapResizeChange()
  else
    manaCircle:setVisible(false)
    manaCircleFront:setVisible(false)
  end

  g_settings.set('healthcircle_mpcircle', not value)
end

function setExpCircle(value)
  value = toboolean(value)
  isExpCircle = value

  if value then
    expCircle:setVisible(true)
    expCircleFront:setVisible(true)
    whenMapResizeChange()
  else
    expCircle:setVisible(false)
    expCircleFront:setVisible(false)
  end

  g_settings.set('healthcircle_expcircle', value)
end

function setSkillCircle(value)
  value = toboolean(value)
  isSkillCircle = value

  if value then
    skillCircle:setVisible(true)
    skillCircleFront:setVisible(true)
    whenMapResizeChange()
  else
    skillCircle:setVisible(false)
    skillCircleFront:setVisible(false)
  end

  g_settings.set('healthcircle_skillcircle', value)
end

function setSkillType(skill)
  if not skillsLoaded then
    return
  end
  
  if skill == 'magic' or skill == 'fist' or skill == 'club' or skill == 'sword' or skill == 'axe' or skill == 'distance' or skill == 'shielding' or skill == 'fishing' then
    skillType = skill
    whenMapResizeChange()
    g_settings.set('healthcircle_skilltype', skill)
  else
    if not skillType then
      skillType = 'magic'
      whenMapResizeChange()
      g_settings.set('healthcircle_skilltype', 'magic')
    end
  end
end

function setDistanceFromCenter(value)
  distanceFromCenter = value
  whenMapResizeChange()

  g_settings.set('healthcircle_distfromcenter', value)
end

function setCircleOpacity(value)
  healthCircle:setOpacity(value)
  healthCircleFront:setOpacity(value)
  manaCircle:setOpacity(value)
  manaCircleFront:setOpacity(value)
  expCircle:setOpacity(value)
  expCircleFront:setOpacity(value)
  skillCircle:setOpacity(value)
  skillCircleFront:setOpacity(value)

  g_settings.set('healthcircle_opacity', value)
end

-------------------------------------------------
--Option Settings--------------------------------
-------------------------------------------------

optionPanel = nil
healthCheckBox = nil
manaCheckBox = nil
experienceCheckBox = nil
skillCheckBox = nil
chooseSkillComboBox = nil
distFromCenLabel = nil
distFromCenScrollbar = nil
opacityLabel = nil
opacityScrollbar = nil

function addToOptionsModule()
  --Add to options module
  optionPanel = g_ui.loadUI('option_healthcircle')
  modules.client_options.addTab(tr('Hp Mp Circle'), optionPanel, '/game_healthcircle/img_game_healthcircle/hp_mp_circle')

  --UI values
  healthCheckBox = optionPanel:recursiveGetChildById('healthCheckBox')
  manaCheckBox = optionPanel:recursiveGetChildById('manaCheckBox')
  experienceCheckBox = optionPanel:recursiveGetChildById('experienceCheckBox')
  skillCheckBox = optionPanel:recursiveGetChildById('skillCheckBox')
  chooseSkillComboBox = optionPanel:recursiveGetChildById('chooseSkillComboBox')
  distFromCenLabel = optionPanel:recursiveGetChildById('distFromCenLabel')
  distFromCenScrollbar = optionPanel:recursiveGetChildById('distFromCenScrollbar')
  opacityLabel = optionPanel:recursiveGetChildById('opacityLabel')
  opacityScrollbar = optionPanel:recursiveGetChildById('opacityScrollbar')

  --ComboBox start values 
  chooseSkillComboBox:addOption('magic')
  chooseSkillComboBox:addOption('fist')
  chooseSkillComboBox:addOption('club')
  chooseSkillComboBox:addOption('sword')
  chooseSkillComboBox:addOption('axe')
  chooseSkillComboBox:addOption('distance')
  chooseSkillComboBox:addOption('shielding')
  chooseSkillComboBox:addOption('fishing')

  --Set values
  healthCheckBox:setChecked(isHealthCircle)
  manaCheckBox:setChecked(isManaCircle)
  experienceCheckBox:setChecked(isExpCircle)
  skillCheckBox:setChecked(isSkillCircle)
  chooseSkillComboBox:setOption(skillType)
  --Prevent skill overwritten before initialize
  skillsLoaded = true

  distFromCenLabel:setText('Distance: ' .. distanceFromCenter)
  distFromCenScrollbar:setValue(distanceFromCenter)
  opacityLabel:setText('Opacity: ' .. opacityCircle)
  opacityScrollbar:setValue(opacityCircle * 100)
end

function destroyOptionsModule()
  healthCheckBox = nil
  manaCheckBox = nil
  experienceCheckBox = nil
  skillCheckBox = nil
  chooseSkillComboBox = nil
  distFromCenLabel = nil
  distFromCenScrollbar = nil
  opacityLabel = nil
  opacityScrollbar = nil

  modules.client_options.removeTab('Hp Mp Circle')
  optionPanel = nil
end
