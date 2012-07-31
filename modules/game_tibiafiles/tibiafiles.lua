function init()
  connect(g_game, { onClientVersionChange = load })
end

function terminate()
  disconnect(g_game, { onClientVersionChange = load })
end

function load()
  local version = g_game.getClientVersion()
  local datPath = resolvepath(version .. '/Tibia.dat')
  local sprPath = resolvepath(version .. '/Tibia.spr')
  if not g_things.loadDat(datPath) then
    fatal(tr("Unable to load dat file, please place a valid dat in '%s'", datPath))
  end
  if not g_sprites.loadSpr(sprPath) then
    fatal(tr("Unable to load spr file, please place a valid spr in '%s'", sprPath))
  end
end
