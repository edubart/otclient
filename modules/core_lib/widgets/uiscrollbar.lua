UIScrollBar = extends(UIWidget)

function UIScrollBar.create()
  local scrollbar = UIScrollBar.internalCreate()
  scrollbar:setFocusable(false)
  return scrollbar
end

function UIScrollBar:onSetup()
  --self.getChildById('upButton').onClick = function() self.navigateUp() end
  --self.getChildById('upButton').onClick = function() self.navigateDown() end
end

function UIScrollBar:attachWidget(widget)
  self.attachedWidget = widget
end

function UIScrollBar:onStyleApply(styleName, styleNode)
  if styleNode['attached-to'] then
    local id = styleNode['attached-to']
    local parent = self:getParent()
    local widget
    if id == 'parent' then
      widget = parent
    elseif parent then
      widget = parent:getChildById(id)
    end
    self:attachWidget(widget)
  end
end
