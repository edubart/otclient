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