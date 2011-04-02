#include "fonts.h"
#include "resourcemanager.h"

Fonts g_fonts;

Fonts::Fonts()
{
}

Fonts::~Fonts()
{
}

bool Fonts::load()
{
    std::list<std::string> files = g_resources.getDirectoryFiles("fonts");
    foreach(const std::string& file, files) {
        notice("File: %s", file.c_str());
    }

    return true;
}
