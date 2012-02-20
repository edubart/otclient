Mouse = {}

function Mouse.setTargetCursor()
  g_window.setMouseCursor('/core_styles/icons/targetcursor.png', {x=9,y=9})
end

function Mouse.restoreCursor()
  g_window.restoreMouseCursor()
end

