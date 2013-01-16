-- @docclass
UIProgressBar = extends(UIWidget)

function UIProgressBar.create()
  local progressbar = UIProgressBar.internalCreate()
  progressbar:setFocusable(false)
  progressbar:setPhantom(true)
  progressbar.percent = 0
  progressbar.bgBorderLeft = 0
  progressbar.bgBorderRight = 0
  progressbar.bgBorderTop = 0
  progressbar.bgBorderBottom = 0
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
  local width = math.round(math.max((self.percent * (self:getWidth() - self.bgBorderLeft - self.bgBorderRight))/100, 1))
  local height = self:getHeight() - self.bgBorderTop - self.bgBorderBottom
  local rect = { x = self.bgBorderLeft, y = self.bgBorderTop, width = width, height = height }
  self:setBackgroundRect(rect)
end

function UIProgressBar:onStyleApply(name, node)
  for name,value in pairs(node) do
    if name == 'background-border-left' then
      self.bgBorderLeft = tonumber(value)
    elseif name == 'background-border-right' then
      self.bgBorderRight = tonumber(value)
    elseif name == 'background-border-top' then
      self.bgBorderTop = tonumber(value)
    elseif name == 'background-border-bottom' then
      self.bgBorderBottom = tonumber(value)
    elseif name == 'background-border' then
      self.bgBorderLeft = tonumber(value)
      self.bgBorderRight = tonumber(value)
      self.bgBorderTop = tonumber(value)
      self.bgBorderBottom = tonumber(value)
    end
  end
end

function UIProgressBar:onGeometryChange(oldRect, newRect)
  self:updateBackground()
end
