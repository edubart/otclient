-- @docclass table

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

function table.recursivecopy(t)
  local res = {}
  for k,v in pairs(t) do
    if type(v) == "table" then
      res[k] = table.recursivecopy(v)
    else
      res[k] = v
    end
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

function table.find(t, value, lowercase)
  for k,v in pairs(t) do
    if lowercase and type(value) == 'string' and type(v) == 'string' then 
      if v:lower() == value:lower() then return k end
    end
    if v == value then return k end
  end
end

function table.contains(t, value)
  return table.find(t, value) ~= nil
end

function table.findKey(t, key)
  if t and type(t) == 'table' then
    for k,v in pairs(t) do
      if k == key then return k end
    end
  end
end

function table.hasKey(t, key)
  return table.findKey(t, key) ~= nil
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

function table.empty(t)
  if t and type(t) == 'table' then
    return next(t) == nil
  end
  return true
end

function table.permute(t, n, count)
  n = n or #t
  for i=1,count or n do
    local j = math.random(i, n)
    t[i], t[j] = t[j], t[i]
  end
  return t
end

function table.findbyfield(t, fieldname, fieldvalue)
  for _i,subt in pairs(t) do
    if subt[fieldname] == fieldvalue then
      return subt
    end
  end
  return nil
end

function table.toString(t)
  local maxn = #t
  local str = ""
  for k,v in pairs(t) do
    if k == maxn and k ~= 1 then
      str = str .. " and " .. v
    elseif maxn > 1 and k ~= 1 then
      str = str .. ", " .. v
    else
      str = str .. " " .. v
    end
  end
  return str
end
