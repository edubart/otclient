vipWindow = nil
vipButton = nil
addVipWindow = nil

function init()
  connect(g_game, { onGameStart = refresh,
                    onGameEnd = clear,
                    onAddVip = onAddVip,
                    onVipStateChange = onVipStateChange })


  g_keyboard.bindKeyDown('Ctrl+P', toggle)

  vipButton = modules.client_topmenu.addRightGameToggleButton('vipListButton', tr('VIP List') .. ' (Ctrl+P)', '/images/topbuttons/viplist', toggle)
  vipButton:setOn(true)
  vipWindow = g_ui.loadUI('viplist', modules.game_interface.getRightPanel())

  refresh()
  vipWindow:setup()
end

function terminate()
  g_keyboard.unbindKeyDown('Ctrl+P')
  disconnect(g_game, { onGameStart = refresh,
                       onGameEnd = clear,
                       onAddVip = onAddVip,
                       onVipStateChange = onVipStateChange })

  vipWindow:destroy()
  vipButton:destroy()
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
  addVipWindow = g_ui.displayUI('addvip')
end

function destroyAddWindow()
  addVipWindow:destroy()
  addVipWindow = nil
end

function addVip()
  g_game.addVip(addVipWindow:getChildById('name'):getText())
  destroyAddWindow()
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

function onAddVip(id, name, state)
  local vipList = vipWindow:getChildById('contentsPanel')

  local label = g_ui.createWidget('VipListLabel')
  label.onMousePress = onVipListLabelMousePress
  label:setId('vip' .. id)
  label:setText(name)

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
    if state == VipState.Online and child.vipState ~= VipState.Online then
      vipList:insertChild(i, label)
      return
    end

    if (state ~= VipState.Online and child.vipState ~= VipState.Online)
      or (state == VipState.Online and child.vipState == VipState.Online) then
      
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
        end
      end
    end
  end

  vipList:insertChild(childrenCount+1, label)
end

function onVipStateChange(id, state)
  local vipList = vipWindow:getChildById('contentsPanel')
  local label = vipList:getChildById('vip' .. id)
  local text = label:getText()
  label:destroy()

  onAddVip(id, text, state)
end

function onVipListMousePress(widget, mousePos, mouseButton)
  if mouseButton ~= MouseRightButton then return end

  local vipList = vipWindow:getChildById('contentsPanel')

  local menu = g_ui.createWidget('PopupMenu')
  menu:addOption(tr('Add new VIP'), function() createAddWindow() end)

  if not isHiddingOffline() then
    menu:addOption(tr('Hide Offline'), function() hideOffline(true) end)
  else
    menu:addOption(tr('Show Offline'), function() hideOffline(false) end)
  end

  menu:display(mousePos)

  return true
end

function onVipListLabelMousePress(widget, mousePos, mouseButton)
  if mouseButton ~= MouseRightButton then return end

  local vipList = vipWindow:getChildById('contentsPanel')

  local menu = g_ui.createWidget('PopupMenu')
  menu:addOption(tr('Send Message'), function() g_game.openPrivateChannel(widget:getText()) end)
  menu:addOption(tr('Add new VIP'), function() createAddWindow() end)
  menu:addOption(tr('Remove %s', widget:getText()), function() if widget then g_game.removeVip(widget:getId():sub(4)) vipList:removeChild(widget) end end)
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
  menu:display(mousePos)

  return true
end
