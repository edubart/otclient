/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#include "uiwindow.h"
#include "uitranslator.h"
#include <framework/graphics/borderimage.h>
#include <framework/graphics/font.h>
#include <framework/graphics/graphics.h>
#include <framework/otml/otml.h>

UIWindow::UIWindow()
{
    m_moving = false;
    m_movePolicy = DONT_MOVE;
    m_titleAlign = Fw::AlignCenter;
    m_headHeight = 0;
    m_oldIndex = -1;
}

void UIWindow::render()
{
    // render children
    UIWidget::render();

    // draw window head

    // draw window head text
    Rect headTextRect = m_rect;
    headTextRect.addTop(-m_headOffset.y);
    headTextRect.setHeight(m_headHeight);
    if(m_titleAlign & Fw::AlignLeft)
        headTextRect.addLeft(-m_headOffset.x);
    else if(m_titleAlign & Fw::AlignRight)
        headTextRect.addRight(-m_headOffset.x);
    else {
        headTextRect.addLeft(-m_headOffset.x);
        headTextRect.addRight(-m_headOffset.x);
    }
    m_font->renderText(m_title, headTextRect, m_titleAlign, m_foregroundColor);
}

void UIWindow::onStyleApply(const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleNode);

    for(OTMLNodePtr node : styleNode->children()) {
        if(node->tag() == "head height")
            m_headHeight = node->value<int>();
        else if(node->tag() == "head offset")
            m_headOffset = node->value<Point>();
        else if(node->tag() == "title")
            setTitle(node->value());
        else if(node->tag() == "head text align")
            m_titleAlign = Fw::translateAlignment(node->value());
        else if(node->tag() == "move policy") {
            if(node->value() == "free")
                m_movePolicy = FREE_MOVE;
            else if(node->value() == "free updated")
                m_movePolicy = FREE_UPDATED_MOVE;
            else
                m_movePolicy = DONT_MOVE;
        }
        else if(node->tag() == "onEnter") {
            g_lua.loadFunction(node->value(), "@" + node->source() + "[" + node->tag() + "]");
            luaSetField(node->tag());
        }
        else if(node->tag() == "onEscape") {
            g_lua.loadFunction(node->value(), "@" + node->source() + "[" + node->tag() + "]");
            luaSetField(node->tag());
        }
    }
}

void UIWindow::onGeometryUpdate(const Rect& oldRect, const Rect& newRect)
{
    // bind window rect to parent rect
    Rect boundRect = newRect;
    UIWidgetPtr parent = getParent();
    if(parent) {
        Rect parentRect = parent->getRect();
        boundRect.bound(parentRect);
    }

    if(boundRect != newRect)
        setRect(boundRect);
}

bool UIWindow::onMousePress(const Point& mousePos, Fw::MouseButton button)
{
    if(m_movePolicy != DONT_MOVE) {
        UIWidgetPtr clickedChild = getChildByPos(mousePos);
        //FIXME: recursively check for non phantom children
        if(!clickedChild || clickedChild->isPhantom()) {
            m_moving = true;
            m_movingReference = mousePos - getRect().topLeft();
            m_oldIndex = getParent()->getChildIndex(asUIWidget());
            m_oldPos = getPosition();
            getParent()->moveChildToTop(asUIWidget());
        }
    }
    return UIWidget::onMousePress(mousePos, button);
}

void UIWindow::onMouseRelease(const Point& mousePos, Fw::MouseButton button)
{
    if(m_moving) {
        if(m_movePolicy == FREE_UPDATED_MOVE) {
            UIWidgetPtr parent = getParent();

            // restore position before move
            parent->moveChildToIndex(asUIWidget(), m_oldIndex);
            moveTo(m_oldPos);

            // calculate new index
            int newIndex;
            for(newIndex=parent->getChildCount();newIndex>1;--newIndex) {
                UIWidgetPtr child = parent->getChildByIndex(newIndex);
                if(mousePos.y >= child->getRect().top())
                    break;
            }

            // set the new index
            parent->moveChildToIndex(asUIWidget(), newIndex);
            updateParentLayout();
        }
        m_moving = false;
    }
    UIWidget::onMouseRelease(mousePos, button);
}

bool UIWindow::onMouseMove(const Point& mousePos, const Point& mouseMoved)
{
    if(m_moving) {
        moveTo(mousePos - m_movingReference);
        return true;
    }
    return UIWidget::onMouseMove(mousePos, mouseMoved);
}

bool UIWindow::onKeyPress(uchar keyCode, char keyChar, int keyboardModifiers)
{
    if(keyboardModifiers == Fw::KeyboardNoModifier) {
        if(keyCode == Fw::KeyReturn || keyCode == Fw::KeyEnter) {
            if(callLuaField<bool>("onEnter"))
                return true;
        } else if(keyCode == Fw::KeyEscape) {
            if(callLuaField<bool>("onEscape"))
                return true;
        }
    }
    return UIWidget::onKeyPress(keyCode, keyChar, keyboardModifiers);
}
