Mouse = {}

function Mouse.setTargetCursor()
  g_window.setMouseCursor('/core_styles/cursors/targetcursor.png', {x=9,y=9})
end

function Mouse.setHorizontalCursor()
  g_window.setMouseCursor('/core_styles/cursors/horizontal.png', {x=9,y=4})
end

function Mouse.setVerticalCursor()
  g_window.setMouseCursor('/core_styles/cursors/vertical.png', {x=4,y=9})
end

function Mouse.restoreCursor()
  g_window.restoreMouseCursor()
end
