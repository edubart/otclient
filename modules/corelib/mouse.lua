-- @docclass
g_mouse = {}

local cursorChanged = false

function g_mouse.setTargetCursor()
  g_window.setMouseCursor('/cursors/targetcursor.png', {x=9,y=9})
  cursorChanged = true
end

function g_mouse.setHorizontalCursor()
  g_window.setMouseCursor('/cursors/horizontal.png', {x=9,y=4})
  cursorChanged = true
end

function g_mouse.setVerticalCursor()
  g_window.setMouseCursor('/cursors/vertical.png', {x=4,y=9})
  cursorChanged = true
end

function g_mouse.restoreCursor()
  g_window.restoreMouseCursor()
  cursorChanged = false
end

function g_mouse.isCursorChanged()
  return cursorChanged
end

function g_mouse.isPressed(button)
  if not button then button = MouseLeftButton end
  return g_window.isMouseButtonPressed(button)
end

function g_mouse.bindAutoPress(widget, callback, delay)
  connect(widget, { onMousePress = function(widget, mousePos, mouseButton)
    local startTime = g_clock.millis()
    callback(widget, mousePos, mouseButton, 0)
    periodicalEvent(function()
      callback(widget, g_window.getMousePosition(), mouseButton, g_clock.millis() - startTime)
    end, function()
      return widget:isPressed()
    end, 30, delay)
    return true
  end })
end

function g_mouse.bindPressMove(widget, callback)
  connect(widget, { onMouseMove = function(widget, mousePos, mouseMoved)
    if widget:isPressed() then
      callback(mousePos, mouseMoved)
    end
    return true
  end })
end
