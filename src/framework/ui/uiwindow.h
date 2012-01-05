/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef UIWINDOW_H
#define UIWINDOW_H

#include "uiwidget.h"

class UIWindow : public UIWidget
{
    enum MovePolicy {
        DONT_MOVE = 0,
        FREE_MOVE,
        FREE_UPDATED_MOVE
    };

public:
    UIWindow();
    virtual void render();

    void setTitle(const std::string& title) { m_title = title; }
    std::string getTitle() const { return m_title; }

protected:
    virtual void onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode);
    virtual void onGeometryChange(const Rect& oldRect, const Rect& newRect);
    virtual bool onMousePress(const Point& mousePos, Fw::MouseButton button);
    virtual void onMouseRelease(const Point& mousePos, Fw::MouseButton button);
    virtual bool onMouseMove(const Point& mousePos, const Point& mouseMoved);
    virtual bool onKeyPress(uchar keyCode, std::string keyText, int keyboardModifiers);

private:
    std::string m_title;
    bool m_moving;
    MovePolicy m_movePolicy;
    Fw::AlignmentFlag m_titleAlign;
    Point m_headTextOffset;
    Point m_movingReference;
    Point m_oldPos;
    int m_oldIndex;
    int m_headHeight;
};

#endif
