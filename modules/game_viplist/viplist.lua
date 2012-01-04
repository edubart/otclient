VipList = {}

-- private variables
local vipWindow = nil
local addVipWindow = nil

-- public functions
function VipList.create()
  vipWindow = displayUI('viplist.otui', { parent = Game.gameRightPanel })
end

function VipList.destroy()
  vipWindow:destroy()
  vipWindow = nil
end

function VipList.createAddWindow()
  addVipWindow = displayUI('addvip.otui')
end

function VipList.destroyAddWindow()
  addVipWindow:destroy()
  addVipWindow = nil
end

function VipList.addVip()
  Game.addVip(addVipWindow:getChildById('name'):getText())
  VipList.destroyAddWindow()
end

-- hooked events
function Game.onAddVip(id, name, online)
  local vipList = vipWindow:getChildById('vipList')

  local label = createWidget('VipListLabel', vipList)
  label:setId('vip' .. id)
  label:setText(name)

  if online then
    label:setForegroundColor('#00ff00')
  else
    label:setForegroundColor('#ff0000')
  end

  label.vipOnline = online
end

function Game.onVipStateChange(id, online)
  local vipList = vipWindow:getChildById('vipList')
  local label = vipList:getChildById('vip' .. id)

  if online then
    label:setForegroundColor('#00ff00')
  else
    label:setForegroundColor('#ff0000')
  end

  label.vipOnline = online
end

function VipList.onVipListMousePress(widget, mousePos, mouseButton)
  if mouseButton ~= MouseRightButton then return end

  local vipList = vipWindow:getChildById('vipList')

  local menu = createWidget('PopupMenu')
  menu:addOption('Add new VIP', function() VipList.createAddWindow() end)
  menu:display(mousePos)
  
  return true
end

function VipList.onVipListLabelMousePress(widget, mousePos, mouseButton)
  if mouseButton ~= MouseRightButton then return end

  local vipList = vipWindow:getChildById('vipList')

  local menu = createWidget('PopupMenu')
  menu:addOption('Add new VIP', function() VipList.createAddWindow() end)
  menu:addOption('Remove ' .. widget:getText(), function() if widget then Game.removeVip(widget:getId():sub(4)) vipList:removeChild(widget) end end)
  menu:addSeparator()
  menu:addOption('Copy Name', function() end)
  menu:display(mousePos)
  
  return true
end


connect(Game, { onLogin = VipList.create,
                onLogout = VipList.destroy })
