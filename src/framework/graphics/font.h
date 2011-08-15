#ifndef FONT_H
#define FONT_H

#include "declarations.h"

#include <framework/otml/declarations.h>

class Font
{
public:
    Font(const std::string& name) : m_name(name) { }

    /// Load font from otml node
    void load(const OTMLNodePtr& fontNode);

    /// Simple text render starting at startPos
    void renderText(const std::string& text,
                    const Point& startPos,
                    const Color& color = Color::white);

    /// Advanced text render delimited by a screen region and alignment
    void renderText(const std::string& text,
                    const Rect& screenCoords,
                    AlignmentFlag align = AlignTopLeft,
                    const Color& color = Color::white);

    /// Calculate glyphs positions to use on render, also calculates textBoxSize if wanted
    const std::vector<Point>& calculateGlyphsPositions(const std::string& text,
                                                       AlignmentFlag align = AlignTopLeft,
                                                       Size* textBoxSize = NULL) const;

    /// Simulate render and calculate text size
    Size calculateTextRectSize(const std::string& text);

    std::string getName() const { return m_name; }
    int getGlyphHeight() const { return m_glyphHeight; }
    const Rect* getGlyphsTextureCoords() const { return m_glyphsTextureCoords; }
    const Size* getGlyphsSize() const { return m_glyphsSize; }
    const TexturePtr& getTexture() const { return m_texture; }
    int getTopMargin() const { return m_topMargin; }
    Size getGlyphSpacing() const { return m_glyphSpacing; }

private:
    /// Calculates each font character by inspecting font bitmap
    void calculateGlyphsWidthsAutomatically(const Size& glyphSize);

    std::string m_name;
    int m_glyphHeight;
    int m_firstGlyph;
    int m_topMargin;
    Size m_glyphSpacing;
    TexturePtr m_texture;
    Rect m_glyphsTextureCoords[256];
    Size m_glyphsSize[256];
};


#endif

