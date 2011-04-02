#ifndef FONTS_H
#define FONTS_H

#include "prerequisites.h"
#include "rect.h"

struct Font
{

    Rect textureArea[256];
};

class Fonts
{
public:
    Fonts();
    ~Fonts();

    bool load();

private:
    std::map<int, Font*> mFonts;
};

extern Fonts g_fonts;

#endif // FONTS_H
