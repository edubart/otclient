#ifndef IMAGE_H
#define IMAGE_H

#include "graphicsdeclarations.h"
#include <otml/otmldeclarations.h>

class Image
{
public:
    Image(TexturePtr texture, Rect textureCoords = Rect());

    static ImagePtr loadFromOTML(const OTMLNodePtr& imageNode);

    virtual void draw(const Rect& screenCoords);

protected:
    TexturePtr m_texture;
    Rect m_textureCoords;
};

#endif
