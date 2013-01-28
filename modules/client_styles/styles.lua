function init()
  local files
  files = g_resources.listDirectoryFiles('/styles')
  for _,file in pairs(files) do
    if g_resources.isFileType(file, 'otui') then
      g_ui.importStyle('/styles/' .. file)
    end
  end

  files = g_resources.listDirectoryFiles('/fonts')
  for _,file in pairs(files) do
    if g_resources.isFileType(file, 'otfont') then
      g_fonts.importFont('/fonts/' .. file)
    end
  end

  files = g_resources.listDirectoryFiles('/particles')
  for _,file in pairs(files) do
    if g_resources.isFileType(file, 'otps')then
      g_particles.importParticle('/particles/' .. file)
    end
  end

  g_mouse.loadCursors('/cursors/cursors')
end

function terminate()
end

