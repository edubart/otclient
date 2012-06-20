function table.dump(t, depth)
  if not depth then depth = 0 end
  for k,v in pairs(t) do
    str = (' '):rep(depth * 2) .. k .. ': '
    if type(v) ~= "table" then
      print(str .. tostring(v))
    else
      print(str)
      table.dump(v, depth+1)
    end
  end
end

function table.copy(t)
  local res = {}
  for k,v in pairs(t) do
    res[k] = v
  end
  return res
end

function table.selectivecopy(t, keys)
  local res = { }
  for i,v in ipairs(keys) do
    res[v] = t[v]
  end
  return res
end

function table.merge(t, src)
  for k,v in pairs(src) do
    t[k] = v
  end
end

function table.find(t, value)
  for k,v in pairs(t) do
    if v == value then return k end
  end
end

function table.removevalue(t, value)
  for k,v in pairs(t) do
    if v == value then
      table.remove(t, k)
      break
    end
  end
end

function table.compare(t, other)
  if #t ~= #other then return false end
  for k,v in pairs(t) do
    if v ~= other[k] then return false end
  end
  return true
end
