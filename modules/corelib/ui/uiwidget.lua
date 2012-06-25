-- @docclass UIWidget

function UIWidget:setMargin(...)
  local params = {...}
  if #params == 1 then
    self:setMarginTop(params[1])
    self:setMarginRight(params[1])
    self:setMarginBottom(params[1])
    self:setMarginLeft(params[1])
  elseif #params == 2 then
    self:setMarginTop(params[1])
    self:setMarginRight(params[2])
    self:setMarginBottom(params[1])
    self:setMarginLeft(params[2])
  elseif #params == 4 then
    self:setMarginTop(params[1])
    self:setMarginRight(params[2])
    self:setMarginBottom(params[3])
    self:setMarginLeft(params[4])
  end
end
