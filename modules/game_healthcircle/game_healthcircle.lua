mapPanel = modules.game_interface.getMapPanel()
gameRootPanel = modules.game_interface.gameBottomPanel
gameLeftPanel = modules.game_interface.getLeftPanel()
gameTopMenu = modules.client_topmenu.getTopMenu()

function currentViewMode()
  return modules.game_interface.currentViewMode
end

healthCircle = nil
manaCircle = nil

g_ui.loadUI('game_healthcircle')

healthCircleFront = nil
manaCircleFront = nil

optionPanel = nil

isHealthCircle = not g_settings.getBoolean('healthcircle_hpcircle')
isManaCircle = not g_settings.getBoolean('healthcircle_mpcircle')
isSkillCircle = g_settings.getBoolean('healthcircle_skillcircle')
skillsLoaded = false

distanceFromCenter = g_settings.getNumber('healthcircle_distfromcenter')
opacityCircle = g_settings.getNumber('healthcircle_opacity', 0.7)

function init()
  healthCircle = g_ui.createWidget('HealthCircle', mapPanel)
  manaCircle = g_ui.createWidget('ManaCircle', mapPanel)

  healthCircleFront = g_ui.createWidget('HealthCircleFront', mapPanel)
  manaCircleFront = g_ui.createWidget('ManaCircleFront', mapPanel)

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

  --Add option window in options module
  addToOptionsModule()
end

function terminate()
  healthCircle:destroy()
  healthCircle = nil
  manaCircle:destroy()
  manaCircle = nil

  healthCircleFront:destroy()
  healthCircleFront = nil
  manaCircleFront:destroy()
  manaCircleFront = nil

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
                         onManaChange = whenManaChange})
end

function terminateOnHpAndMpChange()
  disconnect(LocalPlayer, { onHealthChange = whenHealthChange,
                            onManaChange = whenManaChange })
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

    else
      healthCircleFront:setX(mapPanel:getX() + mapPanel:getWidth() / 2 - healthCircle:getWidth() - barDistance - distanceFromCenter)
      manaCircleFront:setX(mapPanel:getX() + mapPanel:getWidth() / 2 + barDistance + distanceFromCenter)
      
      healthCircle:setX(mapPanel:getX() + mapPanel:getWidth() / 2 - healthCircle:getWidth() - barDistance - distanceFromCenter)
      manaCircle:setX(mapPanel:getX() + mapPanel:getWidth() / 2 + barDistance + distanceFromCenter)

      healthCircle:setY(mapPanel:getY() + mapPanel:getHeight() / 2 - healthCircle:getHeight() / 2)
      manaCircle:setY(mapPanel:getY() + mapPanel:getHeight() / 2 - manaCircle:getHeight() / 2)

    end

    whenHealthChange()
    whenManaChange()
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

  g_settings.set('healthcircle_opacity', value)
end

-------------------------------------------------
--Option Settings--------------------------------
-------------------------------------------------

optionPanel = nil
healthCheckBox = nil
manaCheckBox = nil
distFromCenLabel = nil
distFromCenScrollbar = nil
opacityLabel = nil
opacityScrollbar = nil

function addToOptionsModule()
  --Add to options module
  optionPanel = g_ui.loadUI('option_healthcircle')
  modules.client_options.addTab(tr('Health Circle'), optionPanel, '/game_healthcircle/img_game_healthcircle/hp_mp_circle')

  --UI values
  healthCheckBox = optionPanel:recursiveGetChildById('healthCheckBox')
  manaCheckBox = optionPanel:recursiveGetChildById('manaCheckBox')
  distFromCenLabel = optionPanel:recursiveGetChildById('distFromCenLabel')
  distFromCenScrollbar = optionPanel:recursiveGetChildById('distFromCenScrollbar')
  opacityLabel = optionPanel:recursiveGetChildById('opacityLabel')
  opacityScrollbar = optionPanel:recursiveGetChildById('opacityScrollbar')

  --Set values
  healthCheckBox:setChecked(isHealthCircle)
  manaCheckBox:setChecked(isManaCircle)
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
  distFromCenLabel = nil
  distFromCenScrollbar = nil
  opacityLabel = nil
  opacityScrollbar = nil

  modules.client_options.removeTab('Health Circle')
  optionPanel = nil
end
