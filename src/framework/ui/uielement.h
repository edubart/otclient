/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "../prerequisites.h"
#include "../input.h"
#include "../rect.h"
#include "uiconstants.h"

class UIContainer;
typedef std::shared_ptr<UIContainer> UIContainerPtr;

class UIElement;
typedef std::shared_ptr<UIElement> UIElementPtr;

class UIElement : public std::enable_shared_from_this<UIElement>
{
public:
    UIElement() { }
    virtual ~UIElement() { }

    virtual void render() { }
    virtual void update(int ticks, int elapsedTicks) { }
    virtual bool onInputEvent(InputEvent *event) { return false; }

    virtual void setParent(UIContainerPtr parent) { m_parent = parent; }
    UIContainerPtr getParent() const { return m_parent; }

    virtual void setName(const std::string& name) { m_name = name; }
    const std::string& getName() const { return m_name; }

    virtual void setRect(const Rect& rect) { m_rect = rect; }
    const Rect& getRect() const{ return m_rect; }

    virtual void setActive(bool active) { m_active = active; }
    bool isActive() const { return m_active; }

    virtual void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }

    virtual UI::ControlType getControlType() const { return UI::Element; }

    UIElementPtr asUIElement() { return shared_from_this(); }

protected:
    UI::ControlType  m_type;
    UIContainerPtr m_parent;
    Rect m_rect;
    std::string m_name;
    bool m_visible;
    bool m_active;
};

#endif // UIELEMENT_H
