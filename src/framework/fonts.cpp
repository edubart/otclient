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
    for(std::list<std::string>::iterator it = files.begin(), end = files.end(); it != end; ++it) {
        notice("File: %s", (*it).c_str());
    }

    return true;
}
