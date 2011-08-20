#include "font.h"
#include "texturemanager.h"
#include "graphics.h"

#include <framework/otml/otml.h>

void Font::load(const OTMLNodePtr& fontNode)
{
    std::string textureName = fontNode->valueAt("texture");
    Size glyphSize = fontNode->valueAt<Size>("glyph size");
    m_glyphHeight = fontNode->valueAt<int>("height");
    m_topMargin = fontNode->valueAt("top margin", 0);
    m_firstGlyph = fontNode->valueAt("first glyph", 32);
    m_glyphSpacing = fontNode->valueAt("spacing", Size(0,0));

    // load font texture
    m_texture = g_textures.getTexture(textureName);
    if(!m_texture)
        throw std::runtime_error("failed to load texture for font");

    if(OTMLNodePtr node = fontNode->get("fixed glyph width")) {
        for(int glyph = m_firstGlyph; glyph < 256; ++glyph)
            m_glyphsSize[glyph] = Size(node->value<int>(), m_glyphHeight);
    } else
        calculateGlyphsWidthsAutomatically(glyphSize);

    // read custom widths
    if(OTMLNodePtr node = fontNode->get("glyph widths")) {
        for(const OTMLNodePtr& child : node->children())
            m_glyphsSize[fw::safe_cast<int>(child->tag())].setWidth(child->value<int>());
    }

    // calculate glyphs texture coords
    int numHorizontalGlyphs = m_texture->getSize().width() / glyphSize.width();
    for(int glyph = m_firstGlyph; glyph < 256; ++glyph) {
        m_glyphsTextureCoords[glyph].setRect(((glyph - m_firstGlyph) % numHorizontalGlyphs) * glyphSize.width(),
                                                ((glyph - m_firstGlyph) / numHorizontalGlyphs) * glyphSize.height(),
                                                m_glyphsSize[glyph].width(),
                                                m_glyphHeight);
    }
}

void Font::renderText(const std::string& text,
                      const Point& startPos,
                      const Color& color)
{
    Size boxSize = g_graphics.getScreenSize() - startPos.toSize();
    Rect screenCoords(startPos, boxSize);
    renderText(text, screenCoords, AlignTopLeft, color);
}


void Font::renderText(const std::string& text,
                      const Rect& screenCoords,
                      AlignmentFlag align,
                      const Color& color)
{
    // prevent glitches from invalid rects
    if(!screenCoords.isValid())
        return;

    int textLenght = text.length();

    // map glyphs positions
    Size textBoxSize;
    const std::vector<Point>& glyphsPositions = calculateGlyphsPositions(text, align, &textBoxSize);

    g_graphics.bindColor(color);
    g_graphics.bindTexture(m_texture);
    g_graphics.startDrawing();

    for(int i = 0; i < textLenght; ++i) {
        int glyph = (uchar)text[i];

        // skip invalid glyphs
        if(glyph < 32)
            continue;

        // calculate initial glyph rect and texture coords
        Rect glyphScreenCoords(glyphsPositions[i], m_glyphsSize[glyph]);
        Rect glyphTextureCoords = m_glyphsTextureCoords[glyph];

        // first translate to align position
        if(align & AlignBottom) {
            glyphScreenCoords.translate(0, screenCoords.height() - textBoxSize.height());
        } else if(align & AlignVerticalCenter) {
            glyphScreenCoords.translate(0, (screenCoords.height() - textBoxSize.height()) / 2);
        } else { // AlignTop
            // nothing to do
        }

        if(align & AlignRight) {
            glyphScreenCoords.translate(screenCoords.width() - textBoxSize.width(), 0);
        } else if(align & AlignHorizontalCenter) {
            glyphScreenCoords.translate((screenCoords.width() - textBoxSize.width()) / 2, 0);
        } else { // AlignLeft
            // nothing to do
        }

        // only render glyphs that are after 0, 0
        if(glyphScreenCoords.bottom() < 0 || glyphScreenCoords.right() < 0)
            continue;

        // bound glyph topLeft to 0,0 if needed
        if(glyphScreenCoords.top() < 0) {
            glyphTextureCoords.setTop(glyphTextureCoords.top() - glyphScreenCoords.top());
            glyphScreenCoords.setTop(0);
        }
        if(glyphScreenCoords.left() < 0) {
            glyphTextureCoords.setLeft(glyphTextureCoords.left() - glyphScreenCoords.left());
            glyphScreenCoords.setLeft(0);
        }

        // translate rect to screen coords
        glyphScreenCoords.translate(screenCoords.topLeft());

        // only render if glyph rect is visible on screenCoords
        if(!screenCoords.intersects(glyphScreenCoords))
            continue;

        // bound glyph bottomRight to screenCoords bottomRight
        if(glyphScreenCoords.bottom() > screenCoords.bottom()) {
            glyphTextureCoords.setBottom(glyphTextureCoords.bottom() + (screenCoords.bottom() - glyphScreenCoords.bottom()));
            glyphScreenCoords.setBottom(screenCoords.bottom());
        }
        if(glyphScreenCoords.right() > screenCoords.right()) {
            glyphTextureCoords.setRight(glyphTextureCoords.right() + (screenCoords.right() - glyphScreenCoords.right()));
            glyphScreenCoords.setRight(screenCoords.right());
        }

        // render glyph
        g_graphics.drawTexturedRect(glyphScreenCoords, m_texture, glyphTextureCoords);
    }

    g_graphics.stopDrawing();
}

const std::vector<Point>& Font::calculateGlyphsPositions(const std::string& text,
                                                         AlignmentFlag align,
                                                         Size *textBoxSize) const
{
    // for performance reasons we use statics vectors that are allocated on demand
    static std::vector<Point> glyphsPositions(1);
    static std::vector<int> lineWidths(1);

    int textLength = text.length();
    int maxLineWidth = 0;
    int lines = 0;
    int glyph;
    int i;

    // return if there is no text
    if(textLength == 0) {
        if(textBoxSize)
            textBoxSize->setSize(0,m_glyphHeight);
        return glyphsPositions;
    }

    // resize glyphsPositions vector when needed
    if(textLength > (int)glyphsPositions.size())
        glyphsPositions.resize(textLength);

    // calculate lines width
    if((align & AlignRight || align & AlignHorizontalCenter) || textBoxSize) {
        lineWidths[0] = 0;
        for(i = 0; i< textLength; ++i) {
            glyph = (uchar)text[i];

            if(glyph == (uchar)'\n') {
                lines++;
                if(lines+1 > (int)lineWidths.size())
                    lineWidths.resize(lines+1);
                lineWidths[lines] = 0;
            } else if(glyph >= 32) {
                lineWidths[lines] += m_glyphsSize[glyph].width();
                maxLineWidth = std::max(maxLineWidth, lineWidths[lines]);
            }
        }
    }

    Point virtualPos(0, m_topMargin);
    lines = 0;
    for(i = 0; i < textLength; ++i) {
        glyph = (uchar)text[i];

        // new line or first glyph
        if(glyph == (uchar)'\n' || i == 0) {
            if(glyph == (uchar)'\n') {
                virtualPos.y += m_glyphHeight + m_glyphSpacing.height();
                lines++;
            }

            // calculate start x pos
            if(align & AlignRight) {
                virtualPos.x = (maxLineWidth - lineWidths[lines]);
            } else if(align & AlignHorizontalCenter) {
                virtualPos.x = (maxLineWidth - lineWidths[lines]) / 2;
            } else { // AlignLeft
                virtualPos.x = 0;
            }
        }

        // store current glyph topLeft
        glyphsPositions[i] = virtualPos;

        // render only if the glyph is valid
        if(glyph >= 32 && glyph != (uchar)'\n') {
            virtualPos.x += m_glyphsSize[glyph].width();
        }
    }

    if(textBoxSize) {
        textBoxSize->setWidth(maxLineWidth);
        textBoxSize->setHeight(virtualPos.y + m_glyphHeight);
    }

    return glyphsPositions;
}

Size Font::calculateTextRectSize(const std::string& text)
{
    Size size;
    calculateGlyphsPositions(text, AlignTopLeft, &size);
    return size;
}

void Font::calculateGlyphsWidthsAutomatically(const Size& glyphSize)
{
    int numHorizontalGlyphs = m_texture->getSize().width() / glyphSize.width();
    auto texturePixels = m_texture->getPixels();

    // small AI to auto calculate pixels widths
    for(int glyph = m_firstGlyph; glyph< 256; ++glyph) {
        Rect glyphCoords(((glyph - m_firstGlyph) % numHorizontalGlyphs) * glyphSize.width(),
                         ((glyph - m_firstGlyph) / numHorizontalGlyphs) * glyphSize.height(),
                            glyphSize.width(),
                            m_glyphHeight);
        int width = glyphSize.width();
        int lastColumnFilledPixels = 0;
        for(int x = glyphCoords.left() + 1; x <= glyphCoords.right(); ++x) {
            int columnFilledPixels = 0;

            // check if all vertical pixels are alpha
            for(int y = glyphCoords.top(); y <= glyphCoords.bottom(); ++y) {
                if(texturePixels[(y * m_texture->getSize().width() * 4) + (x*4) + 3] != 0)
                    columnFilledPixels++;
            }

            // if all pixels were alpha we found the width
            if(columnFilledPixels == 0) {
                width = x - glyphCoords.left();
                width += m_glyphSpacing.width();
                if(m_glyphHeight >= 16 && lastColumnFilledPixels >= m_glyphHeight/3)
                    width += 1;
                break;
            }
            lastColumnFilledPixels = columnFilledPixels;
        }
        // store glyph size
        m_glyphsSize[glyph].setSize(width, m_glyphHeight);
    }
}
