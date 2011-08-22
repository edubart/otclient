#include "uiwidget.h"
#include "uimanager.h"

#include <framework/core/eventdispatcher.h>
#include <framework/graphics/image.h>
#include <framework/graphics/borderimage.h>
#include <framework/graphics/fontmanager.h>
#include <framework/otml/otmlnode.h>
#include <framework/graphics/graphics.h>
#include "uianchor.h"

UIWidget::UIWidget()
{
    m_visible = true;
    m_enabled = true;
    m_hovered = false;
    m_focusable = true;
    m_destroyed = false;
    m_layoutUpdated = true;
    m_updateEventScheduled = false;
    m_layoutUpdateScheduled = false;
    m_childrenLayoutUpdateScheduled = false;
    m_opacity = 255;
    m_marginLeft = m_marginRight = m_marginTop = m_marginBottom = 0;
    m_backgroundColor = Color::white;
    m_foregroundColor = Color::white;

    // generate an unique id, this is need because anchored layouts find widgets by id
    static unsigned long id = 1;
    m_id = fw::mkstr("widget", id++);
}

UIWidget::~UIWidget()
{
    if(!m_destroyed)
        logWarning("widget '", m_id, "' was destructed without being explicit destroyed");
}

void UIWidget::destroy()
{
    // destroy only once
    if(!m_destroyed) {
        // first release lua table, because it may contains references to children
        releaseLuaFieldsTable();

        // clear additional child references
        m_lockedChildren.clear();
        m_focusedChild.reset();
        m_anchors.clear();
        m_anchoredWidgets.clear();

        // destroy children
        while(m_children.size() > 0) {
            UIWidgetPtr child = m_children.front(); //hold reference
            child->destroy();
        }

        // remove itself from parent
        if(UIWidgetPtr parent = getParent())
            parent->removeChild(asUIWidget());

        // by now this widget is destroyed
        m_destroyed = true;

        // add destroy check event
        g_dispatcher.addEvent(std::bind(&UIWidget::destroyCheck, asUIWidget()));
    } else
        logWarning("attempt to destroy widget '", m_id, "' again");
}

void UIWidget::destroyCheck()
{
    // collect lua garbage before checking
    g_lua.collectGarbage();

    // get real use count
    int realUseCount = shared_from_this().use_count() - 2;

    // check for leaks upon widget destruction
    if(realUseCount > 0)
        logWarning("destroyed widget with id '",m_id,"', but it still have ",realUseCount," references left");
}

void UIWidget::render()
{
    assert(!m_destroyed);

    // draw background image
    g_graphics.bindColor(m_backgroundColor);
    if(m_image)
        m_image->draw(getRect());

    // draw children
    for(const UIWidgetPtr& child : m_children) {
        if(child->isExplicitlyVisible()) {
            // store current graphics opacity
            int oldOpacity = g_graphics.getOpacity();

            // decrease to self opacity
            if(child->getOpacity() < oldOpacity)
                g_graphics.setOpacity(child->getOpacity());

            // bind child color
            child->render();

            // debug draw box
            //g_graphics.bindColor(Color::green);
            //g_graphics.drawBoundingRect(child->getRect());

            g_graphics.setOpacity(oldOpacity);
        }
    }
}

void UIWidget::setParent(const UIWidgetPtr& parent)
{
    assert(!m_destroyed);

    UIWidgetPtr self = asUIWidget();

    // remove from old parent
    UIWidgetPtr oldParent = m_parent.lock();
    if(oldParent && oldParent->hasChild(self))
        oldParent->removeChild(self);

    // reset parent
    m_parent.reset();

    // set new parent
    if(parent) {
        m_parent = UIWidgetWeakPtr(parent);

        // add to parent if needed
        if(!parent->hasChild(self))
            parent->addChild(self);
    }
}

void UIWidget::applyStyle(const std::string& styleName)
{
    try {
        OTMLNodePtr styleNode = g_ui.getStyle(styleName);
        onStyleApply(styleNode);

        // forces layout recalculation
        updateLayout();
    } catch(std::exception& e) {
        logError("couldn't change widget '", m_id, "' style: ", e.what());
    }
}

void UIWidget::setRect(const Rect& rect)
{
    assert(!m_destroyed);

    Rect oldRect = m_rect;
    m_rect = rect;

    // updates children geometry
    updateChildrenLayout();

    // avoid massive update events
    if(!m_updateEventScheduled) {
        UIWidgetPtr self = asUIWidget();
        g_dispatcher.addEvent([self, oldRect]() {
            self->m_updateEventScheduled = false;
            // this widget could be destroyed before the event happens
            if(!self->isDestroyed())
                self->onGeometryUpdate(oldRect, self->getRect());
        });
        m_updateEventScheduled = true;
    }
}

bool UIWidget::isEnabled()
{
    if(!m_enabled)
        return false;
    else if(UIWidgetPtr parent = getParent())
        return parent->isEnabled();
    else
        return false;
}

bool UIWidget::isVisible()
{
    if(!m_visible)
        return false;
    else if(UIWidgetPtr parent = getParent())
        return parent->isVisible();
    else
        return false;
}

bool UIWidget::hasFocus()
{
    assert(!m_destroyed);

    if(UIWidgetPtr parent = getParent()) {
        if(parent->hasFocus() && parent->getFocusedChild() == shared_from_this())
            return true;
    }
    // root parent always has focus
    else if(asUIWidget() == getRootParent())
        return true;
    return false;
}

bool UIWidget::hasChild(const UIWidgetPtr& child)
{
    assert(!m_destroyed);

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
    assert(!m_destroyed);

    auto it = std::find(m_children.begin(), m_children.end(), relativeChild);
    if(it != m_children.end() && ++it != m_children.end())
        return *it;
    return nullptr;
}

UIWidgetPtr UIWidget::getChildBefore(const UIWidgetPtr& relativeChild)
{
    assert(!m_destroyed);

    auto it = std::find(m_children.rbegin(), m_children.rend(), relativeChild);
    if(it != m_children.rend() && ++it != m_children.rend())
        return *it;
    return nullptr;
}

UIWidgetPtr UIWidget::getChildById(const std::string& childId)
{
    assert(!m_destroyed);

    if(getId() == childId || childId == "self")
        return asUIWidget();
    else if(childId == "parent")
        return getParent();
    else if(childId == "root")
        return getRootParent();
    else if(childId == "prev") {
        if(UIWidgetPtr parent = getParent())
            return parent->getChildBefore(asUIWidget());
    } else if(childId == "next") {
        if(UIWidgetPtr parent = getParent())
            return parent->getChildAfter(asUIWidget());
    } else {
        for(const UIWidgetPtr& child : m_children) {
            if(child->getId() == childId)
                return child;
        }
    }
    return nullptr;
}

UIWidgetPtr UIWidget::getChildByPos(const Point& childPos)
{
    assert(!m_destroyed);

    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        const UIWidgetPtr& widget = (*it);
        if(widget->isExplicitlyVisible() && widget->getRect().contains(childPos))
            return widget;
    }

    return nullptr;
}

UIWidgetPtr UIWidget::getChildByIndex(int childIndex)
{
    assert(!m_destroyed);

    if(childIndex >= 0 && childIndex < getChildCount())
        return m_children.at(childIndex);
    return nullptr;
}

UIWidgetPtr UIWidget::recursiveGetChildById(const std::string& id)
{
    assert(!m_destroyed);

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
    assert(!m_destroyed);

    for(const UIWidgetPtr& child : m_children) {
        if(child->getRect().contains(childPos)) {
            if(UIWidgetPtr subChild = child->recursiveGetChildByPos(childPos))
                return subChild;
            return child;
        }
    }
    return nullptr;
}

UIWidgetPtr UIWidget::backwardsGetWidgetById(const std::string& id)
{
    assert(!m_destroyed);

    UIWidgetPtr widget = getChildById(id);
    if(!widget) {
        if(UIWidgetPtr parent = getParent())
            widget = parent->backwardsGetWidgetById(id);
    }
    return widget;
}

void UIWidget::focusChild(const UIWidgetPtr& child, UI::FocusReason reason)
{
    assert(!m_destroyed);

    if(!child)
        return;

    assert(hasChild(child));

    if(child != m_focusedChild) {
        UIWidgetPtr oldFocused = m_focusedChild;
        m_focusedChild = child;

        if(oldFocused)
            oldFocused->onFocusChange(false, reason);

        if(child)
            child->onFocusChange(child->hasFocus(), reason);
    }
}

void UIWidget::addChild(const UIWidgetPtr& child)
{
    assert(!m_destroyed);

    if(!child)
        return;

    assert(!hasChild(child));

    m_children.push_back(child);
    child->setParent(asUIWidget());

    // recalculate anchors
    getRootParent()->recalculateAnchoredWidgets();

    // may need to update children layout
    updateChildrenLayout();

    // always focus new children
    if(child->isFocusable() && child->isExplicitlyVisible() && child->isExplicitlyEnabled())
        focusChild(child, UI::ActiveFocusReason);
}

void UIWidget::insertChild(const UIWidgetPtr& child, int index)
{
    assert(!m_destroyed);

    // skip null children
    if(!child)
        return;

    assert(!hasChild(child));

    if(index < 0)
        index = m_children.size() + index -1;

    assert((uint)index <= m_children.size());

    // retrieve child by index
    auto it = m_children.begin() + index;
    m_children.insert(it, child);
    child->setParent(asUIWidget());

    // recalculate anchors
    getRootParent()->recalculateAnchoredWidgets();

    // may need to update children layout
    updateChildrenLayout();
}

void UIWidget::removeChild(const UIWidgetPtr& child)
{
    assert(!m_destroyed);

    // skip null children
    if(!child)
        return;

    // defocus if needed
    if(m_focusedChild == child)
        focusChild(nullptr, UI::ActiveFocusReason);

    // unlock child if it was locked
    unlockChild(child);

    // remove from children list
    auto it = std::find(m_children.begin(), m_children.end(), child);
    assert(it != m_children.end());
    m_children.erase(it);

    // reset child parent
    assert(child->getParent() == asUIWidget());
    child->setParent(nullptr);

    // recalculate anchors
    getRootParent()->recalculateAnchoredWidgets();

    // may need to update children layout
    updateChildrenLayout();
}

void UIWidget::focusNextChild(UI::FocusReason reason)
{
    assert(!m_destroyed);

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
        if(child->isFocusable()) {
            toFocus = child;
            break;
        }
    }

    if(toFocus)
        focusChild(toFocus, reason);
}

void UIWidget::moveChildToTop(const UIWidgetPtr& child)
{
    assert(!m_destroyed);

    if(!child)
        return;

    // remove and push child again
    auto it = std::find(m_children.begin(), m_children.end(), child);
    assert(it != m_children.end());
    m_children.erase(it);
    m_children.push_back(child);
}

void UIWidget::lockChild(const UIWidgetPtr& child)
{
    assert(!m_destroyed);

    if(!child)
        return;

    assert(hasChild(child));

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
      focusChild(child, UI::ActiveFocusReason);
}

void UIWidget::unlockChild(const UIWidgetPtr& child)
{
    assert(!m_destroyed);

    if(!child)
        return;

    assert(hasChild(child));

    auto it = std::find(m_lockedChildren.begin(), m_lockedChildren.end(), child);
    if(it == m_lockedChildren.end())
        return;

    m_lockedChildren.erase(it);

    // find new chick to lock
    UIWidgetPtr lockedChild;
    if(m_lockedChildren.size() > 0)
        lockedChild = m_lockedChildren.front();

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
}

void UIWidget::updateLayout()
{
    assert(!m_destroyed);

    if(!m_layoutUpdateScheduled) {
        m_layoutUpdateScheduled = true;
        UIWidgetPtr self = asUIWidget();
        g_dispatcher.addEvent([self] {
            self->m_layoutUpdateScheduled = false;
            if(!self->isDestroyed())
                self->internalUpdateLayout();
        });
    }
}

void UIWidget::updateChildrenLayout()
{
    assert(!m_destroyed);

    if(!m_childrenLayoutUpdateScheduled) {
        m_childrenLayoutUpdateScheduled = true;
        UIWidgetPtr self = asUIWidget();
        g_dispatcher.addEvent([self] {
            self->m_childrenLayoutUpdateScheduled = false;
            if(!self->isDestroyed())
                self->internalUpdateChildrenLayout();
        });
    }
}

bool UIWidget::addAnchor(AnchorEdge edge, const std::string& hookedWidgetId, AnchorEdge hookedEdge)
{
    assert(!m_destroyed);

    UIAnchor anchor(edge, hookedWidgetId, hookedEdge);

    UIWidgetPtr hookedWidget = backwardsGetWidgetById(hookedWidgetId);
    anchor.setHookedWidget(hookedWidget);

    // we can never anchor with itself
    if(hookedWidget == asUIWidget()) {
        logError("anchoring with itself is not possible");
        return false;
    }

    // we must never anchor to an anchor child
    //TODO: this check

    // duplicated anchors must be replaced
    for(auto it = m_anchors.begin(); it != m_anchors.end(); ++it) {
        const UIAnchor& otherAnchor = *it;
        if(otherAnchor.getAnchoredEdge() == edge) {
            m_anchors.erase(it);
            break;
        }
    }

    m_anchors.push_back(anchor);

    updateLayout();
    return true;
}

void UIWidget::centerIn(const std::string& hookedWidgetId)
{
    assert(!m_destroyed);

    addAnchor(AnchorHorizontalCenter, hookedWidgetId, AnchorHorizontalCenter);
    addAnchor(AnchorVerticalCenter, hookedWidgetId, AnchorVerticalCenter);
}

void UIWidget::fill(const std::string& hookedWidgetId)
{
    assert(!m_destroyed);

    addAnchor(AnchorLeft, hookedWidgetId, AnchorLeft);
    addAnchor(AnchorRight, hookedWidgetId, AnchorRight);
    addAnchor(AnchorTop, hookedWidgetId, AnchorTop);
    addAnchor(AnchorBottom, hookedWidgetId, AnchorBottom);
}

void UIWidget::internalUpdateLayout()
{
    assert(!m_destroyed);

    for(const UIAnchor& anchor : m_anchors) {
        // ignore invalid anchors
        if(!anchor.getHookedWidget())
            continue;

        // the update should only happens if the hooked widget is already updated
        if(!anchor.getHookedWidget()->m_layoutUpdated)
            return;
    }

    Rect newRect = m_rect;
    bool verticalMoved = false;
    bool horizontalMoved = false;

    // calculate new rect based on anchors
    for(const UIAnchor& anchor : m_anchors) {
        int point = anchor.getHookedPoint();

        // ignore invalid anchors
        if(point == UIAnchor::INVALID_POINT)
            continue;

        switch(anchor.getAnchoredEdge()) {
            case AnchorHorizontalCenter:
                newRect.moveHorizontalCenter(point + getMarginLeft() - getMarginRight());
                horizontalMoved = true;
                break;
            case AnchorLeft:
                if(!horizontalMoved) {
                    newRect.moveLeft(point + getMarginLeft());
                    horizontalMoved = true;
                } else
                    newRect.setLeft(point + getMarginLeft());
                break;
            case AnchorRight:
                if(!horizontalMoved) {
                    newRect.moveRight(point - getMarginRight());
                    horizontalMoved = true;
                } else
                    newRect.setRight(point - getMarginRight());
                break;
            case AnchorVerticalCenter:
                newRect.moveVerticalCenter(point + getMarginTop() - getMarginBottom());
                verticalMoved = true;
                break;
            case AnchorTop:
                if(!verticalMoved) {
                    newRect.moveTop(point + getMarginTop());
                    verticalMoved = true;
                } else
                    newRect.setTop(point + getMarginTop());
                break;
            case AnchorBottom:
                if(!verticalMoved) {
                    newRect.moveBottom(point - getMarginBottom());
                    verticalMoved = true;
                } else
                    newRect.setBottom(point - getMarginBottom());
                break;
            default:
                break;
        }
    }

    m_layoutUpdated = true;

    // changes the rect only if really needed
    if(newRect != m_rect) {
        // setRect will update children layout too
        setRect(newRect);
    } else {
        // update children
        internalUpdateChildrenLayout();
    }
}

void UIWidget::internalUpdateChildrenLayout()
{
    assert(!m_destroyed);

    // reset all children anchors update state
    resetLayoutUpdateState(false);

    // update children layouts
    for(const UIWidgetWeakPtr& anchoredWidgetWeak : m_anchoredWidgets) {
        if(UIWidgetPtr anchoredWidget = anchoredWidgetWeak.lock())
            anchoredWidget->updateLayout();
    }
}

void UIWidget::resetLayoutUpdateState(bool resetOwn)
{
    if(resetOwn)
        m_layoutUpdated = false;

    // resets children layout update state too
    for(const UIWidgetWeakPtr& anchoredWidgetWeak : m_anchoredWidgets) {
        if(UIWidgetPtr anchoredWidget = anchoredWidgetWeak.lock())
            anchoredWidget->resetLayoutUpdateState(true);
    }
}

void UIWidget::addAnchoredWidget(const UIWidgetPtr& widget)
{
    // prevent duplicated anchored widgets
    for(const UIWidgetWeakPtr& anchoredWidget : m_anchoredWidgets)
        if(anchoredWidget.lock() == widget)
            return;
    m_anchoredWidgets.push_back(widget);
}

void UIWidget::recalculateAnchoredWidgets()
{
    clearAnchoredWidgets();
    computeAnchoredWidgets();
}

void UIWidget::clearAnchoredWidgets()
{
    m_anchoredWidgets.clear();
    for(const UIWidgetPtr& child : m_children)
        child->clearAnchoredWidgets();
}

void UIWidget::computeAnchoredWidgets()
{
    // update anchors's hooked widget
    for(UIAnchor& anchor : m_anchors) {
        UIWidgetPtr hookedWidget = backwardsGetWidgetById(anchor.getHookedWidgetId());
        anchor.setHookedWidget(hookedWidget);
        if(hookedWidget)
            hookedWidget->addAnchoredWidget(asUIWidget());
    }

    for(const UIWidgetPtr& child : m_children)
        child->computeAnchoredWidgets();
}

void UIWidget::onStyleApply(const OTMLNodePtr& styleNode)
{
    assert(!m_destroyed);

    // load styles used by all widgets
    for(const OTMLNodePtr& node : styleNode->children()) {
        // id
        if(node->tag() == "id") {
            setId(node->value());
        }
        // background image
        else if(node->tag() == "image") {
            setImage(Image::loadFromOTML(node));
        }
        else if(node->tag() == "border-image") {
            setImage(BorderImage::loadFromOTML(node));
        }
        // font
        else if(node->tag() == "font") {
            setFont(g_fonts.getFont(node->value()));
        }
        // foreground color
        else if(node->tag() == "color") {
            setForegroundColor(node->value<Color>());
        }
        // background color
        else if(node->tag() == "background-color") {
            setBackgroundColor(node->value<Color>());
        }
        // opacity
        else if(node->tag() == "opacity") {
            setOpacity(node->value<int>());
        }
        // size
        else if(node->tag() == "size") {
            resize(node->value<Size>());
        }
        else if(node->tag() == "width") {
            setWidth(node->value<int>());
        }
        else if(node->tag() == "height") {
            setHeight(node->value<int>());
        }
        // absolute position
        else if(node->tag() == "position") {
            moveTo(node->value<Point>());
        }
        else if(node->tag() == "x") {
            setX(node->value<int>());
        }
        else if(node->tag() == "y") {
            setY(node->value<int>());
        }
        // margins
        else if(node->tag() == "margin.left") {
            setMarginLeft(node->value<int>());
        }
        else if(node->tag() == "margin.right") {
            setMarginRight(node->value<int>());
        }
        else if(node->tag() == "margin.top") {
            setMarginTop(node->value<int>());
        }
        else if(node->tag() == "margin.bottom") {
            setMarginBottom(node->value<int>());
        }
        // anchors
        else if(boost::starts_with(node->tag(), "anchors.")) {
            std::string what = node->tag().substr(8);
            if(what == "fill") {
                fill(node->value());
            } else if(what == "centerIn") {
                centerIn(node->value());
            } else {
                AnchorEdge edge = fw::translateAnchorEdge(what);

                std::string anchorDescription = node->value();
                std::vector<std::string> split;
                boost::split(split, anchorDescription, boost::is_any_of(std::string(".")));
                if(split.size() != 2)
                    throw OTMLException(node, "invalid anchor description");

                std::string hookedWidgetId = split[0];
                AnchorEdge hookedEdge = fw::translateAnchorEdge(split[1]);

                if(edge == AnchorNone)
                    throw OTMLException(node, "invalid anchor edge");

                if(hookedEdge == AnchorNone)
                    throw OTMLException(node, "invalid anchor target edge");

                addAnchor(edge, hookedWidgetId, hookedEdge);
            }
        }
        else if(node->tag() == "onLoad") {
            g_lua.loadFunction(node->value<std::string>(), "@" + node->source() + "[" + node->tag() + "]");
            luaSetField("onLoad");
        }
    }
}

void UIWidget::onGeometryUpdate(const Rect& oldRect, const Rect& newRect)
{

}

void UIWidget::onFocusChange(bool focused, UI::FocusReason reason)
{
    // when containers lose or get focus it's focused child do the same
    if(m_focusedChild)
        m_focusedChild->onFocusChange(focused, reason);
}

void UIWidget::onHoverChange(bool hovered)
{

}

bool UIWidget::onKeyPress(uchar keyCode, char keyChar, int keyboardModifiers)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        // events on hidden or disabled widgets are discarded
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // key events go only to containers or focused child
        if(child->hasChildren() || (child->isFocusable() && child->hasFocus())) {
            if(child->onKeyPress(keyCode, keyChar, keyboardModifiers))
                return true;
        }
    }

    return false;
}

bool UIWidget::onKeyRelease(uchar keyCode, char keyChar, int keyboardModifiers)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        // events on hidden or disabled widgets are discarded
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // key events go only to containers or focused child
        if(child->hasChildren() || (child->isFocusable() && child->hasFocus())) {
            if(child->onKeyRelease(keyCode, keyChar, keyboardModifiers))
                return true;
        }
    }

    return false;
}

bool UIWidget::onMousePress(const Point& mousePos, UI::MouseButton button)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        // events on hidden or disabled widgets are discarded
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // mouse press events only go to children that contains the mouse position
        if(child->getRect().contains(mousePos) && child == getChildByPos(mousePos)) {
            // when a focusable item is focused it must gain focus
            if(child->isFocusable())
                focusChild(child, UI::MouseFocusReason);

            if(child->onMousePress(mousePos, button))
                return true;
        }
    }

    return false;
}

bool UIWidget::onMouseRelease(const Point& mousePos, UI::MouseButton button)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        // events on hidden or disabled widgets are discarded
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // mouse release events go to all children
        if(child->onMouseRelease(mousePos, button))
            return true;
    }

    return false;
}

bool UIWidget::onMouseMove(const Point& mousePos, const Point& mouseMoved)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        // events on hidden or disabled widgets are discarded
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // check if the mouse is really over this child
        bool overChild = (isHovered() &&
                          child->getRect().contains(mousePos) &&
                          child == getChildByPos(mousePos));

        // trigger hover events
        if(overChild != child->isHovered()) {
            child->setHovered(overChild);
            child->onHoverChange(overChild);
        }

        // mouse move events go to all children
        if(child->onMouseMove(mousePos, mouseMoved))
            return true;
    }

    return false;
}

bool UIWidget::onMouseWheel(const Point& mousePos, UI::MouseWheelDirection direction)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        // events on hidden or disabled widgets are discarded
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // mouse wheel events only go to children that contains the mouse position
        if(child->getRect().contains(mousePos) && child == getChildByPos(mousePos)) {
            if(child->onMouseWheel(mousePos, direction))
                return true;
        }
    }

    return false;
}
