function init()
  if not g_things.loadDat('/game_tibiafiles/Tibia.dat') then
    fatal(tr("Unable to load dat file, please place a valid Tibia dat in modules/game_tibiafiles/Tibia.dat"))
  end
  if not g_sprites.loadSpr('/game_tibiafiles/Tibia.spr') then
    fatal(tr("Unable to load spr file, please place a valid Tibia spr in modules/game_tibiafiles/Tibia.spr"))
  end
end

function terminate()
end
