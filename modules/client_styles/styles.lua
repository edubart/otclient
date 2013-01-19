function init()
  local styles = g_resources.listDirectoryFiles('/styles')
  for _i,style in pairs(styles) do
    if string.ends(style, '.otui') then
        g_ui.importStyle('/styles/' .. style)
    end
  end

  local fonts = g_resources.listDirectoryFiles('/fonts')
  for _i,font in pairs(fonts) do
    if string.ends(font, '.otfont') then
      g_fonts.importFont('/fonts/' .. font)
    end
  end

  local particles = g_resources.listDirectoryFiles('/particles')
  for _i,particle in pairs(particles) do
    if string.ends(particle, '.otps') then
      g_particles.importParticle('/particles/' .. particle)
    end
  end
end

function terminate()
end

