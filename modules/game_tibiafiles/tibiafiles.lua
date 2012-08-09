filename = 'Tibia'

function init()
  connect(g_game, { onClientVersionChange = load })
end

function terminate()
  disconnect(g_game, { onClientVersionChange = load })
end

function setFileName(name)
  filename = name
end

function load()
  local version = g_game.getClientVersion()
  local datPath = resolvepath(version .. '/' .. filename .. '.dat')
  local sprPath = resolvepath(version .. '/' .. filename .. '.spr')

  local errorMessage = ''
  if not g_things.loadDat(datPath) then
    errorMessage = errorMessage .. tr("Unable to load dat file, please place a valid dat in '%s'", datPath) .. '\n'
  end
  if not g_sprites.loadSpr(sprPath) then
    errorMessage = errorMessage .. tr("Unable to load spr file, please place a valid spr in '%s'", sprPath) .. '\n'
  end

  if errorMessage:len() > 0 then
    displayErrorBox(tr('Error'), errorMessage)
  end
end
