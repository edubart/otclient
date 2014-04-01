local config = g_configs.getSettings()

g_settings = makesingleton(config)

function g_settings.set(key, value)
  config:set(key, value)
end

function g_settings.setDefault(key, value)
  return config:setDefault(key, value)
end

function g_settings.get(key, default)
  return config:get(key, default)
end

function g_settings.getString(key, default)
  return config:getString(key, default)
end

function g_settings.getInteger(key, default)
  return config:getInteger(key, default)
end

function g_settings.getNumber(key, default)
  return config:getNumber(key, default)
end

function g_settings.getBoolean(key, default)
  return config:getBoolean(key, default)
end

function g_settings.getPoint(key, default)
  return config:getPoint(key, default)
end

function g_settings.getRect(key, default)
  return config:getRect(key, default)
end

function g_settings.getSize(key, default)
  return config:getSize(key, default)
end

function g_settings.getColor(key, default)
  return config:getColor(key, default)
end