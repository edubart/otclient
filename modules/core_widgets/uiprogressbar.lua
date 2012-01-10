UIProgressBar = extends(UIWidget)

function UIProgressBar.create()
  local progressbar = UIProgressBar.internalCreate()
  progressbar:setFocusable(false)
  progressbar:setPhantom(true)
  progressbar.percent = 100
  return progressbar
end

function UIProgressBar:setPercent(percent)
  self:setBackgroundHeight(self:getHeight())
  self:setBackgroundWidth((percent * self:getWidth())/100)
  self.percent = percent
end

function UIProgressBar:getPercent()
  return self.percent
end
