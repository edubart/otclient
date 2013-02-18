-- @docfuncs @{

function print(...)
  local msg = ""
  local args = {...}
  local appendSpace = #args > 1
  for i,v in ipairs(args) do
    msg = msg .. tostring(v)
    if appendSpace and i < #args then
      msg = msg .. '    '
    end
  end
  g_logger.log(LogInfo, msg)
end

function pinfo(msg)
  g_logger.log(LogInfo, msg)
end

function perror(msg)
  g_logger.log(LogError, msg)
end

function pwarning(msg)
  g_logger.log(LogWarning, msg)
end

function pdebug(msg)
  g_logger.log(LogDebug, msg)
end

function fatal(msg)
  g_logger.log(LogFatal, msg)
end

function exit()
  g_app.exit()
end

function quit()
  g_app.quit()
end

function connect(object, arg1, arg2, arg3)
  local signalsAndSlots
  local pushFront
  if type(arg1) == 'string' then
    signalsAndSlots = { [arg1] = arg2 }
    pushFront = arg3
  else
    signalsAndSlots = arg1
    pushFront = arg2
  end

  for signal,slot in pairs(signalsAndSlots) do
    if not object[signal] then
      local mt = getmetatable(object)
      if mt and type(object) == 'userdata' then
        object[signal] = function(...)
          return signalcall(mt[signal], ...)
        end
      end
    end

    if not object[signal] then
      object[signal] = slot
    elseif type(object[signal]) == 'function' then
      object[signal] = { object[signal] }
    end

    if type(slot) ~= 'function' then
      perror(debug.traceback('unable to connect a non function value'))
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

function disconnect(object, arg1, arg2)
  local signalsAndSlots
  if type(arg1) == 'string' then
    if arg2 == nil then
      object[arg1] = nil
      return
    end
    signalsAndSlots = { [arg1] = arg2 }
  elseif type(arg1) == 'table' then
    signalsAndSlots = arg1
  else
    perror(debug.traceback('unable to disconnect'))
  end

  for signal,slot in pairs(signalsAndSlots) do
    if not object[signal] then
    elseif type(object[signal]) == 'function' then
      if object[signal] == slot then
        object[signal] = nil
      end
    elseif type(object[signal]) == 'table' then
      for k,func in pairs(object[signal]) do
        if func == slot then
          table.remove(object[signal], k)

          if #object[signal] == 1 then
            object[signal] = object[signal][1]
          end
          break
        end
      end
    end
  end
end

function newclass()
  local class = {}
  function class.internalCreate()
    local instance = {}
    for k,v in pairs(class) do
      instance[k] = v
    end
    return instance
  end
  class.create = class.internalCreate
  return class
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

function runinsandbox(func, ...)
  if type(func) == 'string' then
    func, err = loadfile(resolvepath(func, 2))
    if not func then
      error(err)
    end
  end
  local env = { }
  local oldenv = getfenv(0)
  setmetatable(env, { __index = oldenv } )
  setfenv(0, env)
  func(...)
  setfenv(0, oldenv)
  return env
end

function loadasmodule(name, file)
  file = file or resolvepath(name, 2)
  if package.loaded[name] then
    return package.loaded[name]
  end
  local env = runinsandbox(file)
  package.loaded[name] = env
  return env
end

local function module_loader(modname)
  local module = g_modules.getModule(modname)
  if not module then
    return '\n\tno module \'' .. modname .. '\''
  end
  return function()
    if not module:load() then
      error('unable to load required module ' .. modname)
    end
    return module:getSandbox()
  end
end
table.insert(package.loaders, 1, module_loader)

function import(table)
  assert(type(table) == 'table')
  local env = getfenv(2)
  for k,v in pairs(table) do
    env[k] = v
  end
end

function export(what, key)
  if key ~= nil then
    _G[key] = what
  else
    for k,v in pairs(what) do
      _G[k] = v
    end
  end
end

function unexport(key)
  if type(key) == 'table' then
    for _k,v in pairs(key) do
      _G[v] = nil
    end
  else
    _G[key] = nil
  end
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
  if not filePath then return nil end
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

function toboolean(v)
  if type(v) == 'string' then
    v = v:trim():lower()
    if v == '1' or v == 'true' then
      return true
    end
  elseif type(v) == 'number' then
    if v == 1 then
      return true
    end
  elseif type(v) == 'boolean' then
    return v
  end
  return false
end

function fromboolean(boolean)
  if boolean then
    return 'true'
  else
    return 'false'
  end
end

function booleantonumber(boolean)
  if boolean then
    return 1
  else
    return 0
  end
end

function numbertoboolean(number)
  if number ~= 0 then
    return true
  else
    return false
  end
end

function signalcall(param, ...)
  if type(param) == 'function' then
    local status, ret = pcall(param, ...)
    if status then
      return ret
    else
      perror(ret)
    end
  elseif type(param) == 'table' then
    for k,v in pairs(param) do
      local status, ret = pcall(v, ...)
      if status then
        if ret then return true end
      else
        perror(ret)
      end
    end
  elseif func ~= nil then
    error('attempt to call a non function value')
  end
  return false
end

function tr(s, ...)
  return string.format(s, ...)
end

function getOppositeAnchor(anchor)
  if anchor == AnchorLeft then
    return AnchorRight
  elseif anchor == AnchorRight then
    return AnchorLeft
  elseif anchor == AnchorTop then
    return AnchorBottom
  elseif anchor == AnchorBottom then
    return AnchorTop
  elseif anchor == AnchorVerticalCenter then
    return AnchorHorizontalCenter
  elseif anchor == AnchorHorizontalCenter then
    return AnchorVerticalCenter
  end
  return anchor
end

-- @}