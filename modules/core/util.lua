function print(...)
  local msg = ""
  for i,v in ipairs(arg) do
    msg = msg .. tostring(v) .. "\t"
  end
  Logger.log(LogInfo, msg)
end

function createEnvironment()
  local env = { }
  setmetatable(env, { __index = _G} )
  return env
end

function connect(object, signalsAndSlots)
  for signal,slot in pairs(signalsAndSlots) do
    if not object[signal] then
        object[signal] = slot
    elseif type(object[signal]) == 'function' then
        object[signal] = { object[signal], slot }
    elseif type(object[signal]) == 'table' then
        table.insert(object[signal], #object[signal]+1, slot)
    end
  end
end

function dumpWidgets()
  for i=1,UI.root:getChildCount() do
    print(UI.root:getChildByIndex(i):getId())
  end
end

function getCallingScriptSourcePath(depth)
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

function resolveFileFullPath(filePath, depth)
  depth = depth or 1
  if filePath:sub(0, 1) ~= '/' then
    return getCallingScriptSourcePath(depth+1) .. '/' .. filePath
  else
    return filePath
  end
end
