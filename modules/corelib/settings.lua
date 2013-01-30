-- @docclass
g_settings = {}

g_settings.exists = g_configs.exists
g_settings.setNode = g_configs.setNode
g_settings.mergeNode = g_configs.mergeNode
g_settings.getNode = g_configs.getNode
g_settings.remove = g_configs.remove
g_settings.setList = g_configs.setList
g_settings.getList = g_configs.getList
g_settings.save = g_configs.save

local function convertSettingValue(value)
  if type(value) == 'table' then
    if value.x and value.width then
      return recttostring(value)
    elseif value.x then
      return pointtostring(value)
    elseif value.width then
      return sizetostring(value)
    elseif value.r then
      return colortostring(value)
    else
      return value
    end
  elseif value == nil then
    return ''
  else
    return tostring(value)
  end
end

function g_settings.set(key, value)
  g_configs.set(key, convertSettingValue(value))
end

function g_settings.setDefault(key, value)
  if g_settings.exists(key) then return false end
  g_settings.set(key, value)
  return true
end

function g_settings.get(key, default)
  if not g_settings.exists(key) and default ~= nil then
    g_settings.set(key, default)
  end
  return g_configs.get(key)
end

function g_settings.getString(key, default)
  return g_settings.get(key, default)
end

function g_settings.getInteger(key, default)
  local v = tonumber(g_settings.get(key, default)) or 0
  return v
end

function g_settings.getNumber(key, default)
  local v = tonumber(g_settings.get(key, default)) or 0
  return v
end

function g_settings.getBoolean(key, default)
  return toboolean(g_settings.get(key, default))
end

function g_settings.getPoint(key, default)
  return topoint(g_settings.get(key, default))
end

function g_settings.getRect(key, default)
  return torect(g_settings.get(key, default))
end

function g_settings.getSize(key, default)
  return tosize(g_settings.get(key, default))
end

function g_settings.getColor(key, default)
  return tocolor(g_settings.get(key, default))
end

function g_settings.getColor(key, default)
  return tocolor(g_settings.get(key, default))
end
