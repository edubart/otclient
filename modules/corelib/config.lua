-- @docclass

local function convertSettingValue(value)
  if type(value) == 'table' then
    if value.x and value.width then
      return recttostring(value)
    elseif value.x then
      return pointtostring(value)
    elseif value.width then
      return sizetostring(value)
    elseif value.r then
      return colortostring(value)
    else
      return value
    end
  elseif value == nil then
    return ''
  else
    return tostring(value)
  end
end

function Config:set(key, value)
  self:setValue(key, convertSettingValue(value))
end

function Config:setDefault(key, value)
  if self:exists(key) then return false end
  self:set(key, value)
  return true
end

function Config:get(key, default)
  if not self:exists(key) and default ~= nil then
    self:set(key, default)
  end
  return self:getValue(key)
end

function Config:getString(key, default)
  return self:get(key, default)
end

function Config:getInteger(key, default)
  local v = tonumber(self:get(key, default)) or 0
  return v
end

function Config:getNumber(key, default)
  local v = tonumber(self:get(key, default)) or 0
  return v
end

function Config:getBoolean(key, default)
  return toboolean(self:get(key, default))
end

function Config:getPoint(key, default)
  return topoint(self:get(key, default))
end

function Config:getRect(key, default)
  return torect(self:get(key, default))
end

function Config:getSize(key, default)
  return tosize(self:get(key, default))
end

function Config:getColor(key, default)
  return tocolor(self:get(key, default))
end

