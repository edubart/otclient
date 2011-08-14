#ifndef TEXTAREA_H
#define TEXTAREA_H

#include "graphicsdeclarations.h"

class TextArea
{
public:
    TextArea();
    TextArea(FontPtr font,
             const std::string& text,
             const Rect& screenCoords,
             AlignmentFlag align = AlignTopLeft,
             const Color& color = Color::white);

    void draw();

    void setFont(FontPtr font);
    void setText(const std::string& text);
    void setScreenCoords(const Rect& screenCoords);
    void setAlign(AlignmentFlag align);
    void setColor(const Color& color) { m_color = color; }
    void setCursorPos(int pos);
    void enableCursor(bool enable = true);
    void setCursorVisible(bool visible = true) { m_cursorVisible = visible; }

    void moveCursor(bool right);
    void appendCharacter(char c);
    void removeCharacter(bool right);

    std::string getText() const { return m_text; }

    FontPtr getFont() const { return m_font; }
    int getTextPos(Point pos);

private:
    void recalculate();

    FontPtr m_font;
    std::string m_text;
    Rect m_screenCoords;
    Rect m_drawArea;
    AlignmentFlag m_align;
    Color m_color;
    int m_cursorPos;
    Point m_startInternalPos;
    int m_startRenderPos;
    int m_cursorTicks;
    bool m_cursorVisible;

    std::vector<Rect> m_glyphsCoords;
    std::vector<Rect> m_glyphsTexCoords;
};

#endif
