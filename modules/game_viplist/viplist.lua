VipList = {}

-- private variables
local vipWindow = nil

-- public functions
function VipList.create()
  vipWindow = displayUI('viplist.otui', { parent = Game.gameRightPanel })
end

function VipList.destroy()
  vipWindow:destroy()
  vipWindow = nil
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

function VipList.onVipListLabelMousePress(widget, mousePos, mouseButton)
  if mouseButton ~= MouseRightButton then return end

  local vipList = vipWindow:getChildById('vipList')

  local menu = createWidget('PopupMenu')
  menu:addOption('Remove from VIP list', function() if widget then Game.removeVip(widget:getId():sub(4)) vipList:removeChild(widget) end end)
  menu:display(mousePos)
end


connect(Game, { onLogin = VipList.create,
                onLogout = VipList.destroy })
