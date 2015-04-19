vipWindow = nil
vipButton = nil
addVipWindow = nil
editVipWindow = nil
vipInfo = {}

function init()
  connect(g_game, { onGameStart = refresh,
                    onGameEnd = clear,
                    onAddVip = onAddVip,
                    onVipStateChange = onVipStateChange })


  g_keyboard.bindKeyDown('Ctrl+P', toggle)

  vipButton = modules.client_topmenu.addRightGameToggleButton('vipListButton', tr('VIP List') .. ' (Ctrl+P)', '/images/topbuttons/viplist', toggle)
  vipButton:setOn(true)
  vipWindow = g_ui.loadUI('viplist', modules.game_interface.getRightPanel())

  if not g_game.getFeature(GameAdditionalVipInfo) then
    loadVipInfo()
  end
  refresh()
  vipWindow:setup()
end

function terminate()
  g_keyboard.unbindKeyDown('Ctrl+P')
  disconnect(g_game, { onGameStart = refresh,
                       onGameEnd = clear,
                       onAddVip = onAddVip,
                       onVipStateChange = onVipStateChange })

  if not g_game.getFeature(GameAdditionalVipInfo) then
    saveVipInfo()
  end

  if addVipWindow then
    addVipWindow:destroy()
  end

  if editVipWindow then
    editVipWindow:destroy()
  end

  vipWindow:destroy()
  vipButton:destroy()
end

function loadVipInfo()
  local settings = g_settings.getNode('VipList')
  if not settings then
    vipInfo = {}
    return
  end
  vipInfo = settings['VipInfo'] or {}
end

function saveVipInfo()
  settings = {}
  settings['VipInfo'] = vipInfo
  g_settings.mergeNode('VipList', settings)
end


function refresh()
  clear()
  for id,vip in pairs(g_game.getVips()) do
    onAddVip(id, unpack(vip))
  end

  vipWindow:setContentMinimumHeight(38)
end

function clear()
  local vipList = vipWindow:getChildById('contentsPanel')
  vipList:destroyChildren()
end

function toggle()
  if vipButton:isOn() then
    vipWindow:close()
    vipButton:setOn(false)
  else
    vipWindow:open()
    vipButton:setOn(true)
  end
end

function onMiniWindowClose()
  vipButton:setOn(false)
end

function createAddWindow()
  if not addVipWindow then
    addVipWindow = g_ui.displayUI('addvip')
  end
end

function createEditWindow(widget)
  if editVipWindow then
    return
  end

  editVipWindow = g_ui.displayUI('editvip')

  local name = widget:getText()
  local id = widget:getId():sub(4)

  local okButton = editVipWindow:getChildById('buttonOK')
  local cancelButton = editVipWindow:getChildById('buttonCancel')

  local nameLabel = editVipWindow:getChildById('nameLabel')
  nameLabel:setText(name)

  local descriptionText = editVipWindow:getChildById('descriptionText')
  descriptionText:appendText(widget:getTooltip())

  local notifyCheckBox = editVipWindow:getChildById('checkBoxNotify')
  notifyCheckBox:setChecked(widget.notifyLogin)

  local iconRadioGroup = UIRadioGroup.create()
  for i = VipIconFirst, VipIconLast do
    iconRadioGroup:addWidget(editVipWindow:recursiveGetChildById('icon' .. i))
  end
  iconRadioGroup:selectWidget(editVipWindow:recursiveGetChildById('icon' .. widget.iconId))

  local cancelFunction = function()
    editVipWindow:destroy()
    iconRadioGroup:destroy()
    editVipWindow = nil
  end

  local saveFunction = function()
    local vipList = vipWindow:getChildById('contentsPanel')
    if not widget or not vipList:hasChild(widget) then
      cancelFunction()
      return
    end

    local name = widget:getText()
    local state = widget.vipState
    local description = descriptionText:getText()
    local iconId = tonumber(iconRadioGroup:getSelectedWidget():getId():sub(5))
    local notify = notifyCheckBox:isChecked()

    if g_game.getFeature(GameAdditionalVipInfo) then
      g_game.editVip(id, description, iconId, notify)
    else
      if notify ~= false or #description > 0 or iconId > 0 then
        vipInfo[id] = {description = description, iconId = iconId, notifyLogin = notify}
      else
        vipInfo[id] = nil
      end
    end

    widget:destroy()
    onAddVip(id, name, state, description, iconId, notify)

    editVipWindow:destroy()
    iconRadioGroup:destroy()
    editVipWindow = nil
  end

  cancelButton.onClick = cancelFunction
  okButton.onClick = saveFunction

  editVipWindow.onEscape = cancelFunction
  editVipWindow.onEnter = saveFunction
end

function destroyAddWindow()
  addVipWindow:destroy()
  addVipWindow = nil
end

function addVip()
  g_game.addVip(addVipWindow:getChildById('name'):getText())
  destroyAddWindow()
end

function removeVip(widgetOrName)
  if not widgetOrName then
    return
  end

  local widget
  local vipList = vipWindow:getChildById('contentsPanel')
  if type(widgetOrName) == 'string' then
    local entries = vipList:getChildren()
    for i = 1, #entries do
      if entries[i]:getText():lower() == widgetOrName:lower() then
        widget = entries[i]
        break
      end
    end
    if not widget then
      return
    end
  else
    widget = widgetOrName
  end

  if widget then
    local id = widget:getId():sub(4)
    g_game.removeVip(id)
    vipList:removeChild(widget)
    if vipInfo[id] and g_game.getFeature(GameAdditionalVipInfo) then
      vipInfo[id] = nil
    end
  end
end

function hideOffline(state)
  settings = {}
  settings['hideOffline'] = state
  g_settings.mergeNode('VipList', settings)

  refresh()
end

function isHiddingOffline()
  local settings = g_settings.getNode('VipList')
  if not settings then
    return false
  end
  return settings['hideOffline']
end

function getSortedBy()
  local settings = g_settings.getNode('VipList')
  if not settings or not settings['sortedBy'] then
    return 'status'
  end
  return settings['sortedBy']
end

function sortBy(state)
  settings = {}
  settings['sortedBy'] = state
  g_settings.mergeNode('VipList', settings)

  refresh()
end

function onAddVip(id, name, state, description, iconId, notify)
  local vipList = vipWindow:getChildById('contentsPanel')

  local label = g_ui.createWidget('VipListLabel')
  label.onMousePress = onVipListLabelMousePress
  label:setId('vip' .. id)
  label:setText(name)

  if not g_game.getFeature(GameAdditionalVipInfo) then
    local tmpVipInfo = vipInfo[tostring(id)]
    label.iconId = 0
    label.notifyLogin = false
    if tmpVipInfo then
      if tmpVipInfo.iconId then
        label:setImageClip(torect((tmpVipInfo.iconId * 12) .. ' 0 12 12'))
        label.iconId = tmpVipInfo.iconId
      end
      if tmpVipInfo.description then
        label:setTooltip(tmpVipInfo.description)
      end
      label.notifyLogin = tmpVipInfo.notifyLogin or false
    end
  else
    label:setTooltip(description)
    label:setImageClip(torect((iconId * 12) .. ' 0 12 12'))
    label.iconId = iconId
    label.notifyLogin = notify
  end

  if state == VipState.Online then
    label:setColor('#00ff00')
  elseif state == VipState.Pending then
    label:setColor('#ffca38')
  else
    label:setColor('#ff0000')
  end

  label.vipState = state

  label:setPhantom(false)
  connect(label, { onDoubleClick = function () g_game.openPrivateChannel(label:getText()) return true end } )

  if state == VipState.Offline and isHiddingOffline() then
    label:setVisible(false)
  end

  local nameLower = name:lower()
  local childrenCount = vipList:getChildCount()

  for i=1,childrenCount do
    local child = vipList:getChildByIndex(i)
    if (state == VipState.Online and child.vipState ~= VipState.Online and getSortedBy() == 'status')
        or (label.iconId > child.iconId and getSortedBy() == 'type') then
      vipList:insertChild(i, label)
      return
    end

    if (((state ~= VipState.Online and child.vipState ~= VipState.Online) or (state == VipState.Online and child.vipState == VipState.Online)) and getSortedBy() == 'status')
        or (label.iconId == child.iconId and getSortedBy() == 'type') or getSortedBy() == 'name' then

      local childText = child:getText():lower()
      local length = math.min(childText:len(), nameLower:len())

      for j=1,length do
        if nameLower:byte(j) < childText:byte(j) then
          vipList:insertChild(i, label)
          return
        elseif nameLower:byte(j) > childText:byte(j) then
          break
        elseif j == nameLower:len() then -- We are at the end of nameLower, and its shorter than childText, thus insert before
          vipList:insertChild(i, label)
          return
        end
      end
    end
  end

  vipList:insertChild(childrenCount+1, label)
end

function onVipStateChange(id, state)
  local vipList = vipWindow:getChildById('contentsPanel')
  local label = vipList:getChildById('vip' .. id)
  local name = label:getText()
  local description = label:getTooltip()
  local iconId = label.iconId
  local notify = label.notifyLogin
  label:destroy()

  onAddVip(id, name, state, description, iconId, notify)

  if notify and state ~= VipState.Pending then
    modules.game_textmessage.displayFailureMessage(tr('%s has logged %s.', name, (state == VipState.Online and 'in' or 'out')))
  end
end

function onVipListMousePress(widget, mousePos, mouseButton)
  if mouseButton ~= MouseRightButton then return end

  local vipList = vipWindow:getChildById('contentsPanel')

  local menu = g_ui.createWidget('PopupMenu')
  menu:setGameMenu(true)
  menu:addOption(tr('Add new VIP'), function() createAddWindow() end)

  menu:addSeparator()
  if not isHiddingOffline() then
    menu:addOption(tr('Hide Offline'), function() hideOffline(true) end)
  else
    menu:addOption(tr('Show Offline'), function() hideOffline(false) end)
  end

  if not(getSortedBy() == 'name') then
    menu:addOption(tr('Sort by name'), function() sortBy('name') end)
  end

  if not(getSortedBy() == 'status') then
    menu:addOption(tr('Sort by status'), function() sortBy('status') end)
  end

  if not(getSortedBy() == 'type') then
    menu:addOption(tr('Sort by type'), function() sortBy('type') end)
  end

  menu:display(mousePos)

  return true
end

function onVipListLabelMousePress(widget, mousePos, mouseButton)
  if mouseButton ~= MouseRightButton then return end

  local vipList = vipWindow:getChildById('contentsPanel')

  local menu = g_ui.createWidget('PopupMenu')
  menu:setGameMenu(true)
  menu:addOption(tr('Send Message'), function() g_game.openPrivateChannel(widget:getText()) end)
  menu:addOption(tr('Add new VIP'), function() createAddWindow() end)
  menu:addOption(tr('Edit %s', widget:getText()), function() if widget then createEditWindow(widget) end end)
  menu:addOption(tr('Remove %s', widget:getText()), function() if widget then removeVip(widget) end end)
  menu:addSeparator()
  menu:addOption(tr('Copy Name'), function() g_window.setClipboardText(widget:getText()) end)

  if modules.game_console.getOwnPrivateTab() then
    menu:addSeparator()
    menu:addOption(tr('Invite to private chat'), function() g_game.inviteToOwnChannel(widget:getText()) end)
    menu:addOption(tr('Exclude from private chat'), function() g_game.excludeFromOwnChannel(widget:getText()) end)
  end

  if not isHiddingOffline() then
    menu:addOption(tr('Hide Offline'), function() hideOffline(true) end)
  else
    menu:addOption(tr('Show Offline'), function() hideOffline(false) end)
  end

  if not(getSortedBy() == 'name') then
    menu:addOption(tr('Sort by name'), function() sortBy('name') end)
  end

  if not(getSortedBy() == 'status') then
    menu:addOption(tr('Sort by status'), function() sortBy('status') end)
  end

  menu:display(mousePos)

  return true
end
