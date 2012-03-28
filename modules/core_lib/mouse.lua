Mouse = {}

local cursorChanged = false

function Mouse.setTargetCursor()
  g_window.setMouseCursor('/core_styles/cursors/targetcursor.png', {x=9,y=9})
  cursorChanged = true
end

function Mouse.setHorizontalCursor()
  g_window.setMouseCursor('/core_styles/cursors/horizontal.png', {x=9,y=4})
  cursorChanged = true
end

function Mouse.setVerticalCursor()
  g_window.setMouseCursor('/core_styles/cursors/vertical.png', {x=4,y=9})
  cursorChanged = true
end

function Mouse.restoreCursor()
  g_window.restoreMouseCursor()
  cursorChanged = false
end

function Mouse.isCursorChanged()
  return cursorChanged
end

function Mouse.isPressed()
  return g_ui.getPressedWidget() ~= nil
end

function Mouse.bindAutoPress(widget, callback)
  connect(widget, { onMousePress = function(widget, mousePos, mouseButton)
    callback()
    periodicalEvent(function()
      callback()
    end, function()
      return widget:isPressed()
    end, 30, 300)
    return true
  end })
end

function Mouse.bindPressMove(widget, callback)
  connect(widget, { onMouseMove = function(widget, mousePos, mouseMoved)
    if widget:isPressed() then
      callback(mousePos, mouseMoved)
    end
    return true
  end })
end
