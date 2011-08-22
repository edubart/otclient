#ifndef UILINEEDIT_H
#define UILINEEDIT_H

#include "uiwidget.h"

class UILineEdit : public UIWidget
{
public:
    UILineEdit();

    static UILineEditPtr create();

    virtual void render();

    void update();

    void setText(const std::string& text);
    void setAlign(AlignmentFlag align);
    void setCursorPos(int pos);
    void setCursorEnabled(bool enable = true);

    void clearText() { setText(""); }
    void moveCursor(bool right);
    void appendCharacter(char c);
    void removeCharacter(bool right);

    void setFont(const FontPtr& font);
    std::string getText() const { return m_text; }
    int getTextPos(Point pos);

protected:
    virtual void onStyleApply(const OTMLNodePtr& styleNode);
    virtual void onGeometryUpdate(const Rect& oldRect, const Rect& newRect);
    virtual void onFocusChange(bool focused, UI::FocusReason reason);
    virtual bool onKeyPress(uchar keyCode, char keyChar, int keyboardModifiers);
    virtual bool onMousePress(const Point& mousePos, UI::MouseButton button);

private:
    void blinkCursor();

    std::string m_text;
    Rect m_drawArea;
    AlignmentFlag m_align;
    int m_cursorPos;
    Point m_startInternalPos;
    int m_startRenderPos;
    int m_cursorTicks;
    int m_textHorizontalMargin;
    SimpleCallback m_onAction;

    std::vector<Rect> m_glyphsCoords;
    std::vector<Rect> m_glyphsTexCoords;
};

#endif
