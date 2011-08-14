#ifndef BORDERIMAGE_H
#define BORDERIMAGE_H

#include "image.h"

class BorderImage : public Image
{
public:
    BorderImage(TexturePtr texture,
                  const Rect& left,
                  const Rect& right,
                  const Rect& top,
                  const Rect& bottom,
                  const Rect& topLeft,
                  const Rect& topRight,
                  const Rect& bottomLeft,
                  const Rect& bottomRight,
                  const Rect& center);

    static BorderImagePtr loadFromOTML(const OTMLNodePtr& borderImageNode);

    void draw(const Rect& screenCoords);

    Size getDefaultSize() const { return m_defaultSize; }

private:
    Rect m_leftBorderTexCoords;
    Rect m_rightBorderTexCoords;
    Rect m_topBorderTexCoords;
    Rect m_bottomBorderTexCoords;

    Rect m_topLeftCornerTexCoords;
    Rect m_topRightCornerTexCoords;
    Rect m_bottomLeftCornerTexCoords;
    Rect m_bottomRightCornerTexCoords;

    Rect m_centerTexCoords;

    Size m_bordersSize;
    Size m_defaultSize;
};

#endif
