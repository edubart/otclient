local currentRsa

function g_game.getRsa()
  return currentRsa
end

function g_game.chooseRsa(host)
  if string.ends(host, '.tibia.com') or string.ends(host, '.cipsoft.com') then
    g_game.setRsa(CIPSOFT_RSA)

    if g_app.getOs() == 'windows' then
      g_game.setCustomOs(OsTypes.Windows)
    else
      g_game.setCustomOs(OsTypes.Linux)
    end
  else
    if currentRsa == CIPSOFT_RSA then
      g_game.setCustomOs(-1)
    end
    g_game.setRsa(OTSERV_RSA)
  end
end

function g_game.setRsa(rsa)
  if currentRsa ~= rsa then
    currentRsa = rsa
    g_crypt.rsaSetPublicKey(currentRsa, '65537')
  end
end

function g_game.isOfficialTibia()
  return currentRsa == CIPSOFT_RSA
end

function g_game.getSupportedProtocols()
  return {
    810, 853, 854, 860, 861, 862, 870,
    910, 940, 944, 953, 954, 960, 961,
    963, 970, 971, 973
  }
end

function g_game.getSupportedClients(protocol)
  clients = {
    [971] = {980},
    [973] = {981}
  }
  return clients[protocol] or {protocol}
end

g_game.setRsa(OTSERV_RSA)
