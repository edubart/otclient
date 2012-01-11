UIProgressBar = extends(UIWidget)

function UIProgressBar.create()
  local progressbar = UIProgressBar.internalCreate()
  progressbar:setFocusable(false)
  progressbar:setPhantom(true)
  progressbar.percent = 0
  progressbar:setBackgroundSize({width = 1, height = 1})
  return progressbar
end

function UIProgressBar:setPercent(percent)
  self:setBackgroundHeight(self:getHeight())
  local width = (percent * self:getWidth())/100
  if width == 0 then width = 1 end
  self:setBackgroundWidth(width)
  self.percent = percent
end

function UIProgressBar:getPercent()
  return self.percent
end
