UIWindow = extends(UIWidget)

function UIWindow.create()
  local window = UIWindow.internalCreate()
  window:setTextAlign(AlignTopCenter)
  return window
end

function UIWindow:onKeyPress(keyCode, keyboardModifiers)
  if keyboardModifiers == KeyboardNoModifier then
    if keyCode == KeyEnter then
      signalcall(self.onEnter, self)
    elseif keyCode == KeyEscape then
      signalcall(self.onEscape, self)
    end
  end
end

function UIWindow:onMousePress(mousePos, mouseButton)

end

function UIWindow:onGeometryChange(oldRect, newRect)

end
