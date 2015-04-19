unjustifiedPointsWindow = nil
unjustifiedPointsButton = nil
contentsPanel = nil

openPvpSituationsLabel = nil
currentSkullWidget = nil
skullTimeLabel = nil

dayProgressBar = nil
weekProgressBar = nil
monthProgressBar = nil

daySkullWidget = nil
weekSkullWidget = nil
monthSkullWidget = nil

function init()
  connect(g_game, { onGameStart = online,
                    onUnjustifiedPointsChange = onUnjustifiedPointsChange,
                    onOpenPvpSituationsChange = onOpenPvpSituationsChange })
  connect(LocalPlayer, { onSkullChange = onSkullChange } )

  unjustifiedPointsButton = modules.client_topmenu.addRightGameToggleButton('unjustifiedPointsButton',
    tr('Unjustified Points'), '/images/topbuttons/unjustifiedpoints', toggle)
  unjustifiedPointsButton:setOn(true)
  unjustifiedPointsButton:hide()

  unjustifiedPointsWindow = g_ui.loadUI('unjustifiedpoints', modules.game_interface.getRightPanel())
  unjustifiedPointsWindow:disableResize()
  unjustifiedPointsWindow:setup()

  contentsPanel = unjustifiedPointsWindow:getChildById('contentsPanel')

  openPvpSituationsLabel = contentsPanel:getChildById('openPvpSituationsLabel')
  currentSkullWidget = contentsPanel:getChildById('currentSkullWidget')
  skullTimeLabel = contentsPanel:getChildById('skullTimeLabel')

  dayProgressBar = contentsPanel:getChildById('dayProgressBar')
  weekProgressBar = contentsPanel:getChildById('weekProgressBar')
  monthProgressBar = contentsPanel:getChildById('monthProgressBar')
  daySkullWidget = contentsPanel:getChildById('daySkullWidget')
  weekSkullWidget = contentsPanel:getChildById('weekSkullWidget')
  monthSkullWidget = contentsPanel:getChildById('monthSkullWidget')

  if g_game.isOnline() then
    online()
  end
end

function terminate()
  disconnect(g_game, { onGameStart = online,
                       onUnjustifiedPointsChange = onUnjustifiedPointsChange,
                       onOpenPvpSituationsChange = onOpenPvpSituationsChange })
  disconnect(LocalPlayer, { onSkullChange = onSkullChange } )

  unjustifiedPointsWindow:destroy()
  unjustifiedPointsButton:destroy()
end

function onMiniWindowClose()
  unjustifiedPointsButton:setOn(false)
end

function toggle()
  if unjustifiedPointsButton:isOn() then
    unjustifiedPointsWindow:close()
    unjustifiedPointsButton:setOn(false)
  else
    unjustifiedPointsWindow:open()
    unjustifiedPointsButton:setOn(true)
  end
end

function online()
  if g_game.getFeature(GameUnjustifiedPoints) then
    unjustifiedPointsButton:show()
  else
    unjustifiedPointsButton:hide()
    unjustifiedPointsWindow:close()
  end

  refresh()
end

function refresh()
  local localPlayer = g_game.getLocalPlayer()

  local unjustifiedPoints = g_game.getUnjustifiedPoints()
  onUnjustifiedPointsChange(unjustifiedPoints)

  onSkullChange(localPlayer, localPlayer:getSkull())
  onOpenPvpSituationsChange(g_game.getOpenPvpSituations())
end

function onSkullChange(localPlayer, skull)
  if not localPlayer:isLocalPlayer() then return end

  if skull == SkullRed or skull == SkullBlack then
    currentSkullWidget:setIcon(getSkullImagePath(skull))
    currentSkullWidget:setTooltip('Remaining skull time')
  else
    currentSkullWidget:setIcon('')
    currentSkullWidget:setTooltip('You have no skull')
  end

  daySkullWidget:setIcon(getSkullImagePath(getNextSkullId(skull)))
  weekSkullWidget:setIcon(getSkullImagePath(getNextSkullId(skull)))
  monthSkullWidget:setIcon(getSkullImagePath(getNextSkullId(skull)))
end

function onOpenPvpSituationsChange(amount)
  openPvpSituationsLabel:setText(amount)
end

local function getColorByKills(kills)
  if kills < 2 then
    return 'red'
  elseif kills < 3 then
    return 'yellow'
  end

  return 'green'
end

function onUnjustifiedPointsChange(unjustifiedPoints)
  if unjustifiedPoints.skullTime == 0 then
    skullTimeLabel:setText('No skull')
    skullTimeLabel:setTooltip('You have no skull')
  else
    skullTimeLabel:setText(unjustifiedPoints.skullTime .. ' days')
    skullTimeLabel:setTooltip('Remaining skull time')
  end

  dayProgressBar:setValue(unjustifiedPoints.killsDay, 0, 100)
  dayProgressBar:setBackgroundColor(getColorByKills(unjustifiedPoints.killsDayRemaining))
  dayProgressBar:setTooltip(string.format('Unjustified points gained during the last 24 hours.\n%i kill%s left.', unjustifiedPoints.killsDayRemaining, (unjustifiedPoints.killsDayRemaining == 1 and '' or 's')))
  dayProgressBar:setText(string.format('%i kill%s left',  unjustifiedPoints.killsDayRemaining, (unjustifiedPoints.killsDayRemaining == 1 and '' or 's')))

  weekProgressBar:setValue(unjustifiedPoints.killsWeek, 0, 100)
  weekProgressBar:setBackgroundColor(getColorByKills(unjustifiedPoints.killsWeekRemaining))
  weekProgressBar:setTooltip(string.format('Unjustified points gained during the last 7 days.\n%i kill%s left.', unjustifiedPoints.killsWeekRemaining, (unjustifiedPoints.killsWeekRemaining == 1 and '' or 's')))
  weekProgressBar:setText(string.format('%i kill%s left',  unjustifiedPoints.killsWeekRemaining, (unjustifiedPoints.killsWeekRemaining == 1 and '' or 's')))

  monthProgressBar:setValue(unjustifiedPoints.killsMonth, 0, 100)
  monthProgressBar:setBackgroundColor(getColorByKills(unjustifiedPoints.killsMonthRemaining))
  monthProgressBar:setTooltip(string.format('Unjustified points gained during the last 30 days.\n%i kill%s left.', unjustifiedPoints.killsMonthRemaining, (unjustifiedPoints.killsMonthRemaining == 1 and '' or 's')))
  monthProgressBar:setText(string.format('%i kill%s left',  unjustifiedPoints.killsMonthRemaining, (unjustifiedPoints.killsMonthRemaining == 1 and '' or 's')))
end
