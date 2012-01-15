UIWindow = extends(UIWidget)

function UIWindow.create()
  local window = UIWindow.internalCreate()
  window:setTextAlign(AlignTopCenter)
  return window
end

function UIWindow:onKeyPress(keyCode, keyboardModifiers, wouldFilter)
  if wouldFilter then return end
  if keyboardModifiers == KeyboardNoModifier then
    if keyCode == KeyReturn or keyCode == KeyEnter then
      signalcall(self.onEnter, self)
    elseif keyCode == KeyEscape then
      signalcall(self.onEscape, self)
    end
  end
end
