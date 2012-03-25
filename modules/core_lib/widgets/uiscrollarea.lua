UIScrollArea = extends(UIWidget)

function UIScrollArea.create()
  local scrollarea = UIScrollArea.internalCreate()
  scrollarea:setClipping(true)
  return scrollarea
end

function UIScrollArea:onStyleApply(styleName, styleNode)
  for name,value in pairs(styleNode) do
    if name == 'horizontal-scrollbar' then
      
    end
  end
end
