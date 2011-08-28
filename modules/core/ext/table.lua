function table.dump(t, depth)
  if not depth then depth = 0 end
  for k,v in pairs(t) do
    str = string.rep(' ', depth * 2) .. k .. ': '
    if type(v) ~= "table" then
      print(str .. v)
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

function table.selective_copy(t, keys)
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
