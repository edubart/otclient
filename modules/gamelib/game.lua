local currentRsa

function g_game.getRsa()
  return currentRsa
end

function g_game.chooseRsa(host)
  if currentRsa ~= CIPSOFT_RSA and currentRsa ~= OTSERV_RSA then return end
  if host:ends('.tibia.com') or host:ends('.cipsoft.com') then
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

function g_game.setRsa(rsa, e)
  e = e or '65537'
  g_crypt.rsaSetPublicKey(rsa, e)
  currentRsa = rsa
end

function g_game.isOfficialTibia()
  return currentRsa == CIPSOFT_RSA
end

function g_game.getSupportedClients()
  return {
    760, 810, 811, 840, 842, 850, 853, 
    854, 860, 861, 862, 870, 910, 940, 
    944, 953, 954, 960, 961, 963, 970,
    980, 981, 982, 983, 984, 985, 986, 
    1001, 1002, 1010
  }
end

function g_game.getProtocolVersionForClient(client)
  clients = {
    [980] = 971,
    [981] = 973,
    [982] = 974,
	  [983] = 975,
	  [984] = 976,
	  [985] = 977,
	  [986] = 978,
	  [1001] = 979,
	  [1002] = 980,
  }
  return clients[client] or client
end

g_game.setRsa(OTSERV_RSA)
