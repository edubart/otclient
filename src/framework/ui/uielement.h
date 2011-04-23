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

#include <prerequisites.h>
#include <core/input.h>
#include <ui/uiconstants.h>
#include <ui/uilayout.h>
#include <ui/uielementskin.h>

class UIElementSkin;

class UIContainer;
typedef boost::shared_ptr<UIContainer> UIContainerPtr;
typedef boost::weak_ptr<UIContainer> UIContainerWeakPtr;

class UIElement;
typedef boost::shared_ptr<UIElement> UIElementPtr;
typedef boost::weak_ptr<UIElement> UIElementWeakPtr;

class UIElement : public UILayout
{
public:
    UIElement(UI::EElementType type = UI::Element);
    virtual ~UIElement() { }

    /// Destroy this element by removing it from its parent
    void destroy();

    /// Draw element
    virtual void render();

    // events
    virtual void onInputEvent(const InputEvent& event) { }
    virtual void onFocusChange() { }

    UIElementPtr backwardsGetElementById(const std::string& id);

    void moveTo(Point pos);

    void setSkin(const UIElementSkinPtr& skin);
    const UIElementSkinPtr& getSkin() const { return m_skin; }

    void setParent(UIContainerPtr parent) { m_parent = parent; }
    UIContainerPtr getParent() const { return m_parent.lock(); }

    void setId(const std::string& id) { m_id = id; }
    const std::string& getId() const { return m_id; }

    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    void setFocused(bool focused) { m_focused = focused; }
    bool isFocused() const { return m_focused; }

    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }

    virtual bool isFocusable() const { return false; }
    UI::EElementType getElementType() const { return m_type; }

    UIElementPtr asUIElement() { return boost::static_pointer_cast<UIElement>(shared_from_this()); }
    virtual UIContainerPtr asUIContainer() { return UIContainerPtr(); }
    virtual const char *getScriptableName() const { return "UIElement"; }

    void setOnDestroy(
    friend class UIContainer;

private:
    void internalDestroy();

    UI::EElementType m_type;
    UIContainerWeakPtr m_parent;
    UIElementSkinPtr m_skin;
    std::string m_id;
    bool m_visible;
    bool m_enabled;
    bool m_focused;
};

#endif // UIELEMENT_H
