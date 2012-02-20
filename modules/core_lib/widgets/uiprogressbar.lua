UIProgressBar = extends(UIWidget)

function UIProgressBar.create()
  local progressbar = UIProgressBar.internalCreate()
  progressbar:setFocusable(false)
  progressbar:setPhantom(true)
  progressbar.m_percent = 0
  progressbar:updateBackground()
  return progressbar
end

function UIProgressBar:setPercent(percent)
  self.m_percent = math.max(math.min(percent, 100), 0)
  self:updateBackground()
end


function UIProgressBar:getPercent()
  return self.m_percent
end

function UIProgressBar:updateBackground()
  local width = math.max((self.m_percent * self:getWidth())/100, 1)
  local height = self:getHeight()
  self:setBackgroundSize({width=width, height=height})
end


function UIProgressBar:onGeometryChange(oldRect, newRect)
  self:updateBackground()
end
