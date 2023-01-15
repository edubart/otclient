touchHotkeysWindow = nil
touchHotkeysButton = nil
addTouchHotkeysWindow = nil
windowType = nil

function init()
  windowType = g_settings.getBoolean('touchHotkeys_isHorizontal')
  if windowType == nil then
    windowType = false
  end

  touchHotkeysButton = modules.client_topmenu.addRightGameToggleButton('touchHotkeysButton', tr('Touch Hotkeys'), '/touch_hotkeys/img_touch_hotkeys/Hotkey_icon', toggle)
  touchHotkeysButton:setOn(true)

  if windowType == false then
    touchHotkeysWindow = g_ui.loadUI('touch_hotkeys', modules.game_interface.getRightPanel())
    touchHotkeysWindow:setup()
    touchHotkeysWindow:setContentMinimumHeight(380)
    touchHotkeysWindow:setContentMaximumHeight(380)
    --touchHotkeysWindow:setHeight()
    windowType = true
  else
    touchHotkeysWindow = g_ui.loadUI('touch_hotkeys_horizontal', modules.game_interface.getRightPanel())
    touchHotkeysWindow:setup()
    touchHotkeysWindow:setContentMinimumHeight(100)
    touchHotkeysWindow:setContentMaximumHeight(100)
    --touchHotkeysWindow:setHeight()
    windowType = false
  end

  --[[touchHotkeysWindow = g_ui.loadUI('touch_hotkeys', modules.game_interface.getRightPanel())
  
  touchHotkeysWindow:setup()
  touchHotkeysWindow:setContentMinimumHeight(380)
  touchHotkeysWindow:setContentMaximumHeight(380)]]
end

function terminate()
  disconnect(g_game, { onGameEnd = clear })

  touchHotkeysWindow:destroy()
  touchHotkeysButton:destroy()

end

function clear()
  local touchHotkeysList = touchHotkeysWindow:getChildById('contentsPanel')
  touchHotkeysList:destroyChildren()
end

function toggle()
  if touchHotkeysButton:isOn() then
    touchHotkeysWindow:close()
    touchHotkeysButton:setOn(false)
  else
    touchHotkeysWindow:open()
    touchHotkeysButton:setOn(true)
  end
end

function onMiniWindowClose()
  touchHotkeysButton:setOn(false)
end

function changeVerticalToHorizontal()
  if windowType then
    clear()
    touchHotkeysWindow:destroy()
    touchHotkeysWindow = g_ui.loadUI('touch_hotkeys_horizontal', modules.game_interface.getRightPanel())
    touchHotkeysWindow:setup()
    windowType = false
    touchHotkeysWindow:setContentMinimumHeight(65)
    touchHotkeysWindow:setContentMaximumHeight(65)
    touchHotkeysWindow:setHeight(90)
    g_settings.set('touchHotkeys_isHorizontal', true)
  else
    clear()
    touchHotkeysWindow:destroy()
    touchHotkeysWindow = g_ui.loadUI('touch_hotkeys', modules.game_interface.getRightPanel())
    touchHotkeysWindow:setup()
    windowType = true
    touchHotkeysWindow:setContentMinimumHeight(380)
    touchHotkeysWindow:setContentMaximumHeight(380)
    touchHotkeysWindow:setHeight(407)
    g_settings.set('touchHotkeys_isHorizontal', false)
  end
end