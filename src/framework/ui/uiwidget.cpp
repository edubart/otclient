#include "uiwidget.h"
#include "uimanager.h"

#include <framework/core/eventdispatcher.h>
#include <framework/graphics/image.h>
#include <framework/graphics/borderimage.h>
#include <framework/graphics/fontmanager.h>
#include <framework/otml/otmlnode.h>
#include <framework/graphics/graphics.h>
#include "uianchor.h"

UIWidget::UIWidget(UIWidgetType type)
{
    m_type = type;
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

UIWidgetPtr UIWidget::create()
{
    UIWidgetPtr widget(new UIWidget);
    return widget;
}

void UIWidget::destroy()
{
    //TODO: onDestroy event
    // destroy only once
    if(!m_destroyed) {
        releaseLuaFieldsTable();

        // clear additional reference
        m_lockedWidgets.clear();
        m_focusedChild.reset();

        // destroy children
        while(m_children.size() > 0) {
            UIWidgetPtr child = m_children.front(); //hold reference
            child->destroy();
        }

        // remove itself from parent
        if(UIWidgetPtr parent = getParent())
            parent->removeChild(asUIWidget());

        // add destroy check event
        g_dispatcher.addEvent(std::bind(&UIWidget::destroyCheck, asUIWidget()));
        m_destroyed = true;
        m_enabled = false;
        m_visible = false;
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

void UIWidget::loadStyleFromOTML(const OTMLNodePtr& styleNode)
{
    assert(!m_destroyed);

    // load styles used by all widgets
    for(const OTMLNodePtr& node : styleNode->children()) {
        // id
        if(node->tag() == "id") {
            setId(node->value());
        }
        // image
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
        // font color
        else if(node->tag() == "color") {
            setForegroundColor(node->value<Color>());
        }
        // color
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
        // position
        else if(node->tag() == "position") {
            move(node->value<Point>());
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
                AnchorEdge myEdge = fw::translateAnchorEdge(what);

                std::string anchorDescription = node->value();
                std::vector<std::string> split;
                boost::split(split, anchorDescription, boost::is_any_of(std::string(".")));
                if(split.size() != 2)
                    throw OTMLException(node, "invalid anchor description");

                std::string target = split[0];
                AnchorEdge hookedEdge = fw::translateAnchorEdge(split[1]);

                if(myEdge == AnchorNone)
                    throw OTMLException(node, "invalid anchor edge");

                if(hookedEdge == AnchorNone)
                    throw OTMLException(node, "invalid anchor target edge");

                addAnchor(myEdge, target, hookedEdge);
            }
        }
        else if(node->tag() == "onLoad") {
            g_lua.loadFunction(node->value<std::string>(), "@" + node->source() + "[" + node->tag() + "]");
            luaSetField("onLoad");
        }
    }
}

void UIWidget::render()
{
    assert(!m_destroyed);

    if(m_image)
        m_image->draw(getRect());

    for(const UIWidgetPtr& child : m_children) {
        if(child->isExplicitlyVisible()) {
            int oldOpacity = g_graphics.getOpacity();
            if(child->getOpacity() < oldOpacity)
                g_graphics.setOpacity(child->getOpacity());

            g_graphics.bindColor(child->getBackgroundColor());
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

    UIWidgetPtr me = asUIWidget();

    // remove from old parent
    UIWidgetPtr oldParent = m_parent.lock();
    if(oldParent && oldParent->hasChild(me)) {
        oldParent->removeChild(me);
    }
    m_parent.reset();

    if(parent) {
        m_parent = UIWidgetWeakPtr(parent);
        if(!parent->hasChild(me))
            parent->addChild(me);
    }
}

void UIWidget::setStyle(const std::string& styleName)
{
    try {
        OTMLNodePtr styleNode = g_ui.getStyle(styleName);
        loadStyleFromOTML(styleNode);

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
            UIRectUpdateEvent e(oldRect, self->getRect());
            // this widget could be destroyed before the event happens
            if(!self->isDestroyed())
                self->onRectUpdate(e);
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

bool UIWidget::hasFocus()
{
    assert(!m_destroyed);

    if(UIWidgetPtr parent = getParent()) {
        if(parent->hasFocus() && parent->getFocusedChild() == shared_from_this())
            return true;
    }
    // root widget always has focus
    else if(asUIWidget() == g_ui.getRootWidget())
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

    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        const UIWidgetPtr& child = (*it);
        if(child == relativeChild) {
            if(++it != m_children.end())
                return (*it);
            break;
        }
    }
    return nullptr;
}

UIWidgetPtr UIWidget::getChildBefore(const UIWidgetPtr& relativeChild)
{
    assert(!m_destroyed);

    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        const UIWidgetPtr& child = (*it);
        if(child == relativeChild) {
            if(++it != m_children.rend())
                return (*it);
            break;
        }
    }
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
        return g_ui.getRootWidget();
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

void UIWidget::focusChild(const UIWidgetPtr& focusedChild, FocusReason reason)
{
    assert(!m_destroyed);

    if(focusedChild != m_focusedChild) {
        UIWidgetPtr oldFocused = m_focusedChild;
        m_focusedChild = focusedChild;

        if(oldFocused) {
            UIFocusEvent e(reason, false);
            oldFocused->onFocusChange(e);
        }
        if(focusedChild) {
            UIFocusEvent e(reason, focusedChild->hasFocus());
            focusedChild->onFocusChange(e);
        }
    }
}

void UIWidget::addChild(const UIWidgetPtr& childToAdd)
{
    assert(!m_destroyed);

    if(!childToAdd)
        return;

    assert(!hasChild(childToAdd));
    m_children.push_back(childToAdd);
    childToAdd->setParent(asUIWidget());

    // recalculate anchors
    getRootParent()->recalculateAnchoredWidgets();

    // may need to update children layout
    updateChildrenLayout();

    // always focus new children
    if(childToAdd->isFocusable())
        focusChild(childToAdd, ActiveFocusReason);
}

void UIWidget::insertChild(const UIWidgetPtr& childToInsert, int index)
{
    assert(!m_destroyed);

    if(!childToInsert)
        return;

    assert(!hasChild(childToInsert));

    if(index < 0)
        index = m_children.size() + index -1;

    assert((uint)index <= m_children.size());

    auto it = m_children.begin() + index;
    m_children.insert(it, childToInsert);
    childToInsert->setParent(asUIWidget());

    // recalculate anchors
    getRootParent()->recalculateAnchoredWidgets();

    // may need to update children layout
    updateChildrenLayout();
}

void UIWidget::removeChild(const UIWidgetPtr& childToRemove)
{
    assert(!m_destroyed);

    if(!childToRemove)
        return;

    // defocus if needed
    if(m_focusedChild == childToRemove)
        focusChild(nullptr, ActiveFocusReason);

    // try to unlock
    unlockChild(childToRemove);

    // remove from children list
    auto it = std::find(m_children.begin(), m_children.end(), childToRemove);
    assert(it != m_children.end());
    m_children.erase(it);

    // reset child parent
    assert(childToRemove->getParent() == asUIWidget());
    childToRemove->setParent(nullptr);

    // recalculate anchors
    UIWidgetPtr parent = getRootParent();
    parent->recalculateAnchoredWidgets();

    // may need to update children layout
    updateChildrenLayout();
}

void UIWidget::focusNextChild(FocusReason reason)
{
    assert(!m_destroyed);

    UIWidgetPtr toFocus;
    UIWidgetList rotatedChildren(m_children);
    auto focusedIt = std::find(rotatedChildren.begin(), rotatedChildren.end(), m_focusedChild);
    if(focusedIt != rotatedChildren.end()) {
        std::rotate(rotatedChildren.begin(), focusedIt, rotatedChildren.end());
        rotatedChildren.pop_front();
        for(const UIWidgetPtr& child : rotatedChildren) {
            if(child->isFocusable()) {
                toFocus = child;
                break;
            }
        }
    } else if(m_children.size() > 0)
        toFocus = m_children.back();

    if(toFocus)
        focusChild(toFocus, reason);
}

void UIWidget::moveChildToTop(const UIWidgetPtr& childToMove)
{
    assert(!m_destroyed);

    // remove and push child again
    auto it = std::find(m_children.begin(), m_children.end(), childToMove);
    assert(it != m_children.end());
    m_children.erase(it);
    m_children.push_back(childToMove);
}

void UIWidget::lockChild(const UIWidgetPtr& childToLock)
{
    assert(hasChild(childToLock));

    // disable all other widgets
    for(const UIWidgetPtr& widget : m_children) {
        if(widget == childToLock)
            widget->setEnabled(true);
        else
            widget->setEnabled(false);
    }

    m_lockedWidgets.push_front(childToLock);

    // lock child focus
    if(childToLock->isFocusable())
      focusChild(childToLock, ActiveFocusReason);
}

void UIWidget::unlockChild(const UIWidgetPtr& childToUnlock)
{
    assert(hasChild(childToUnlock));

    auto it = std::find(m_lockedWidgets.begin(), m_lockedWidgets.end(), childToUnlock);
    if(it != m_lockedWidgets.end())
        m_lockedWidgets.erase(it);

    UIWidgetPtr newLockedWidget;
    if(m_lockedWidgets.size() > 0)
        newLockedWidget = m_lockedWidgets.front();

    for(const UIWidgetPtr& child : m_children) {
        if(newLockedWidget) {
            if(child == newLockedWidget)
                child->setEnabled(true);
            else
                child->setEnabled(false);
        } else
            child->setEnabled(true);
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

void UIWidget::onFocusChange(UIFocusEvent& event)
{
    if(m_focusedChild)
        m_focusedChild->onFocusChange(event);
}

void UIWidget::onKeyPress(UIKeyEvent& event)
{
    assert(!m_destroyed);

    event.ignore();

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // key events go only to containers or focused child
        if(child->hasChildren() || (child->isFocusable() && child->hasFocus())) {
            event.accept();
            child->onKeyPress(event);
        }

        if(event.isAccepted())
            break;
    }
}

void UIWidget::onKeyRelease(UIKeyEvent& event)
{
    assert(!m_destroyed);

    event.ignore();

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // key events go only to containers or focused child
        if(child->hasChildren() || (child->isFocusable() && child->hasFocus())) {
            event.accept();
            child->onKeyRelease(event);
        }

        if(event.isAccepted())
            break;
    }
}

void UIWidget::onMousePress(UIMouseEvent& event)
{
    assert(!m_destroyed);

    event.ignore();

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // mouse press events only go to children that contains the mouse position
        if(child->getRect().contains(event.pos()) && child == getChildByPos(event.pos())) {
            // focus it
            if(child->isFocusable())
                focusChild(child, MouseFocusReason);

            event.accept();
            child->onMousePress(event);
        }

        if(event.isAccepted())
            break;
    }
}

void UIWidget::onMouseRelease(UIMouseEvent& event)
{
    assert(!m_destroyed);

    event.ignore();

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // mouse release events go to all children
        event.accept();
        child->onMouseRelease(event);

        if(event.isAccepted())
            break;
    }
}

void UIWidget::onMouseMove(UIMouseEvent& event)
{
    assert(!m_destroyed);

    event.ignore();

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // check if the mouse is relally over this child
        bool overChild = (isHovered() &&
                          child->getRect().contains(event.pos()) &&
                          child == getChildByPos(event.pos()));
        if(overChild != child->isHovered()) {
            child->setHovered(overChild);

            UIHoverEvent e(overChild);
            child->onHoverChange(e);
        }

        // mouse move events go to all children
        event.accept();
        child->onMouseMove(event);

        if(event.isAccepted())
            break;
    }
}

void UIWidget::onMouseWheel(UIMouseEvent& event)
{
    assert(!m_destroyed);

    event.ignore();

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isExplicitlyEnabled() || !child->isExplicitlyVisible())
            continue;

        // mouse wheel events only go to children that contains the mouse position
        if(child->getRect().contains(event.pos()) && child == getChildByPos(event.pos())) {
            event.accept();
            child->onMouseWheel(event);
        }

        if(event.isAccepted())
            break;
    }
}
