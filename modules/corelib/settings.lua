-- @docclass
g_settings = {
  impl = g_configs.getSettings()
}

function g_settings.save()
  g_settings.impl:save()
end

function g_settings.setList(key, list)
  g_settings.impl:setList(key, list)
end

function g_settings.getList(key)
  g_settings.impl:getList(key)
end

function g_settings.exists(key)
  g_settings.impl:exists(key)
end

function g_settings.remove(key)
  g_settings.impl:remove(key)
end

function g_settings.setNode(key, node)
  g_settings.impl:setNode(key, node)
end

function g_settings.getNode(key)
  g_settings.impl:getNode(key)
end

function g_settings.mergeNode(key, node)
  g_settings.impl:mergeNode(key, node)
end

-- convenience methods

function g_settings.set(key, value)
  g_settings.impl:set(key, value)
end

function g_settings.setDefault(key, value)
  return g_settings.impl:setDefault(key, value)
end

function g_settings.get(key, default)
  return g_settings.impl:get(key, default)
end

function g_settings.getString(key, default)
  return g_settings.impl:getString(key, default)
end

function g_settings.getInteger(key, default)
  return g_settings.impl:getInteger(key, default)
end

function g_settings.getNumber(key, default)
  return g_settings.impl:getNumber(key, default)
end

function g_settings.getBoolean(key, default)
  return g_settings.impl:getBoolean(key, default)
end

function g_settings.getPoint(key, default)
  return g_settings.impl:getPoint(key, default)
end

function g_settings.getRect(key, default)
  return g_settings.impl:getRect(key, default)
end

function g_settings.getSize(key, default)
  return g_settings.impl:getSize(key, default)
end

function g_settings.getColor(key, default)
  return g_settings.impl:getColor(key, default)
end
