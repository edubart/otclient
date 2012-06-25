-- @docclass
UIProgressBar = extends(UIWidget)

function UIProgressBar.create()
  local progressbar = UIProgressBar.internalCreate()
  progressbar:setFocusable(false)
  progressbar:setPhantom(true)
  progressbar.percent = 0
  progressbar:updateBackground()
  return progressbar
end

function UIProgressBar:setPercent(percent)
  self.percent = math.max(math.min(percent, 100), 0)
  self:updateBackground()
end


function UIProgressBar:getPercent()
  return self.percent
end

function UIProgressBar:getPercentPixels()
  return 100 / self:getWidth()
end

function UIProgressBar:updateBackground()
  local width = math.round(math.max((self.percent * self:getWidth())/100, 1))
  local height = self:getHeight()
  self:setBackgroundSize({width=width, height=height})
end


function UIProgressBar:onGeometryChange(oldRect, newRect)
  self:updateBackground()
end
