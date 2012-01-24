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

function setonclose(func)
  g_app.onClose = func
end

function exit()
  g_app.exit()
end

function connect(object, signalsAndSlots, pushFront)
  for signal,slot in pairs(signalsAndSlots) do
    if not object[signal] then
      object[signal] = slot
    elseif type(object[signal]) == 'function' then
      object[signal] = { object[signal] }
    end
    if type(object[signal]) == 'table' then
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
  if filePath then
    if filePath:sub(0, 1) ~= '/' then
      local basepath = getfsrcpath(depth+1)
      if basepath:sub(#basepath) ~= '/' then basepath = basepath .. '/' end
      return  basepath .. filePath
    else
      return filePath
    end
  else
    local basepath = getfsrcpath(depth+1)
    if basepath:sub(#basepath) ~= '/' then basepath = basepath .. '/' end
    return basepath
  end
end

function toboolean(str)
  str = str:trim():lower()
  if str == '1' or str == 'true' then
    return true
  end
  return false
end

local oldtonumber = tonumber
function tonumber(v)
  if v == nil then return 0 end
  return oldtonumber(v)
end

function signalcall(param, ...)
  if type(param) == 'function' then
    return param(...)
  elseif type(param) == 'table' then
    for k,v in pairs(param) do
      if param(...) then
        return true
      end
    end
  elseif func ~= nil then
    error('attempt to call a non function value')
  end
  return false
end

function runscript(file)
  g_lua.runScript(resolvepath(file, 2))
end