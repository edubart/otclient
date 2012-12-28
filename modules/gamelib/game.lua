local currentRsa
local enableCreatureNameFormat = true

function g_game.getRsa()
  return currentRsa
end

function g_game.isCreatureNameFormatEnabled()
  return enableCreatureNameFormat
end

function g_game.chooseRsa(host)
  if string.ends(host, '.tibia.com') or string.ends(host, '.cipsoft.com') then
    g_game.setRsa(CIPSOFT_RSA)
  else
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

function g_game.getOsType()
  if g_game.isOfficialTibia() then
    if g_app.getOs() == 'windows' then
      return OsTypes.Windows
    else
      return OsTypes.Linux
    end
  else
    if g_app.getOs() == 'windows' then
      return OsTypes.OtclientWindows
    elseif g_app.getOs() == 'mac' then
      return OsTypes.OtclientMac
    else
      return OsTypes.OtclientLinux
    end
  end
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
