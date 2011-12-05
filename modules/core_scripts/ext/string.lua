function string:split(sep)
  local t = { }
  local function helper(word)
    table.insert(t, word)
    return ""
  end
  if not self:gsub("%w+", helper):find("%S") then
    return t
  end
end

function string:starts(start)
  return self:sub(1, #start) == start
end

function string.trim(str)
  return str:match'^%s*(.*%S)' or ''
end

function toboolean(str)
  str = string.trim(str)
  if str == '1' or str == 'true' then
    return true
  end
  return false
end
