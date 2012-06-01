function string.split(s, delim)
  local start = 1
  local results = {}
  while true do
    local pos = string.find(s, delim, start, true)
    if not pos then
      break
    end
    table.insert(results, string.sub(s, start, pos-1))
    start = pos + string.len(delim)
  end
  table.insert(results, string.sub(s, start))
  table.removevalue(results, '')
  return results
end

function string.starts(s, start)
  return string.sub(s, 1, #start) == start
end

function string.trim(s)
  return string.match(s, '^%s*(.*%S)') or ''
end

function string.explode(str, sep, limit)
  if(type(sep) ~= 'string' or tostring(str):len() == 0 or sep:len() == 0) then
    return {}
  end

  local i, pos, tmp, t = 0, 1, "", {}
  for s, e in function() return string.find(str, sep, pos) end do
    tmp = str:sub(pos, s - 1):trim()
    table.insert(t, tmp)
    pos = e + 1

    i = i + 1
    if(limit ~= nil and i == limit) then
      break
    end
  end

  tmp = str:sub(pos):trim()
  table.insert(t, tmp)
  return t
end
