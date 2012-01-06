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

#include "uiwidget.h"
#include "uimanager.h"
#include "uianchorlayout.h"
#include "uiverticallayout.h"
#include "uitranslator.h"

#include <framework/core/eventdispatcher.h>
#include <framework/graphics/image.h>
#include <framework/graphics/borderimage.h>
#include <framework/graphics/fontmanager.h>
#include <framework/otml/otmlnode.h>
#include <framework/graphics/graphics.h>
#include <framework/platform/platformwindow.h>
#include <framework/graphics/texturemanager.h>

UIWidget::UIWidget()
{
    m_lastFocusReason = Fw::ActiveFocusReason;
    m_states = Fw::DefaultState;
    m_font = g_fonts.getDefaultFont();
    m_opacity = 255;
    m_marginTop = m_marginRight = m_marginBottom = m_marginLeft = 0;
    //m_backgroundColor = Fw::alpha;
    m_backgroundColor = Fw::white;
    m_foregroundColor = Fw::white;
    m_textAlign = Fw::AlignCenter;

    // generate an unique id, this is need because anchored layouts find widgets by id
    static unsigned long id = 1;
    m_id = Fw::mkstr("widget", id++);
}

void UIWidget::destroy()
{
    setVisible(false);
    setEnabled(false);

    // release input grabs
    if(g_ui.getKeyboardReceiver() == asUIWidget())
        g_ui.resetKeyboardReceiver();

    if(g_ui.getMouseReceiver() == asUIWidget())
        g_ui.resetMouseReceiver();

    // remove itself from parent
    if(UIWidgetPtr parent = getParent()) {
        if(parent->hasChild(asUIWidget()))
            parent->removeChild(asUIWidget());
    }

    callLuaField("onDestroy");
}

void UIWidget::render()
{
    renderSelf();
    renderChildren();
}

void UIWidget::renderSelf()
{
    // draw style components in order
    drawBackground(m_rect);
    drawBorder(m_rect);
    drawImage(m_rect);
    drawIcon(m_rect);
    drawText(m_rect);
}

void UIWidget::renderChildren()
{
    // draw children
    for(const UIWidgetPtr& child : m_children) {
        // render only visible children with a valid rect inside our rect
        if(child->isExplicitlyVisible() &&
           child->getRect().isValid() &&
           child->getRect().intersects(m_rect) &&
           child->getOpacity() > 0) {
            // store current graphics opacity
            int oldOpacity = g_painter.getOpacity();

            // decrease to self opacity
            if(child->getOpacity() < oldOpacity)
                g_painter.setOpacity(child->getOpacity());

            child->render();

            // debug draw box
            //g_painter.setColor(Fw::green);
            //g_painter.drawBoundingRect(child->getRect());
            //g_fonts.getDefaultFont()->renderText(child->getId(), child->getPos() + Point(2, 0), Fw::red);

            g_painter.setOpacity(oldOpacity);
        }
    }
}

void UIWidget::drawBackground(const Rect& screenCoords)
{
    /*
    if(m_backgroundColor.a() > 0) {
        g_painter.setColor(m_backgroundColor);
        g_painter.drawFilledRect(screenCoords);
        //g_painter.drawFilledRect(screenCoords.expanded(-m_borderWidth));
    }
    */
}

void UIWidget::drawBorder(const Rect& screenCoords)
{
    /*
    if(m_borderWidth > 0 && m_borderColor.a() > 0) {
        g_painter.bindColor(m_borderColor);
        g_painter.drawBoundingRect(screenCoords, m_borderWidth);
    }
    */
}

void UIWidget::drawImage(const Rect& screenCoords)
{
    if(m_image) {
        g_painter.setColor(m_backgroundColor);
        m_image->draw(screenCoords);
    }
}

void UIWidget::drawIcon(const Rect& screenCoords)
{
    if(m_icon) {
        Rect iconRect;
        iconRect.resize(m_icon->getSize());
        iconRect.moveCenter(screenCoords.center());
        g_painter.setColor(Fw::white);
        g_painter.drawTexturedRect(iconRect, m_icon);
    }
}

void UIWidget::drawText(const Rect& screenCoords)
{
    g_painter.setColor(m_foregroundColor);
    if(m_text.length() > 0 && m_foregroundColor.a() > 0) {
        Rect textRect = screenCoords;
        textRect.translate(m_textOffset);
        m_font->renderText(m_text, textRect, m_textAlign, m_foregroundColor);
    }
}

void UIWidget::setVisible(bool visible)
{
    if(m_visible != visible) {
        m_visible = visible;

        // make parent focus another child
        if(!visible && isFocused()) {
            if(UIWidgetPtr parent = getParent())
                parent->focusPreviousChild(Fw::ActiveFocusReason);
        }

        updateState(Fw::ActiveState);
        updateState(Fw::HoverState);
    }
}

void UIWidget::setEnabled(bool enabled)
{
    if(enabled != m_enabled) {
        m_enabled = enabled;

        updateState(Fw::DisabledState);
        updateState(Fw::ActiveState);
        updateState(Fw::HoverState);
    }
}

void UIWidget::setPressed(bool pressed)
{
    if(pressed != m_pressed) {
        m_pressed = pressed;
        updateState(Fw::PressedState);
    }
}

void UIWidget::setOn(bool on)
{
    setState(Fw::OnState, on);
}

void UIWidget::setChecked(bool checked)
{
    if(setState(Fw::CheckedState, checked))
        callLuaField("onCheckChange", checked);
}

void UIWidget::setFocusable(bool focusable)
{
    if(m_focusable != focusable) {
        m_focusable = focusable;

        // make parent focus another child
        if(!focusable && isFocused()) {
            if(UIWidgetPtr parent = getParent())
                parent->focusPreviousChild(Fw::ActiveFocusReason);
        }
    }
}

void UIWidget::setStyle(const std::string& styleName)
{
    OTMLNodePtr styleNode = g_ui.getStyle(styleName);
    if(!styleNode) {
        logTraceError("unable to retrive style '", styleName, "': not a defined style");
        return;
    }
    applyStyle(styleNode);
    m_style = styleNode;
    assert(getStyleName() != "");
    updateStyle();
}

void UIWidget::setStyleFromNode(const OTMLNodePtr& styleNode)
{
    applyStyle(styleNode);
    m_style = styleNode;
    updateStyle();
}

void UIWidget::setParent(const UIWidgetPtr& parent)
{
    // remove from old parent
    UIWidgetPtr oldParent = getParent();

    // the parent is already the same
    if(oldParent == parent)
        return;

    UIWidgetPtr self = asUIWidget();
    if(oldParent && oldParent->hasChild(self))
        oldParent->removeChild(self);

    // reset parent
    m_parent.reset();

    // set new parent
    if(parent) {
        m_parent = parent;

        // add to parent if needed
        if(!parent->hasChild(self))
            parent->addChild(self);
    }
}

void UIWidget::setRect(const Rect& rect)
{
    // only update if the rect really changed
    Rect oldRect = m_rect;
    if(rect == oldRect)
        return;

    m_rect = rect;

    // updates own layout
    updateLayout();

    // avoid massive update events
    if(!m_updateEventScheduled) {
        UIWidgetPtr self = asUIWidget();
        g_dispatcher.addEvent([self, oldRect]() {
            self->m_updateEventScheduled = false;
            self->onGeometryChange(oldRect, self->getRect());
        });
        m_updateEventScheduled = true;
    }
}

void UIWidget::setIcon(const std::string& iconFile)
{
    m_icon = g_textures.getTexture(iconFile);
}

void UIWidget::setText(const std::string& text)
{
    if(m_text != text) {
        m_text = text;

        // update rect size
        if(!m_rect.isValid()) {
            Size textSize = m_font->calculateTextRectSize(m_text);
            Size newSize = getSize();
            if(newSize.width() <= 0)
                newSize.setWidth(textSize.width());
            if(newSize.height() <= 0)
                newSize.setHeight(textSize.height());
            resize(newSize);
        }

        onTextChange(text);
    }
}

void UIWidget::setFont(const std::string& fontName)
{
    m_font = g_fonts.getFont(fontName);
}

void UIWidget::bindRectToParent()
{
    Rect boundRect = m_rect;
    UIWidgetPtr parent = getParent();
    if(parent) {
        Rect parentRect = parent->getRect();
        boundRect.bound(parentRect);
    }

    setRect(boundRect);
}

void UIWidget::lock()
{
    if(UIWidgetPtr parent = getParent())
        parent->lockChild(asUIWidget());
}

void UIWidget::unlock()
{
    if(UIWidgetPtr parent = getParent())
        parent->unlockChild(asUIWidget());
}

void UIWidget::focus()
{
    if(!m_focusable)
        return;
    if(UIWidgetPtr parent = getParent())
        parent->focusChild(asUIWidget(), Fw::ActiveFocusReason);
}

void UIWidget::grabMouse()
{
    g_ui.setMouseReceiver(asUIWidget());
}

void UIWidget::ungrabMouse()
{
    if(g_ui.getMouseReceiver() == asUIWidget())
        g_ui.resetMouseReceiver();
}

void UIWidget::grabKeyboard()
{
    g_ui.setKeyboardReceiver(asUIWidget());
}

void UIWidget::ungrabKeyboard()
{
    if(g_ui.getKeyboardReceiver() == asUIWidget())
        g_ui.resetKeyboardReceiver();
}

bool UIWidget::isVisible()
{
    if(!m_visible)
        return false;
    else if(UIWidgetPtr parent = getParent())
        return parent->isVisible();
    else
        return asUIWidget() == g_ui.getRootWidget();
}

bool UIWidget::hasChild(const UIWidgetPtr& child)
{
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if(it != m_children.end())
        return true;
    return false;
}

UIWidgetPtr UIWidget::getRootParent()
{
    if(UIWidgetPtr parent = getParent())
        return parent->getRootParent();
    else
        return asUIWidget();
}

UIWidgetPtr UIWidget::getChildAfter(const UIWidgetPtr& relativeChild)
{
    auto it = std::find(m_children.begin(), m_children.end(), relativeChild);
    if(it != m_children.end() && ++it != m_children.end())
        return *it;
    return nullptr;
}

UIWidgetPtr UIWidget::getChildBefore(const UIWidgetPtr& relativeChild)
{
    auto it = std::find(m_children.rbegin(), m_children.rend(), relativeChild);
    if(it != m_children.rend() && ++it != m_children.rend())
        return *it;
    return nullptr;
}

UIWidgetPtr UIWidget::getChildById(const std::string& childId)
{
    for(const UIWidgetPtr& child : m_children) {
        if(child->getId() == childId)
            return child;
    }
    return nullptr;
}

UIWidgetPtr UIWidget::getChildByPos(const Point& childPos)
{
    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        const UIWidgetPtr& widget = (*it);
        if(widget->isExplicitlyVisible() && widget->containsPoint(childPos))
            return widget;
    }

    return nullptr;
}

UIWidgetPtr UIWidget::getChildByIndex(int index)
{
    index = index <= 0 ? (m_children.size() + index) : index-1;
    if(index >= 0 && (uint)index < m_children.size())
        return m_children.at(index);
    return nullptr;
}

UIWidgetPtr UIWidget::recursiveGetChildById(const std::string& id)
{
    UIWidgetPtr widget = getChildById(id);
    if(!widget) {
        for(const UIWidgetPtr& child : m_children) {
            widget = child->recursiveGetChildById(id);
            if(widget)
                break;
        }
    }
    return widget;
}

UIWidgetPtr UIWidget::recursiveGetChildByPos(const Point& childPos)
{
    for(const UIWidgetPtr& child : m_children) {
        if(child->containsPoint(childPos)) {
            if(UIWidgetPtr subChild = child->recursiveGetChildByPos(childPos))
                return subChild;
            return child;
        }
    }
    return nullptr;
}

UIWidgetPtr UIWidget::backwardsGetWidgetById(const std::string& id)
{
    UIWidgetPtr widget = getChildById(id);
    if(!widget) {
        if(UIWidgetPtr parent = getParent())
            widget = parent->backwardsGetWidgetById(id);
    }
    return widget;
}

void UIWidget::focusChild(const UIWidgetPtr& child, Fw::FocusReason reason)
{
    if(child && !hasChild(child)) {
        logError("Attempt to focus an unknown child in a UIWidget");
        return;
    }

    if(child != m_focusedChild) {
        UIWidgetPtr oldFocused = m_focusedChild;
        m_focusedChild = child;

        if(child) {
            child->setLastFocusReason(reason);
            child->updateState(Fw::FocusState);
            child->updateState(Fw::ActiveState);
        }

        if(oldFocused) {
            oldFocused->setLastFocusReason(reason);
            oldFocused->updateState(Fw::FocusState);
            oldFocused->updateState(Fw::ActiveState);
        }
    }
}

void UIWidget::addChild(const UIWidgetPtr& child)
{
    if(!child) {
        logWarning("attempt to add a null child into a UIWidget");
        return;
    }

    if(hasChild(child)) {
        logWarning("attempt to add a child again into a UIWidget");
        return;
    }

    m_children.push_back(child);
    child->setParent(asUIWidget());

    // create default layout
    if(!m_layout)
        m_layout = UIAnchorLayoutPtr(new UIAnchorLayout(asUIWidget()));

    // add to layout and updates it
    m_layout->addWidget(child);

    // update new child states
    child->updateStates();
    updateChildrenIndexStates();
}

void UIWidget::insertChild(int index, const UIWidgetPtr& child)
{
    if(!child) {
        logWarning("attempt to insert a null child into a UIWidget");
        return;
    }

    if(hasChild(child)) {
        logWarning("attempt to insert a child again into a UIWidget");
        return;
    }

    index = index <= 0 ? (m_children.size() + index) : index-1;

    assert(index >= 0 && (uint)index <= m_children.size());

    // retrieve child by index
    auto it = m_children.begin() + index;
    m_children.insert(it, child);
    child->setParent(asUIWidget());

    // create default layout if needed
    if(!m_layout)
        m_layout = UIAnchorLayoutPtr(new UIAnchorLayout(asUIWidget()));

    // add to layout and updates it
    m_layout->addWidget(child);

    // update new child states
    child->updateStates();
    updateChildrenIndexStates();
}

void UIWidget::removeChild(const UIWidgetPtr& child)
{
    // remove from children list
    if(hasChild(child)) {
        // defocus if needed
        bool focusAnother = false;
        if(m_focusedChild == child) {
            focusChild(nullptr, Fw::ActiveFocusReason);
            focusAnother = true;
        }

        if(isChildLocked(child))
            unlockChild(child);

        auto it = std::find(m_children.begin(), m_children.end(), child);
        m_children.erase(it);

        // reset child parent
        assert(child->getParent() == asUIWidget());
        child->setParent(nullptr);

        m_layout->removeWidget(child);

        // update child states
        child->updateStates();
        updateChildrenIndexStates();

        if(focusAnother && !m_focusedChild)
            focusPreviousChild(Fw::ActiveFocusReason);
    } else
        logError("Attempt to remove an unknown child from a UIWidget");
}

void UIWidget::focusNextChild(Fw::FocusReason reason)
{
    UIWidgetPtr toFocus;
    UIWidgetList rotatedChildren(m_children);

    if(m_focusedChild) {
        auto focusedIt = std::find(rotatedChildren.begin(), rotatedChildren.end(), m_focusedChild);
        if(focusedIt != rotatedChildren.end()) {
            std::rotate(rotatedChildren.begin(), focusedIt, rotatedChildren.end());
            rotatedChildren.pop_front();
        }
    }

    // finds next child to focus
    for(const UIWidgetPtr& child : rotatedChildren) {
        if(child->isFocusable() && child->isExplicitlyEnabled() && child->isVisible()) {
            toFocus = child;
            break;
        }
    }

    if(toFocus)
        focusChild(toFocus, reason);
}

void UIWidget::focusPreviousChild(Fw::FocusReason reason)
{
    UIWidgetPtr toFocus;
    UIWidgetList rotatedChildren(m_children);
    std::reverse(rotatedChildren.begin(), rotatedChildren.end());

    if(m_focusedChild) {
        auto focusedIt = std::find(rotatedChildren.begin(), rotatedChildren.end(), m_focusedChild);
        if(focusedIt != rotatedChildren.end()) {
            std::rotate(rotatedChildren.begin(), focusedIt, rotatedChildren.end());
            rotatedChildren.pop_front();
        }
    }

    // finds next child to focus
    for(const UIWidgetPtr& child : rotatedChildren) {
        if(child->isFocusable() && child->isExplicitlyEnabled() && child->isVisible()) {
            toFocus = child;
            break;
        }
    }

    if(toFocus)
        focusChild(toFocus, reason);
}

void UIWidget::moveChildToTop(const UIWidgetPtr& child)
{
    if(!child)
        return;

    // remove and push child again
    auto it = std::find(m_children.begin(), m_children.end(), child);
    assert(it != m_children.end());
    m_children.erase(it);
    m_children.push_back(child);
    updateChildrenIndexStates();
}

void UIWidget::moveChildToIndex(const UIWidgetPtr& child, int index)
{
    if(!child)
        return;

    // remove and push child again
    auto it = std::find(m_children.begin(), m_children.end(), child);
    assert(it != m_children.end());
    m_children.erase(it);
    m_children.insert(m_children.begin() + index - 1, child);
    updateChildrenIndexStates();
}

void UIWidget::lockChild(const UIWidgetPtr& child)
{
    if(!child)
        return;

    assert(hasChild(child));

    // prevent double locks
    if(isChildLocked(child))
        unlockChild(child);

    // disable all other children
    for(const UIWidgetPtr& otherChild : m_children) {
        if(otherChild == child)
            child->setEnabled(true);
        else
            otherChild->setEnabled(false);
    }

    m_lockedChildren.push_front(child);

    // lock child focus
    if(child->isFocusable())
        focusChild(child, Fw::ActiveFocusReason);

    moveChildToTop(child);
}

void UIWidget::unlockChild(const UIWidgetPtr& child)
{
    if(!child)
        return;

    assert(hasChild(child));

    auto it = std::find(m_lockedChildren.begin(), m_lockedChildren.end(), child);
    if(it == m_lockedChildren.end())
        return;

    m_lockedChildren.erase(it);

    // find new child to lock
    UIWidgetPtr lockedChild;
    if(m_lockedChildren.size() > 0) {
        lockedChild = m_lockedChildren.front();
        assert(hasChild(lockedChild));
    }

    for(const UIWidgetPtr& otherChild : m_children) {
        // lock new child
        if(lockedChild) {
            if(otherChild == lockedChild)
                lockedChild->setEnabled(true);
            else
                otherChild->setEnabled(false);
        }
        // else unlock all
        else
            otherChild->setEnabled(true);
    }

    if(lockedChild) {
        if(lockedChild->isFocusable())
            focusChild(lockedChild, Fw::ActiveFocusReason);

        moveChildToTop(lockedChild);
    }
}

bool UIWidget::isChildLocked(const UIWidgetPtr& child)
{
    auto it = std::find(m_lockedChildren.begin(), m_lockedChildren.end(), child);
    return it != m_lockedChildren.end();
}

int UIWidget::getChildIndex(const UIWidgetPtr& child)
{
    int index = 1;
    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        if(*it == child)
            return index;
        ++index;
    }
    return -1;
}

void UIWidget::updateParentLayout()
{
    if(UIWidgetPtr parent = getParent())
        parent->updateLayout();
    else
        updateLayout();
}

void UIWidget::updateLayout()
{
    if(m_layout)
        m_layout->update();
}

void UIWidget::applyStyle(const OTMLNodePtr& styleNode)
{
    try {
        m_loadingStyle = true;
        onStyleApply(styleNode->tag(), styleNode);
        callLuaField("onStyleApply", styleNode->tag(), styleNode);

        if(m_firstOnStyle) {
            callLuaField("onSetup");
            // always focus new child
            if(isFocusable() && isExplicitlyVisible() && isExplicitlyEnabled())
                focus();
        }
        m_firstOnStyle = false;

        m_loadingStyle = false;
    } catch(Exception& e) {
        logError("Failed to apply style to widget '", m_id, "' style: ", e.what());
    }
}

bool UIWidget::setState(Fw::WidgetState state, bool on)
{
    if(state == Fw::InvalidState)
        return false;

    int oldStates = m_states;
    if(on)
        m_states |= state;
    else
        m_states &= ~state;

    if(oldStates != m_states) {
        updateStyle();
        return true;
    }
    return false;
}

bool UIWidget::hasState(Fw::WidgetState state)
{
    if(state == Fw::InvalidState)
        return false;
    return (m_states & state);
}

void UIWidget::updateState(Fw::WidgetState state)
{
    bool newStatus = true;
    bool oldStatus = hasState(state);
    bool updateChildren = false;

    switch(state) {
        case Fw::ActiveState: {
            UIWidgetPtr widget = asUIWidget();
            UIWidgetPtr parent;
            do {
                parent = widget->getParent();
                if(!widget->isExplicitlyEnabled() ||
                   ((parent && parent->getFocusedChild() != widget))) {
                    newStatus = false;
                    break;
                }
            } while(widget = parent);

            updateChildren = true;
            break;
        }
        case Fw::FocusState: {
            newStatus = (getParent() && getParent()->getFocusedChild() == asUIWidget());
            break;
        }
        case Fw::HoverState: {
            updateChildren = true;
            Point mousePos = g_window.getMousePos();
            UIWidgetPtr widget = asUIWidget();
            UIWidgetPtr parent;
            do {
                parent = widget->getParent();
                if(!widget->isExplicitlyEnabled() || !widget->isExplicitlyVisible() || !widget->containsPoint(mousePos) ||
                   (parent && widget != parent->getChildByPos(mousePos))) {
                    newStatus = false;
                    break;
                }
            } while(widget = parent);
            break;
        }
        case Fw::PressedState: {
            newStatus = m_pressed;
            break;
        }
        case Fw::DisabledState: {
            bool enabled = true;
            updateChildren = true;
            UIWidgetPtr widget = asUIWidget();
            do {
                if(!widget->isExplicitlyEnabled()) {
                    enabled = false;
                    break;
                }
            } while(widget = widget->getParent());
            newStatus = !enabled;
            break;
        }
        case Fw::FirstState: {
            newStatus = (getParent() && getParent()->getFirstChild() == asUIWidget());
            break;
        }
        case Fw::MiddleState: {
            newStatus = (getParent() && getParent()->getFirstChild() != asUIWidget() && getParent()->getLastChild() != asUIWidget());
            break;
        }
        case Fw::LastState: {
            newStatus = (getParent() && getParent()->getLastChild() == asUIWidget());
            break;
        }
        case Fw::AlternateState: {
            newStatus = (getParent() && (getParent()->getChildIndex(asUIWidget()) % 2) == 1);
            break;
        }
        default:
            return;
    }

    if(updateChildren) {
        // do a backup of children list, because it may change while looping it
        UIWidgetList children = m_children;
        for(const UIWidgetPtr& child : children)
            child->updateState(state);
    }

    if(setState(state, newStatus)) {
        if(state == Fw::FocusState) {
            g_dispatcher.addEvent(std::bind(&UIWidget::onFocusChange, asUIWidget(), newStatus, m_lastFocusReason));
        } else if(state == Fw::HoverState)
            g_dispatcher.addEvent(std::bind(&UIWidget::onHoverChange, asUIWidget(), newStatus));
    }
}

void UIWidget::updateStates()
{
    for(int state = 1; state != Fw::LastWidgetState; state <<= 1)
        updateState((Fw::WidgetState)state);
}

void UIWidget::updateChildrenIndexStates()
{
    for(const UIWidgetPtr& child : m_children) {
        child->updateState(Fw::FirstState);
        child->updateState(Fw::MiddleState);
        child->updateState(Fw::LastState);
        child->updateState(Fw::AlternateState);
    }
}

void UIWidget::updateStyle()
{
    if(m_loadingStyle && !m_updateStyleScheduled) {
        UIWidgetPtr self = asUIWidget();
        g_dispatcher.addEvent([self] {
            self->m_updateStyleScheduled = false;
            self->updateStyle();
        });
        m_updateStyleScheduled = true;
        return;
    }

    if(!m_style)
        return;

    OTMLNodePtr newStateStyle = OTMLNode::create();

    // copy only the changed styles from default style
    if(m_stateStyle) {
        for(OTMLNodePtr node : m_stateStyle->children()) {
            if(OTMLNodePtr otherNode = m_style->get(node->tag()))
                newStateStyle->addChild(otherNode->clone());
        }
    }

    // checks for states combination
    for(const OTMLNodePtr& style : m_style->children()) {
        if(boost::starts_with(style->tag(), "$")) {
            std::string statesStr = style->tag().substr(1);
            std::vector<std::string> statesSplit;
            boost::split(statesSplit, statesStr, boost::is_any_of(std::string(" ")));

            bool match = true;
            for(std::string stateStr : statesSplit) {
                if(stateStr.length() == 0)
                    continue;

                bool notstate = (stateStr[0] == '!');
                if(notstate)
                    stateStr = stateStr.substr(1);

                bool stateOn = hasState(Fw::translateState(stateStr));
                if((!notstate && !stateOn) || (notstate && stateOn))
                    match = false;
            }

            // merge states styles
            if(match) {
                newStateStyle->merge(style);
            }
        }
    }

    //TODO: prevent setting already set proprieties

    applyStyle(newStateStyle);
    m_stateStyle = newStateStyle;
}

void UIWidget::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    // first set id
    if(const OTMLNodePtr& node = styleNode->get("id"))
        setId(node->value());

    // load styles used by all widgets
    for(const OTMLNodePtr& node : styleNode->children()) {
        // background image
        if(node->tag() == "image") {
            ImagePtr image = ImagePtr(new Image);
            image->loadFromOTML(node);
            setImage(image);
        }
        else if(node->tag() == "border-image")
            setImage(BorderImage::loadFromOTML(node));
        if(node->tag() == "icon")
            setIcon(node->value());
        else if(node->tag() == "text")
            setText(node->value());
        else if(node->tag() == "text-align")
            setTextAlign(Fw::translateAlignment(node->value()));
        else if(node->tag() == "text-offset")
            setTextOffset(node->value<Point>());
        else if(node->tag() == "font")
            setFont(node->value());
        else if(node->tag() == "color")
            setForegroundColor(node->value<Color>());
        else if(node->tag() == "background-color")
            setBackgroundColor(node->value<Color>());
        else if(node->tag() == "opacity")
            setOpacity(node->value<int>());
        else if(node->tag() == "enabled")
            setEnabled(node->value<bool>());
        else if(node->tag() == "visible")
            setVisible(node->value<bool>());
        else if(node->tag() == "checked")
            setChecked(node->value<bool>());
        else if(node->tag() == "on")
            setOn(node->value<bool>());
        else if(node->tag() == "focusable")
            setFocusable(node->value<bool>());
        else if(node->tag() == "phantom")
            setPhantom(node->value<bool>());
        else if(node->tag() == "size")
            resize(node->value<Size>());
        else if(node->tag() == "width")
            setWidth(node->value<int>());
        else if(node->tag() == "height")
            setHeight(node->value<int>());
        else if(node->tag() == "fixed-size")
            setSizeFixed(node->value<bool>());
        else if(node->tag() == "pos")
            moveTo(node->value<Point>());
        else if(node->tag() == "x")
            setX(node->value<int>());
        else if(node->tag() == "y")
            setY(node->value<int>());
        else if(node->tag() == "margin-top")
            setMarginTop(node->value<int>());
        else if(node->tag() == "margin-right")
            setMarginRight(node->value<int>());
        else if(node->tag() == "margin-bottom")
            setMarginBottom(node->value<int>());
        else if(node->tag() == "margin-left")
            setMarginLeft(node->value<int>());
        else if(node->tag() == "margin") {
            std::string marginDesc = node->value();
            std::vector<std::string> split;
            boost::split(split, marginDesc, boost::is_any_of(std::string(" ")));
            if(split.size() == 4) {
                setMarginTop(Fw::safeCast<int>(split[0]));
                setMarginRight(Fw::safeCast<int>(split[1]));
                setMarginBottom(Fw::safeCast<int>(split[2]));
                setMarginLeft(Fw::safeCast<int>(split[3]));
            } else if(split.size() == 3) {
                int marginTop = Fw::safeCast<int>(split[0]);
                int marginHorizontal = Fw::safeCast<int>(split[1]);
                int marginBottom = Fw::safeCast<int>(split[2]);
                setMarginTop(marginTop);
                setMarginRight(marginHorizontal);
                setMarginBottom(marginBottom);
                setMarginLeft(marginHorizontal);
            } else if(split.size() == 2) {
                int marginVertical = Fw::safeCast<int>(split[0]);
                int marginHorizontal = Fw::safeCast<int>(split[1]);
                setMarginTop(marginVertical);
                setMarginRight(marginHorizontal);
                setMarginBottom(marginVertical);
                setMarginLeft(marginHorizontal);
            } else if(split.size() == 1) {
                int margin = Fw::safeCast<int>(split[0]);
                setMarginTop(margin);
                setMarginRight(margin);
                setMarginBottom(margin);
                setMarginLeft(margin);
            }
        }
        // layouts
        else if(node->tag() == "layout") {
            std::string layoutType;
            if(node->hasValue())
                layoutType = node->value();
            else
                layoutType = node->valueAt<std::string>("type", "");

            if(!layoutType.empty()) {
                UILayoutPtr layout;
                if(layoutType == "verticalBox")
                    layout = UIVerticalLayoutPtr(new UIVerticalLayout(asUIWidget()));
                else if(layoutType == "anchor")
                    layout = UIAnchorLayoutPtr(new UIAnchorLayout(asUIWidget()));
                else
                    throw OTMLException(node, "cannot determine layout type");
                setLayout(layout);
            }

            if(node->hasChildren())
                m_layout->applyStyle(node);
        }
        // anchors
        else if(boost::starts_with(node->tag(), "anchors.")) {
            UIWidgetPtr parent = getParent();
            if(!parent)
                throw OTMLException(node, "cannot create anchor, there is no parent widget!");

            UIAnchorLayoutPtr anchorLayout = parent->getLayout()->asUIAnchorLayout();
            if(!anchorLayout)
                throw OTMLException(node, "cannot create anchor, the parent widget doesn't use anchor layout!");

            std::string what = node->tag().substr(8);
            if(what == "fill") {
                anchorLayout->fill(asUIWidget(), node->value());
            } else if(what == "centerIn") {
                anchorLayout->centerIn(asUIWidget(), node->value());
            } else {
                Fw::AnchorEdge anchoredEdge = Fw::translateAnchorEdge(what);

                std::string anchorDescription = node->value();
                std::vector<std::string> split;
                boost::split(split, anchorDescription, boost::is_any_of(std::string(".")));
                if(split.size() != 2)
                    throw OTMLException(node, "invalid anchor description");

                std::string hookedWidgetId = split[0];
                Fw::AnchorEdge hookedEdge = Fw::translateAnchorEdge(split[1]);

                if(anchoredEdge == Fw::AnchorNone)
                    throw OTMLException(node, "invalid anchor edge");

                if(hookedEdge == Fw::AnchorNone)
                    throw OTMLException(node, "invalid anchor target edge");

                anchorLayout->addAnchor(asUIWidget(), anchoredEdge, hookedWidgetId, hookedEdge);
            }
        // lua functions
        } else if(boost::starts_with(node->tag(), "@")) {
            // load once
            if(m_firstOnStyle) {
                std::string funcName = node->tag().substr(1);
                std::string funcOrigin = "@" + node->source() + "[" + node->tag() + "]";
                g_lua.loadFunction(node->value(), funcOrigin);
                luaSetField(funcName);
            }
        // lua fields value
        } else if(boost::starts_with(node->tag(), "&")) {
            std::string fieldName = node->tag().substr(1);
            std::string fieldOrigin = "@" + node->source() + "[" + node->tag() + "]";
            g_lua.evaluateExpression(node->value(), fieldOrigin);
            luaSetField(fieldName);
        }
    }
}

void UIWidget::onGeometryChange(const Rect& oldRect, const Rect& newRect)
{
    callLuaField("onGeometryChange", oldRect, newRect);
}

void UIWidget::onFocusChange(bool focused, Fw::FocusReason reason)
{
    callLuaField("onFocusChange", focused, reason);
}

void UIWidget::onHoverChange(bool hovered)
{
    callLuaField("onHoverChange", hovered);

    // check for new hovered elements when the current widget is removed
    if(!hovered && !getParent() && g_ui.getRootWidget())
        g_ui.getRootWidget()->updateState(Fw::HoverState);
}


void UIWidget::onTextChange(const std::string& text)
{
    callLuaField("onTextChange", text);
}

void UIWidget::onFontChange(const std::string& font)
{
    callLuaField("onFontChange", font);
}

bool UIWidget::onKeyPress(uchar keyCode, std::string keyText, int keyboardModifiers)
{
    return callLuaField<bool>("onKeyPress", keyCode, keyText, keyboardModifiers);
}

bool UIWidget::onKeyRelease(uchar keyCode, std::string keyText, int keyboardModifiers)
{
    return callLuaField<bool>("onKeyRelease", keyCode, keyText, keyboardModifiers);
}

bool UIWidget::onMousePress(const Point& mousePos, Fw::MouseButton button)
{
    return callLuaField<bool>("onMousePress", mousePos, button);
}

void UIWidget::onMouseRelease(const Point& mousePos, Fw::MouseButton button)
{
    if(isPressed() && getRect().contains(mousePos))
        callLuaField("onClick");

    callLuaField("onMouseRelease", mousePos, button);
}

bool UIWidget::onMouseMove(const Point& mousePos, const Point& mouseMoved)
{
    return callLuaField<bool>("onMouseMove", mousePos, mouseMoved);
}

bool UIWidget::onMouseWheel(const Point& mousePos, Fw::MouseWheelDirection direction)
{
    return callLuaField<bool>("onMouseWheel", mousePos, direction);
}

bool UIWidget::propagateOnKeyPress(uchar keyCode, std::string keyText, int keyboardModifiers)
{
    // do a backup of children list, because it may change while looping it
    UIWidgetList children;
    for(const UIWidgetPtr& child : m_children) {
        // events on hidden or disabled widgets are discarded
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // key events go only to containers or focused child
        if(child->isFocused())
            children.push_back(child);
    }

    for(const UIWidgetPtr& child : children) {
        if(child->propagateOnKeyPress(keyCode, keyText, keyboardModifiers))
            return true;
    }

    return onKeyPress(keyCode, keyText, keyboardModifiers);
}

bool UIWidget::propagateOnKeyRelease(uchar keyCode, std::string keyText, int keyboardModifiers)
{
    // do a backup of children list, because it may change while looping it
    UIWidgetList children;
    for(const UIWidgetPtr& child : m_children) {
        // events on hidden or disabled widgets are discarded
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // key events go only to focused child
        if(child->isFocused())
            children.push_back(child);
    }

    for(const UIWidgetPtr& child : children) {
        if(child->propagateOnKeyRelease(keyCode, keyText, keyboardModifiers))
            return true;
    }

    return onKeyRelease(keyCode, keyText, keyboardModifiers);
}

bool UIWidget::propagateOnMousePress(const Point& mousePos, Fw::MouseButton button)
{
    // do a backup of children list, because it may change while looping it
    UIWidgetList children;
    for(const UIWidgetPtr& child : m_children) {
        // events on hidden or disabled widgets are discarded
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // mouse press events only go to children that contains the mouse position
        if(child->containsPoint(mousePos) && child == getChildByPos(mousePos))
            children.push_back(child);
    }

    for(const UIWidgetPtr& child : children) {
        // when a focusable item is focused it must gain focus
        if(child->isFocusable())
            focusChild(child, Fw::MouseFocusReason);

        bool mustEnd = child->propagateOnMousePress(mousePos, button);

        if(button == Fw::MouseLeftButton && !child->isPressed()) {
            UIWidgetPtr clickedChild = child->getChildByPos(mousePos);
            if(!clickedChild || clickedChild->isPhantom())
                child->setPressed(true);
        }

        if(mustEnd)
            return true;
    }

    if(!isPhantom())
        return onMousePress(mousePos, button);
    else
        return false;
}

void UIWidget::propagateOnMouseRelease(const Point& mousePos, Fw::MouseButton button)
{
    // do a backup of children list, because it may change while looping it
    UIWidgetList children;
    for(const UIWidgetPtr& child : m_children) {
        // events on hidden or disabled widgets are discarded
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // mouse release events go to all children
        children.push_back(child);
    }

    for(const UIWidgetPtr& child : children) {
        child->propagateOnMouseRelease(mousePos, button);

        if(child->isPressed() && button == Fw::MouseLeftButton)
            child->setPressed(false);
    }

    // fire release events only when pressed
    if(isPressed())
        onMouseRelease(mousePos, button);
}

bool UIWidget::propagateOnMouseMove(const Point& mousePos, const Point& mouseMoved)
{
    // do a backup of children list, because it may change while looping it
    UIWidgetList children;
    for(const UIWidgetPtr& child : m_children) {
        // events on hidden or disabled widgets are discarded
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // mouse move events go to all children
        children.push_back(child);
    }

    for(const UIWidgetPtr& child : children) {
        if(child->propagateOnMouseMove(mousePos, mouseMoved))
            return true;
    }

    if(!isPhantom())
        return onMouseMove(mousePos, mouseMoved);
    else
        return false;
}

bool UIWidget::propagateOnMouseWheel(const Point& mousePos, Fw::MouseWheelDirection direction)
{
    // do a backup of children list, because it may change while looping it
    UIWidgetList children;
    for(const UIWidgetPtr& child : m_children) {
        // events on hidden or disabled widgets are discarded
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // mouse wheel events only go to children that contains the mouse position
        if(child->containsPoint(mousePos) && child == getChildByPos(mousePos))
            children.push_back(child);
    }

    for(const UIWidgetPtr& child : children) {
        if(child->propagateOnMouseWheel(mousePos, direction))
            return true;
    }

    if(!isPhantom())
        return onMouseWheel(mousePos, direction);
    else
        return false;
}
