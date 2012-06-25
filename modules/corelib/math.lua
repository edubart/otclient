-- @docclass math

function math.round(num, idp)
  local mult = 10^(idp or 0)
  if num >= 0 then
      return math.floor(num * mult + 0.5) / mult
  else
      return math.ceil(num * mult - 0.5) / mult
  end
end
