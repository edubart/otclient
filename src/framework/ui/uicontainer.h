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

#include "prerequisites.h"
#include "uielement.h"

class UIContainer : public UIElement
{
public:
    UIContainer(UI::EElementType type = UI::Container) : UIElement(type) { }
    virtual ~UIContainer() { }

    virtual void render();
    virtual void onInputEvent(const InputEvent& event);

    void addChild(UIElementPtr child);
    void removeChild(UIElementPtr child);
    UIElementPtr getChildById(const std::string& id);

    UIElementPtr recursiveGetChildById(const std::string& id);

    void setFocusedElement(UIElementPtr focusedElement);
    UIElementPtr getFocusedElement() const { return m_focusedElement; }

    virtual UI::EElementType getElementType() const { return UI::Container; }

    UIContainerPtr asUIContainer() { return boost::static_pointer_cast<UIContainer>(shared_from_this()); }

    static UIContainerPtr& getRootContainer();

private:
    std::list<UIElementPtr> m_children;
    UIElementPtr m_focusedElement;
};

#endif // UICONTAINER_H
