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

  -- TODO load particles
end

function terminate()
end

