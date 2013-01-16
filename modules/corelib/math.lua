-- @docclass math

local U8  = 2^8
local U16 = 2^16
local U32 = 2^32
local U64 = 2^64

function math.round(num, idp)
  local mult = 10^(idp or 0)
  if num >= 0 then
    return math.floor(num * mult + 0.5) / mult
  else
    return math.ceil(num * mult - 0.5) / mult
  end
end

function math.isu8(num)
  return math.isinteger(num) and num >= 0 and num < U8
end

function math.isu16(num)
  return math.isinteger(num) and num >= U8 and num < U16
end

function math.isu32(num)
  return math.isinteger(num) and num >= U16 and num < U32
end

function math.isu64(num)
  return math.isinteger(num) and num >= U32 and num < U64
end

function math.isinteger(num)
  return ((type(num) == 'number') and (num == math.floor(num)))
end
