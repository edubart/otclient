#include <global.h>
#include <core/resources.h>
#include <graphics/fonts.h>


Fonts g_fonts;

void Fonts::init()
{
    g_resources.pushCurrentPath("fonts");

    // load all fonts
    std::list<std::string> files = g_resources.listDirectoryFiles();
    foreach(const std::string& file, files) {
        if(boost::ends_with(file, ".otml")) {
            std::string name = file;
            boost::erase_first(name, ".otml");
            FontPtr font(new Font(name));
            if(font->load(file))
                m_fonts.push_back(font);
        }
    }

    g_resources.popCurrentPath();
}

FontPtr Fonts::get(const std::string& fontName)
{
    // find font by name
    foreach(const FontPtr& font, m_fonts) {
        if(font->getName() == fontName)
            return font;
    }

    logFatal("ERROR: Font '",fontName,"' not found");
    return FontPtr();
}

