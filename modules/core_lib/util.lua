function print(...)
  local msg = ""
  for i,v in ipairs(arg) do
    msg = msg .. tostring(v) .. "\t"
  end
  Logger.log(LogInfo, msg)
end

function fatal(msg)
  Logger.log(LogFatal, msg)
end

function connect(object, signalsAndSlots, pushFront)
  for signal,slot in pairs(signalsAndSlots) do
    if not object[signal] then
        object[signal] = slot
    elseif type(object[signal]) == 'function' then
        object[signal] = { object[signal], slot }
    elseif type(object[signal]) == 'table' then
        if pushFront then
          table.insert(object[signal], 1, slot)
        else
          table.insert(object[signal], #object[signal]+1, slot)
        end
    end
  end
end

function extends(base)
  local derived = {}
  function derived.internalCreate()
    local instance = base.create()
    for k,v in pairs(derived) do
      instance[k] = v
    end
    return instance
  end
  derived.create = derived.internalCreate
  return derived
end

function newenv()
  local env = { }
  setmetatable(env, { __index = _G} )
  return env
end

function getfsrcpath(depth)
  depth = depth or 2
  local info = debug.getinfo(1+depth, "Sn")
  local path
  if info.short_src then
    path = info.short_src:match("(.*)/.*")
  end
  if not path then
    path = '/'
  elseif path:sub(0, 1) ~= '/' then
    path = '/' .. path
  end
  return path
end

function resolvepath(filePath, depth)
  depth = depth or 1
  if filePath:sub(0, 1) ~= '/' then
    return getfsrcpath(depth+1) .. '/' .. filePath
  else
    return filePath
  end
end

function toboolean(str)
  str = str:trim():lower()
  if str == '1' or str == 'true' then
    return true
  end
  return false
end
