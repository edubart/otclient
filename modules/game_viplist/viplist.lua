VipList = {}

-- private variables
local vipWindow
local vipButton
local addVipWindow

-- public functions
function VipList.init()
  connect(g_game, { onGameEnd = VipList.clear,
                    onAddVip = VipList.onAddVip,
                    onVipStateChange = VipList.onVipStateChange })


  g_keyboard.bindKeyDown('Ctrl+P', VipList.toggle)

  vipWindow = g_ui.loadUI('viplist.otui', GameInterface.getRightPanel())
  vipButton = TopMenu.addRightGameToggleButton('vipListButton', tr('VIP list') .. ' (Ctrl+P)', 'viplist.png', VipList.toggle)
  vipButton:setOn(true)

  VipList.refresh()
end

function VipList.terminate()
  g_keyboard.unbindKeyDown('Ctrl+P')
  disconnect(g_game, { onGameEnd = VipList.clear,
                       onAddVip = VipList.onAddVip,
                       onVipStateChange = VipList.onVipStateChange })

  vipWindow:destroy()
  vipWindow = nil
  vipButton:destroy()
  vipButton = nil

  VipList = nil
end

function VipList.refresh()
  VipList.clear()
  for id,vip in pairs(g_game.getVips()) do
    VipList.onAddVip(id, unpack(vip))
  end
end

function VipList.clear()
  local vipList = vipWindow:getChildById('contentsPanel')
  vipList:destroyChildren()
end

function VipList.toggle()
  if vipButton:isOn() then
    vipWindow:close()
    vipButton:setOn(false)
  else
    vipWindow:open()
    vipButton:setOn(true)
  end
end

function VipList.onMiniWindowClose()
  vipButton:setOn(false)
end

function VipList.createAddWindow()
  addVipWindow = g_ui.displayUI('addvip.otui')
end

function VipList.destroyAddWindow()
  addVipWindow:destroy()
  addVipWindow = nil
end

function VipList.addVip()
  g_game.addVip(addVipWindow:getChildById('name'):getText())
  VipList.destroyAddWindow()
end

-- hooked events
function VipList.onAddVip(id, name, online)
  local vipList = vipWindow:getChildById('contentsPanel')

  local label = g_ui.createWidget('VipListLabel')
  label:setId('vip' .. id)
  label:setText(name)

  if online then
    label:setColor('#00ff00')
  else
    label:setColor('#ff0000')
  end

  label.vipOnline = online

  label:setPhantom(false)
  connect(label, { onDoubleClick = function () g_game.openPrivateChannel(label:getText()) return true end } )

  local nameLower = name:lower()
  local childrenCount = vipList:getChildCount()

  for i=1,childrenCount do
    local child = vipList:getChildByIndex(i)
    if online and not child.vipOnline then
      vipList:insertChild(i, label)
      return
    end

    if (not online and not child.vipOnline) or (online and child.vipOnline) then
      local childText = child:getText():lower()
      local length = math.min(childText:len(), nameLower:len())

      for j=1,length do
        if nameLower:byte(j) < childText:byte(j) then
          vipList:insertChild(i, label)
          return
        elseif nameLower:byte(j) > childText:byte(j) then
          break
        end
      end
    end
  end

  vipList:insertChild(childrenCount+1, label)
end

function VipList.onVipStateChange(id, online)
  local vipList = vipWindow:getChildById('contentsPanel')
  local label = vipList:getChildById('vip' .. id)
  local text = label:getText()
  label:destroy()

  VipList.onAddVip(id, text, online)
end

function VipList.onVipListMousePress(widget, mousePos, mouseButton)
  if mouseButton ~= MouseRightButton then return end

  local vipList = vipWindow:getChildById('contentsPanel')

  local menu = g_ui.createWidget('PopupMenu')
  menu:addOption(tr('Add new VIP'), function() VipList.createAddWindow() end)
  menu:display(mousePos)

  return true
end

function VipList.onVipListLabelMousePress(widget, mousePos, mouseButton)
  if mouseButton ~= MouseRightButton then return end

  local vipList = vipWindow:getChildById('contentsPanel')

  local menu = g_ui.createWidget('PopupMenu')
  menu:addOption(tr('Add new VIP'), function() VipList.createAddWindow() end)
  menu:addOption(tr('Remove %s', widget:getText()), function() if widget then g_game.removeVip(widget:getId():sub(4)) vipList:removeChild(widget) end end)
  menu:addSeparator()
  menu:addOption(tr('Copy Name'), function() g_window.setClipboardText(widget:getText()) end)
  menu:display(mousePos)

  return true
end
