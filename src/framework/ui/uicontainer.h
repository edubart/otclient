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


#ifndef UICONTAINER_H
#define UICONTAINER_H

#include "../prerequisites.h"
#include "uielement.h"
#include "../point.h"
#include "../rect.h"

class UIContainer : public UIElement
{
public:
    UIContainer(UIContainerPtr parent = UIContainerPtr()) : UIElement(parent) { }
    virtual ~UIContainer() { }

    virtual void addChild(UIElementPtr child);
    virtual void removeChild(UIElementPtr child);
    virtual UIElementPtr getChildByName(const std::string& name) const;

    virtual void setRect(const Rect& rect);
    virtual void resize(const Size& size);
    virtual void move(const Point& trans);
    virtual void moveTo(const Point& pos);

    virtual void render() { }
    virtual void update(int ticks, int elapsedTicks) { }
    virtual bool onInputEvent(InputEvent *event) { return false; }

    virtual void setActiveElement(UIElementPtr activeElement);
    UIElementPtr getActiveElement() const { return m_activeElement; }

    virtual UI::ControlType getControlType() const { return UI::Container; }
    UIContainerPtr asUIContainer() { return UIContainerPtr(this); }

protected:
    std::list<UIElementPtr> m_children;
    UIElementPtr m_activeElement;

private:
    void onMove(const Point& pos);
};

extern UIContainer g_gui;

#endif // UICONTAINER_H
