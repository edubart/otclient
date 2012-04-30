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
#include "uitranslator.h"

#include <framework/core/eventdispatcher.h>
#include <framework/otml/otmlnode.h>
#include <framework/graphics/graphics.h>
#include <framework/platform/platformwindow.h>
#include <framework/graphics/texturemanager.h>

UIWidget::UIWidget()
{
    m_lastFocusReason = Fw::ActiveFocusReason;
    m_states = Fw::DefaultState;
    m_clickTimer.stop();
    m_autoRepeatDelay = 500;

    initBaseStyle();
    initText();
    initImage();
}

UIWidget::~UIWidget()
{
#ifdef DEBUG
    if(!m_destroyed)
        logWarning("widget '", m_id, "' was not explicitly destroyed");
#endif
}

void UIWidget::draw(const Rect& visibleRect)
{
    if(m_clipping)
        g_painter->setClipRect(visibleRect);

    drawSelf();

    if(m_children.size() > 0) {
        if(m_clipping)
            g_painter->setClipRect(visibleRect.intersection(getClippingRect()));

        drawChildren(visibleRect);
    }

    if(m_clipping)
        g_painter->resetClipRect();
}

void UIWidget::drawSelf()
{
    // draw style components in order
    if(m_backgroundColor.aF() > Fw::MIN_ALPHA) {
        Rect backgroundDestRect = m_rect;
        backgroundDestRect.expand(-m_borderWidth.top, -m_borderWidth.right, -m_borderWidth.bottom, -m_borderWidth.left);
        drawBackground(m_rect);
    }

    drawImage(m_rect);
    drawBorder(m_rect);
    drawIcon(m_rect);
    drawText(m_rect);
}

void UIWidget::drawChildren(const Rect& visibleRect)
{
    // draw children
    for(const UIWidgetPtr& child : m_children) {
        // render only visible children with a valid rect inside parent rect
        if(!child->isExplicitlyVisible() || !child->getRect().isValid() || child->getOpacity() < Fw::MIN_ALPHA)
            continue;

        Rect childVisibleRect = visibleRect.intersection(child->getRect());
        if(!childVisibleRect.isValid())
            continue;

        // store current graphics opacity
        float oldOpacity = g_painter->getOpacity();

        // decrease to self opacity
        if(child->getOpacity() < oldOpacity)
            g_painter->setOpacity(child->getOpacity());

        child->draw(childVisibleRect);

        // debug draw box
        if(g_ui.isDrawingDebugBoxes()) {
            g_painter->setColor(Color::green);
            g_painter->drawBoundingRect(child->getRect());
        }
        //g_fonts.getDefaultFont()->renderText(child->getId(), child->getPosition() + Point(2, 0), Color::red);

        g_painter->setOpacity(oldOpacity);
    }
}

void UIWidget::addChild(const UIWidgetPtr& child)
{
    if(!child) {
        logWarning("attempt to add a null child into a UIWidget");
        return;
    }

    if(child->isDestroyed()) {
        logWarning("attemp to add a destroyed child into a UIWidget");
        return;
    }

    if(hasChild(child)) {
        logWarning("attempt to add a child again into a UIWidget");
        return;
    }

    UIWidgetPtr oldLastChild = getLastChild();

    m_children.push_back(child);
    child->setParent(asUIWidget());

    // create default layout
    if(!m_layout)
        m_layout = UIAnchorLayoutPtr(new UIAnchorLayout(asUIWidget()));

    // add to layout and updates it
    m_layout->addWidget(child);

    // update new child states
    child->updateStates();

    // update old child index states
    if(oldLastChild) {
        oldLastChild->updateState(Fw::MiddleState);
        oldLastChild->updateState(Fw::LastState);
    }

    g_ui.onWidgetAppear(child);
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

    if(!(index >= 0 && (uint)index <= m_children.size())) {
        logTraceError("attemp to insert a child in an invalid index");
        return;
    }

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

    g_ui.onWidgetAppear(child);
}

void UIWidget::removeChild(UIWidgetPtr child)
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

        g_ui.onWidgetDisappear(child);
    } else
        logError("attempt to remove an unknown child from a UIWidget");
}


void UIWidget::focusChild(const UIWidgetPtr& child, Fw::FocusReason reason)
{
    if(m_destroyed)
        return;

    if(child == m_focusedChild)
        return;

    if(child && !hasChild(child)) {
        logError("attempt to focus an unknown child in a UIWidget");
        return;
    }

    UIWidgetPtr oldFocused = m_focusedChild;
    m_focusedChild = child;

    if(child) {
        child->setLastFocusReason(reason);
        child->updateState(Fw::FocusState);
        child->updateState(Fw::ActiveState);

        child->onFocusChange(true, reason);
    }

    if(oldFocused) {
        oldFocused->setLastFocusReason(reason);
        oldFocused->updateState(Fw::FocusState);
        oldFocused->updateState(Fw::ActiveState);

        oldFocused->onFocusChange(false, reason);
    }

    onChildFocusChange(child, oldFocused, reason);
}

void UIWidget::focusNextChild(Fw::FocusReason reason)
{
    if(m_destroyed)
        return;

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
    if(m_destroyed)
        return;

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

void UIWidget::lowerChild(UIWidgetPtr child)
{
    if(m_destroyed)
        return;

    if(!child)
        return;

    // remove and push child again
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if(it == m_children.end()) {
        logTraceError("cannot find child");
        return;
    }

    m_children.erase(it);
    m_children.push_front(child);
    updateChildrenIndexStates();
}

void UIWidget::raiseChild(UIWidgetPtr child)
{
    if(m_destroyed)
        return;

    if(!child)
        return;

    // remove and push child again
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if(it == m_children.end()) {
        logTraceError("cannot find child");
        return;
    }
    m_children.erase(it);
    m_children.push_back(child);
    updateChildrenIndexStates();
}

void UIWidget::moveChildToIndex(const UIWidgetPtr& child, int index)
{
    if(m_destroyed)
        return;

    if(!child)
        return;

    // remove and push child again
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if(it == m_children.end()) {
        logTraceError("cannot find child");
        return;
    }
    m_children.erase(it);
    m_children.insert(m_children.begin() + index - 1, child);
    updateChildrenIndexStates();
}

void UIWidget::lockChild(const UIWidgetPtr& child)
{
    if(m_destroyed)
        return;

    if(!child)
        return;

    if(!hasChild(child)) {
        logTraceError("cannot find child");
        return;
    }

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
}

void UIWidget::unlockChild(const UIWidgetPtr& child)
{
    if(m_destroyed)
        return;

    if(!child)
        return;

    if(!hasChild(child)) {
        logTraceError("cannot find child");
        return;
    }

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
    }
}

void UIWidget::mergeStyle(const OTMLNodePtr& styleNode)
{
    applyStyle(styleNode);
    m_style->merge(styleNode);
    updateStyle();
}

void UIWidget::applyStyle(const OTMLNodePtr& styleNode)
{
    if(m_destroyed)
        return;

    if(styleNode->size() == 0)
        return;

    m_loadingStyle = true;
    try {
        // translate ! style tags
        for(const OTMLNodePtr& node : styleNode->children()) {
            if(node->tag()[0] == '!') {
                std::string tag = node->tag().substr(1);
                std::string code = Fw::formatString("tostring(%s)", node->value().c_str());
                std::string origin = "@" + node->source() + "[" + node->tag() + "]";
                g_lua.evaluateExpression(code, origin);
                std::string value = g_lua.popString();

                node->setTag(tag);
                node->setValue(value);
            }
        }

        onStyleApply(styleNode->tag(), styleNode);
        callLuaField("onStyleApply", styleNode->tag(), styleNode);

        if(m_firstOnStyle) {
            callLuaField("onSetup");
            // always focus new child
            if(isFocusable() && isExplicitlyVisible() && isExplicitlyEnabled())
                focus();
        }
        m_firstOnStyle = false;
    } catch(Exception& e) {
        logError("Failed to apply style to widget '", m_id, "' style: ", e.what());
    }
    m_loadingStyle = false;
}

void UIWidget::addAnchor(Fw::AnchorEdge anchoredEdge, const std::string& hookedWidgetId, Fw::AnchorEdge hookedEdge)
{
    if(m_destroyed)
        return;

    if(UIAnchorLayoutPtr anchorLayout = getAnchoredLayout())
        anchorLayout->addAnchor(asUIWidget(), anchoredEdge, hookedWidgetId, hookedEdge);
    else
        logError("cannot add anchors to widget ", m_id, ": the parent doesn't use anchors layout");
}

void UIWidget::removeAnchor(Fw::AnchorEdge anchoredEdge)
{
    addAnchor(anchoredEdge, "none", Fw::AnchorNone);
}

void UIWidget::centerIn(const std::string& hookedWidgetId)
{
    if(m_destroyed)
        return;

    if(UIAnchorLayoutPtr anchorLayout = getAnchoredLayout()) {
        anchorLayout->addAnchor(asUIWidget(), Fw::AnchorHorizontalCenter, hookedWidgetId, Fw::AnchorHorizontalCenter);
        anchorLayout->addAnchor(asUIWidget(), Fw::AnchorVerticalCenter, hookedWidgetId, Fw::AnchorVerticalCenter);
    } else
        logError("cannot add anchors to widget ", m_id, ": the parent doesn't use anchors layout");
}

void UIWidget::fill(const std::string& hookedWidgetId)
{
    if(m_destroyed)
        return;

    if(UIAnchorLayoutPtr anchorLayout = getAnchoredLayout()) {
        anchorLayout->addAnchor(asUIWidget(), Fw::AnchorLeft, hookedWidgetId, Fw::AnchorLeft);
        anchorLayout->addAnchor(asUIWidget(), Fw::AnchorRight, hookedWidgetId, Fw::AnchorRight);
        anchorLayout->addAnchor(asUIWidget(), Fw::AnchorTop, hookedWidgetId, Fw::AnchorTop);
        anchorLayout->addAnchor(asUIWidget(), Fw::AnchorBottom, hookedWidgetId, Fw::AnchorBottom);
    } else
        logError("cannot add anchors to widget ", m_id, ": the parent doesn't use anchors layout");
}

void UIWidget::breakAnchors()
{
    if(m_destroyed)
        return;

    if(UIAnchorLayoutPtr anchorLayout = getAnchoredLayout())
        anchorLayout->removeAnchors(asUIWidget());
}

void UIWidget::updateParentLayout()
{
    if(m_destroyed)
        return;

    if(UIWidgetPtr parent = getParent())
        parent->updateLayout();
    else
        updateLayout();
}

void UIWidget::updateLayout()
{
    if(m_destroyed)
        return;

    if(m_layout)
        m_layout->update();

    // children can affect the parent layout
    if(UIWidgetPtr parent = getParent())
        if(UILayoutPtr parentLayout = parent->getLayout())
            parentLayout->updateLater();
}

void UIWidget::lock()
{
    if(m_destroyed)
        return;

    if(UIWidgetPtr parent = getParent())
        parent->lockChild(asUIWidget());
}

void UIWidget::unlock()
{
    if(m_destroyed)
        return;

    if(UIWidgetPtr parent = getParent())
        parent->unlockChild(asUIWidget());
}

void UIWidget::focus()
{
    if(m_destroyed)
        return;

    if(!m_focusable)
        return;

    if(UIWidgetPtr parent = getParent())
        parent->focusChild(asUIWidget(), Fw::ActiveFocusReason);
}

void UIWidget::recursiveFocus(Fw::FocusReason reason)
{
    if(m_destroyed)
        return;

    if(UIWidgetPtr parent = getParent()) {
        if(m_focusable)
            parent->focusChild(asUIWidget(), reason);
        parent->recursiveFocus(reason);
    }
}

void UIWidget::lower()
{
    if(m_destroyed)
        return;

    UIWidgetPtr parent = getParent();
    if(parent)
        parent->lowerChild(asUIWidget());
}

void UIWidget::raise()
{
    if(m_destroyed)
        return;

    UIWidgetPtr parent = getParent();
    if(parent)
        parent->raiseChild(asUIWidget());
}

void UIWidget::grabMouse()
{
    if(m_destroyed)
        return;

    g_ui.setMouseReceiver(asUIWidget());
}

void UIWidget::ungrabMouse()
{
    if(g_ui.getMouseReceiver() == asUIWidget())
        g_ui.resetMouseReceiver();
}

void UIWidget::grabKeyboard()
{
    if(m_destroyed)
        return;

    g_ui.setKeyboardReceiver(asUIWidget());
}

void UIWidget::ungrabKeyboard()
{
    if(g_ui.getKeyboardReceiver() == asUIWidget())
        g_ui.resetKeyboardReceiver();
}

void UIWidget::bindRectToParent()
{
    if(m_destroyed)
        return;

    Rect boundRect = m_rect;
    UIWidgetPtr parent = getParent();
    if(parent) {
        Rect parentRect = parent->getClippingRect();
        boundRect.bind(parentRect);
    }

    setRect(boundRect);
}

void UIWidget::internalDestroy()
{
    m_destroyed = true;
    m_visible = false;
    m_enabled = false;
    m_parent.reset();
    m_focusedChild = nullptr;
    m_layout = nullptr;
    m_lockedChildren.clear();

    for(const UIWidgetPtr& child : m_children)
        child->internalDestroy();
    m_children.clear();

    callLuaField("onDestroy");

    releaseLuaFieldsTable();

    g_ui.onWidgetDestroy(asUIWidget());
}

void UIWidget::destroy()
{
    if(m_destroyed)
        logWarning("attempt to destroy widget '", m_id, "' two times");

    // hold itself reference
    UIWidgetPtr self = asUIWidget();
    m_destroyed = true;

    // remove itself from parent
    if(UIWidgetPtr parent = getParent())
        parent->removeChild(self);
    internalDestroy();
}

void UIWidget::destroyChildren()
{
    UILayoutPtr layout = getLayout();
    if(layout)
        layout->disableUpdates();

    m_focusedChild = nullptr;
    m_lockedChildren.clear();
    while(!m_children.empty()) {
        UIWidgetPtr child = m_children.front();
        m_children.pop_front();
        child->setParent(nullptr);
        m_layout->removeWidget(child);
        child->destroy();
    }

    layout->enableUpdates();
}

void UIWidget::setId(const std::string& id)
{
    m_id = id;
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

void UIWidget::setLayout(const UILayoutPtr& layout)
{
    if(m_layout)
        m_layout->disableUpdates();

    layout->setParent(asUIWidget());
    layout->disableUpdates();

    for(const UIWidgetPtr& child : m_children) {
        if(m_layout)
            m_layout->removeWidget(child);
        layout->addWidget(child);
    }

    if(m_layout) {
        m_layout->enableUpdates();
        m_layout->setParent(nullptr);
        m_layout->update();
    }

    layout->enableUpdates();
    m_layout = layout;
}

bool UIWidget::setRect(const Rect& rect)
{
    if(rect.width() > 8192 || rect.height() > 8192) {
        logError("attempt to set huge rect size (", rect,") for ", m_id);
        return false;
    }
    // only update if the rect really changed
    Rect oldRect = m_rect;
    if(rect == oldRect)
        return false;

    m_rect = rect;

    // updates own layout
    updateLayout();

    // avoid massive update events
    if(!m_updateEventScheduled) {
        UIWidgetPtr self = asUIWidget();
        g_eventDispatcher.addEvent([self, oldRect]() {
            self->m_updateEventScheduled = false;
            if(oldRect != self->getRect())
                self->onGeometryChange(oldRect, self->getRect());
        });
        m_updateEventScheduled = true;
    }

    return true;
}

void UIWidget::setStyle(const std::string& styleName)
{
    OTMLNodePtr styleNode = g_ui.getStyle(styleName);
    if(!styleNode) {
        logTraceError("unable to retrive style '", styleName, "': not a defined style");
        return;
    }
    styleNode = styleNode->clone();
    applyStyle(styleNode);
    m_style = styleNode;
    updateStyle();
}

void UIWidget::setStyleFromNode(const OTMLNodePtr& styleNode)
{
    applyStyle(styleNode);
    m_style = styleNode;
    updateStyle();
}

void UIWidget::setEnabled(bool enabled)
{
    if(enabled != m_enabled) {
        m_enabled = enabled;

        updateState(Fw::DisabledState);
        updateState(Fw::ActiveState);
    }
}

void UIWidget::setVisible(bool visible)
{
    if(m_visible != visible) {
        m_visible = visible;

        // hiding a widget make it lose focus
        if(!visible && isFocused()) {
            if(UIWidgetPtr parent = getParent())
                parent->focusPreviousChild(Fw::ActiveFocusReason);
        }

        // visibility can change change parent layout
        updateParentLayout();

        updateState(Fw::ActiveState);

        // visibility can change the current hovered widget
        if(visible)
            g_ui.onWidgetAppear(asUIWidget());
        else
            g_ui.onWidgetDisappear(asUIWidget());
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

void UIWidget::setPhantom(bool phantom)
{
    m_phantom = phantom;
}

void UIWidget::setDragable(bool dragable)
{
    m_dragable = dragable;
}

void UIWidget::setFixedSize(bool fixed)
{
    m_fixedSize = fixed;
    updateParentLayout();
}

void UIWidget::setLastFocusReason(Fw::FocusReason reason)
{
    m_lastFocusReason = reason;
}

void UIWidget::setVirtualOffset(const Point& offset)
{
    m_virtualOffset = offset;
    if(m_layout)
        m_layout->update();
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

bool UIWidget::isAnchored()
{
    if(UIWidgetPtr parent = getParent())
        if(UIAnchorLayoutPtr anchorLayout = parent->getAnchoredLayout())
            return anchorLayout->hasAnchors(asUIWidget());
    return false;
}

bool UIWidget::isChildLocked(const UIWidgetPtr& child)
{
    auto it = std::find(m_lockedChildren.begin(), m_lockedChildren.end(), child);
    return it != m_lockedChildren.end();
}

bool UIWidget::hasChild(const UIWidgetPtr& child)
{
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if(it != m_children.end())
        return true;
    return false;
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

Rect UIWidget::getClippingRect()
{
    Rect rect = m_rect;
    rect.expand(-m_padding.top, -m_padding.right, -m_padding.bottom, -m_padding.left);
    return rect;
}

Rect UIWidget::getMarginRect()
{
    Rect rect = m_rect;
    rect.expand(m_margin.top, m_margin.right, m_margin.bottom, m_margin.left);
    return rect;
}

Rect UIWidget::getChildrenRect()
{
    Rect childrenRect;
    for(const UIWidgetPtr& child : m_children) {
        if(!child->isExplicitlyVisible() || !child->getRect().isValid())
            continue;
        Rect marginRect = child->getMarginRect();
        if(!childrenRect.isValid())
            childrenRect = marginRect;
        else
            childrenRect = childrenRect.united(marginRect);
    }

    Rect myClippingRect = getClippingRect();
    if(!childrenRect.isValid())
        childrenRect = myClippingRect;
    else {
        if(childrenRect.width() < myClippingRect.width())
            childrenRect.setWidth(myClippingRect.width());
        if(childrenRect.height() < myClippingRect.height())
            childrenRect.setHeight(myClippingRect.height());
    }
    return childrenRect;
}

UIAnchorLayoutPtr UIWidget::getAnchoredLayout()
{
    UIWidgetPtr parent = getParent();
    if(!parent)
        return nullptr;

    return parent->getLayout()->asUIAnchorLayout();
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
    if(!containsChildPoint(childPos))
        return nullptr;

    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        const UIWidgetPtr& child = (*it);
        if(child->isExplicitlyVisible() && child->containsPoint(childPos))
            return child;
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

UIWidgetPtr UIWidget::recursiveGetChildByPos(const Point& childPos, bool wantsPhantom)
{
    if(!containsChildPoint(childPos))
        return nullptr;

    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        const UIWidgetPtr& child = (*it);
        if(child->isExplicitlyVisible() && child->containsPoint(childPos)) {
            UIWidgetPtr subChild = child->recursiveGetChildByPos(childPos, wantsPhantom);
            if(subChild)
                return subChild;
            else if(wantsPhantom || !child->isPhantom())
                return child;
        }
    }
    return nullptr;
}

UIWidgetList UIWidget::recursiveGetChildrenByPos(const Point& childPos)
{
    UIWidgetList children;
    if(!containsChildPoint(childPos))
        return children;

    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        const UIWidgetPtr& child = (*it);
        if(child->isExplicitlyVisible() && child->containsPoint(childPos)) {
            UIWidgetList subChildren = child->recursiveGetChildrenByPos(childPos);
            if(!subChildren.empty())
                children.insert(children.end(), subChildren.begin(), subChildren.end());
            children.push_back(child);
        }
    }
    return children;
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
    if(m_destroyed)
        return;

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

            updateChildren = newStatus != oldStatus;
            break;
        }
        case Fw::FocusState: {
            newStatus = (getParent() && getParent()->getFocusedChild() == asUIWidget());
            break;
        }
        case Fw::HoverState: {
            newStatus = (g_ui.getHoveredWidget() == asUIWidget() && isEnabled());
            break;
        }
        case Fw::PressedState: {
            newStatus = (g_ui.getPressedWidget() == asUIWidget());
            break;
        }
        case Fw::DraggingState: {
            newStatus = (g_ui.getDraggingWidget() == asUIWidget());
            break;
        }
        case Fw::DisabledState: {
            bool enabled = true;
            UIWidgetPtr widget = asUIWidget();
            do {
                if(!widget->isExplicitlyEnabled()) {
                    enabled = false;
                    break;
                }
            } while(widget = widget->getParent());
            newStatus = !enabled;
            updateChildren = newStatus != oldStatus;
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
        // disabled widgets cannot have hover state
        if(state == Fw::DisabledState && !newStatus && isHovered()) {
            g_ui.updateHoveredWidget();
        }
    }
}

void UIWidget::updateStates()
{
    if(m_destroyed)
        return;

    for(int state = 1; state != Fw::LastWidgetState; state <<= 1)
        updateState((Fw::WidgetState)state);
}

void UIWidget::updateChildrenIndexStates()
{
    if(m_destroyed)
        return;

    for(const UIWidgetPtr& child : m_children) {
        child->updateState(Fw::FirstState);
        child->updateState(Fw::MiddleState);
        child->updateState(Fw::LastState);
        child->updateState(Fw::AlternateState);
    }
}

void UIWidget::updateStyle()
{
    if(m_destroyed)
        return;

    if(m_loadingStyle && !m_updateStyleScheduled) {
        UIWidgetPtr self = asUIWidget();
        g_eventDispatcher.addEvent([self] {
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
    if(m_destroyed)
        return;

    // first set id
    if(const OTMLNodePtr& node = styleNode->get("id"))
        setId(node->value());

    parseBaseStyle(styleNode);
    parseImageStyle(styleNode);
    parseTextStyle(styleNode);
}

void UIWidget::onGeometryChange(const Rect& oldRect, const Rect& newRect)
{
    if(m_textWrap && oldRect.size() != newRect.size())
        updateText();

    // move children that is outside the parent rect to inside again
    for(const UIWidgetPtr& child : m_children) {
        if(!child->isAnchored())
            child->bindRectToParent();
    }

    callLuaField("onGeometryChange", oldRect, newRect);
}

void UIWidget::onLayoutUpdate()
{
    callLuaField("onLayoutUpdate");
}

void UIWidget::onFocusChange(bool focused, Fw::FocusReason reason)
{
    callLuaField("onFocusChange", focused, reason);
}

void UIWidget::onChildFocusChange(const UIWidgetPtr& focusedChild, const UIWidgetPtr& unfocusedChild, Fw::FocusReason reason)
{
    callLuaField("onChildFocusChange", focusedChild, unfocusedChild, reason);
}

void UIWidget::onHoverChange(bool hovered)
{
    callLuaField("onHoverChange", hovered);
}

bool UIWidget::onDragEnter(const Point& mousePos)
{
    return callLuaField<bool>("onDragEnter", mousePos);
}

bool UIWidget::onDragLeave(UIWidgetPtr droppedWidget, const Point& mousePos)
{
    return callLuaField<bool>("onDragLeave", droppedWidget, mousePos);
}

bool UIWidget::onDragMove(const Point& mousePos, const Point& mouseMoved)
{
    return callLuaField<bool>("onDragMove", mousePos, mouseMoved);
}

bool UIWidget::onDrop(UIWidgetPtr draggedWidget, const Point& mousePos)
{
    return callLuaField<bool>("onDrop", draggedWidget, mousePos);
}

bool UIWidget::onKeyText(const std::string& keyText)
{
    return callLuaField<bool>("onKeyText", keyText);
}

bool UIWidget::onKeyDown(uchar keyCode, int keyboardModifiers)
{
    return callLuaField<bool>("onKeyDown", keyCode, keyboardModifiers);
}

bool UIWidget::onKeyPress(uchar keyCode, int keyboardModifiers, int autoRepeatTicks)
{
    return callLuaField<bool>("onKeyPress", keyCode, keyboardModifiers, autoRepeatTicks);
}

bool UIWidget::onKeyUp(uchar keyCode, int keyboardModifiers)
{
    return callLuaField<bool>("onKeyUp", keyCode, keyboardModifiers);
}

bool UIWidget::onMousePress(const Point& mousePos, Fw::MouseButton button)
{
    if(button == Fw::MouseLeftButton) {
        if(m_clickTimer.running() && m_clickTimer.ticksElapsed() <= 500) {
            if(onDoubleClick(mousePos))
                return true;
            m_clickTimer.stop();
        } else
            m_clickTimer.restart();
        m_lastClickPosition = mousePos;
    }

    if(hasLuaField("onMousePress"))
        return callLuaField<bool>("onMousePress", mousePos, button);

    return false;
}

bool UIWidget::onMouseRelease(const Point& mousePos, Fw::MouseButton button)
{
    return callLuaField<bool>("onMouseRelease", mousePos, button);
}

bool UIWidget::onMouseMove(const Point& mousePos, const Point& mouseMoved)
{
    return callLuaField<bool>("onMouseMove", mousePos, mouseMoved);
}

bool UIWidget::onMouseWheel(const Point& mousePos, Fw::MouseWheelDirection direction)
{
    return callLuaField<bool>("onMouseWheel", mousePos, direction);
}

bool UIWidget::onClick(const Point& mousePos)
{
    if(hasLuaField("onClick")) {
        callLuaField("onClick", mousePos);
        return true;
    }
    return false;
}

bool UIWidget::onDoubleClick(const Point& mousePos)
{
    if(hasLuaField("onDoubleClick")) {
        callLuaField("onDoubleClick", mousePos);
        return true;
    }
    return false;
}

bool UIWidget::propagateOnKeyText(const std::string& keyText)
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
        if(child->propagateOnKeyText(keyText))
            return true;
    }

    return onKeyText(keyText);
}

bool UIWidget::propagateOnKeyDown(uchar keyCode, int keyboardModifiers)
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
        if(child->propagateOnKeyDown(keyCode, keyboardModifiers))
            return true;
    }

    return onKeyDown(keyCode, keyboardModifiers);
}

bool UIWidget::propagateOnKeyPress(uchar keyCode, int keyboardModifiers, int autoRepeatTicks)
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
        if(child->propagateOnKeyPress(keyCode, keyboardModifiers, autoRepeatTicks))
            return true;
    }

    if(autoRepeatTicks == 0 || autoRepeatTicks >= m_autoRepeatDelay)
        return onKeyPress(keyCode, keyboardModifiers, autoRepeatTicks);
    else
        return false;
}

bool UIWidget::propagateOnKeyUp(uchar keyCode, int keyboardModifiers)
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
        if(child->propagateOnKeyUp(keyCode, keyboardModifiers))
            return true;
    }

    return onKeyUp(keyCode, keyboardModifiers);
}

bool UIWidget::propagateOnMouseEvent(const Point& mousePos, UIWidgetList& widgetList)
{
    bool ret = false;
    if(containsChildPoint(mousePos)) {
        for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
            const UIWidgetPtr& child = *it;
            if(child->isExplicitlyEnabled() && child->isExplicitlyVisible() && child->containsPoint(mousePos)) {
                if(child->propagateOnMouseEvent(mousePos, widgetList)) {
                    ret = true;
                    break;
                }
            }
        }
    }

    widgetList.push_back(asUIWidget());

    if(!isPhantom())
        ret = true;
    return ret;
}

bool UIWidget::propagateOnMouseMove(const Point& mousePos, const Point& mouseMoved, UIWidgetList& widgetList)
{
    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        const UIWidgetPtr& child = *it;
        if(child->isExplicitlyVisible() && child->isExplicitlyEnabled())
            child->propagateOnMouseMove(mousePos, mouseMoved, widgetList);
    }

    widgetList.push_back(asUIWidget());
    return true;
}
