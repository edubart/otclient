#include "uiwidget.h"
#include "uimanager.h"
#include "uilayout.h"
#include "uianchorlayout.h"

#include <framework/core/eventdispatcher.h>
#include <framework/graphics/image.h>
#include <framework/graphics/borderimage.h>
#include <framework/graphics/fontmanager.h>
#include <framework/otml/otmlnode.h>
#include <framework/graphics/graphics.h>

UIWidget::UIWidget(UIWidgetType type)
{
    m_type = type;
    m_visible = true;
    m_enabled = true;
    m_hovered = false;
    m_focusable = false;
    m_destroyed = false;
    m_updateScheduled = false;
    m_opacity = 255;
    m_marginLeft = m_marginRight = m_marginTop = m_marginBottom = 0;
    m_color = Color::white;
    m_fontColor = Color::white;

    // generate an unique id, this is need because anchored layouts find widgets by id
    static unsigned long id = 1;
    m_id = fw::mkstr("widget", id++);
}

UIWidget::~UIWidget()
{
    //logTraceDebug(m_id);
    if(!m_destroyed)
        logWarning("WARNING: widget '", m_id, "' was destructed without being explicit destroyed");
}

UIWidgetPtr UIWidget::create()
{
    UIWidgetPtr widget(new UIWidget);
    //widget->setStyle("Widget");
    return widget;
}

void UIWidget::destroy()
{
    //TODO: onDestroy event
    // destroy only once
    if(!m_destroyed) {
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
        logWarning("WARNING: attempt to destroy widget '", m_id, "' again");
}

void UIWidget::destroyCheck()
{
    // collect lua garbage before checking
    g_lua.collectGarbage();

    // get real use count
    int realUseCount = shared_from_this().use_count() - 2;

    // check for leaks upon widget destruction
    if(realUseCount > 0)
        logWarning("WARNING: destroyed widget with id '",m_id,"', but it still have ",realUseCount," references left");
}

void UIWidget::load()
{
    for(const UIWidgetPtr& child : m_children)
        child->load();

    // schedule onLoad
    UIWidgetPtr self = asUIWidget();
    g_dispatcher.addEvent([self]() {
        // this widget could be destroyed before the event happens
        if(!self->isDestroyed())
            self->callLuaField("onLoad");
    });
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
        else if(node->tag() == "font-color") {
            setFontColor(node->value<Color>());
        }
        // color
        else if(node->tag() == "color") {
            setColor(node->value<Color>());
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
                AnchorPoint myEdge = fw::translateAnchorPoint(what);

                std::string anchorDescription = node->value();
                std::vector<std::string> split;
                boost::split(split, anchorDescription, boost::is_any_of(std::string(".")));
                if(split.size() != 2)
                    throw OTMLException(node, "invalid anchor description");

                std::string target = split[0];
                AnchorPoint targetEdge = fw::translateAnchorPoint(split[1]);

                if(myEdge == AnchorNone)
                    throw OTMLException(node, "invalid anchor edge");

                if(targetEdge == AnchorNone)
                    throw OTMLException(node, "invalid anchor target edge");

                addAnchor(myEdge, target, targetEdge);
            }
        }
        else if(node->tag() == "onLoad") {
            g_lua.loadFunction(node->value<std::string>(), "@" + node->source() + "[" + node->tag() + "]");
            luaSetField("onLoad");
        }
    }

    if(!m_font)
        m_font = g_fonts.getDefaultFont();
}

void UIWidget::render()
{
    assert(!m_destroyed);

    if(m_image)
        m_image->draw(getGeometry());

    for(const UIWidgetPtr& child : m_children) {
        if(child->isVisible()) {
            int oldOpacity = g_graphics.getOpacity();
            if(child->getOpacity() < oldOpacity)
                g_graphics.setOpacity(child->getOpacity());

            g_graphics.bindColor(child->getColor());
            child->render();

            g_graphics.setOpacity(oldOpacity);
        }
    }
}

void UIWidget::updateGeometry()
{
    assert(!m_destroyed);

    if(UILayoutPtr layout = getLayout())
        layout->updateWidget(asUIWidget());
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
        updateGeometry();
    } catch(std::exception& e) {
        logError("ERROR: couldn't change widget '", m_id, "' style: ", e.what());
    }
}

void UIWidget::setGeometry(const Rect& rect)
{
    assert(!m_destroyed);

    Rect oldRect = m_rect;
    m_rect = rect;
    if(UILayoutPtr layout = getLayout())
        layout->updateWidgetChildren(asUIWidget());

    // avoid massive updates
    if(!m_updateScheduled) {
        UIWidgetPtr self = asUIWidget();
        g_dispatcher.addEvent([self, oldRect]() {
            self->m_updateScheduled = false;
            UIGeometryUpdateEvent e(oldRect, self->getGeometry());
            // this widget could be destroyed before the event happens
            if(!self->isDestroyed())
                self->onGeometryUpdate(e);
        });
        m_updateScheduled = true;
    }
}

void UIWidget::lock()
{
    assert(!m_destroyed);

    UIWidgetPtr parent = getParent();
    if(parent)
        parent->lockChild(asUIWidget());
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

    if(UIWidgetPtr parent = m_parent.lock())
        return (parent->getFocusedChild() == shared_from_this());
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

UILayoutPtr UIWidget::getLayout() const
{
    assert(!m_destroyed);

    if(m_layout)
        return m_layout;
    else if(getParent() && getParent()->getLayout())
        return getParent()->getLayout();
    // fallback to root layout
    return g_ui.getRootWidget()->getLayout();
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
        if(widget->getGeometry().contains(childPos))
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

UIWidgetPtr UIWidget::recursiveGetChildById(const std::string& childId)
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
        for(const UIWidgetPtr& child : m_children) {
            if(UIWidgetPtr subChild = child->recursiveGetChildById(childId)) {
                if(subChild->getId() == childId)
                    return subChild;
            }
        }
    }
    return nullptr;
}

UIWidgetPtr UIWidget::recursiveGetChildByPos(const Point& childPos)
{
    assert(!m_destroyed);

    for(const UIWidgetPtr& child : m_children) {
        if(child->getGeometry().contains(childPos)) {
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

    UIWidgetPtr widget;
    if(getId() == id || id == "self")
        widget = asUIWidget();
    else if(id == "parent")
        widget = getParent();
    else if(id == "root")
        widget = g_ui.getRootWidget();
    else if(id == "prev") {
        if(UIWidgetPtr parent = getParent())
            widget = parent->getChildBefore(asUIWidget());
    } else if(id == "next") {
        if(UIWidgetPtr parent = getParent())
            widget = parent->getChildAfter(asUIWidget());
    } else {
        widget = recursiveGetChildById(id);
        if(widget)
            return widget;

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
            g_dispatcher.addEvent([oldFocused,reason]() {
                // the widget can be destroyed before the event happens
                UIFocusEvent e(reason, false);
                if(!oldFocused->isDestroyed())
                    oldFocused->onFocusChange(e);
            });
        }
        if(focusedChild) {
            g_dispatcher.addEvent([focusedChild,reason]() {
                // the widget can be destroyed before the event happens
                UIFocusEvent e(reason, true);
                if(!focusedChild->isDestroyed())
                    focusedChild->onFocusChange(e);
            });
        }

        // when containers are focused they go to the top
        if(focusedChild && focusedChild->hasChildren())
            moveChildToTop(focusedChild);
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

    // updates child position
    childToAdd->updateGeometry();

    // focus it if there is no focused child yet
    if(!m_focusedChild && childToAdd->isFocusable())
        focusChild(childToAdd, ActiveFocusReason);
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
}

void UIWidget::unlockChild(const UIWidgetPtr& childToUnlock)
{
    assert(hasChild(childToUnlock));

    auto it = std::find(m_lockedWidgets.begin(), m_lockedWidgets.end(), childToUnlock);
    if(it != m_lockedWidgets.end()) {
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
}

void UIWidget::addAnchor(AnchorPoint edge, const std::string& targetId, AnchorPoint targetEdge)
{
    assert(!m_destroyed);

    UIAnchorLayoutPtr layout = std::dynamic_pointer_cast<UIAnchorLayout>(getLayout());
    assert(layout);
    if(layout)
        layout->addAnchor(asUIWidget(), edge, AnchorLine(targetId, targetEdge));
}

void UIWidget::centerIn(const std::string& targetId)
{
    assert(!m_destroyed);

    addAnchor(AnchorHorizontalCenter, targetId, AnchorHorizontalCenter);
    addAnchor(AnchorVerticalCenter, targetId, AnchorVerticalCenter);
}

void UIWidget::fill(const std::string& targetId)
{
    assert(!m_destroyed);

    addAnchor(AnchorLeft, targetId, AnchorLeft);
    addAnchor(AnchorRight, targetId, AnchorRight);
    addAnchor(AnchorTop, targetId, AnchorTop);
    addAnchor(AnchorBottom, targetId, AnchorBottom);
}

void UIWidget::onKeyPress(UIKeyEvent& event)
{
    assert(!m_destroyed);

    event.ignore();

    // focus next child when pressing tab
    if(isFocusable() && hasFocus() && !hasChildren() && event.keyCode() == KC_TAB) {
        if(UIWidgetPtr parent = getParent()) {
            g_dispatcher.addEvent([parent]{
                if(!parent->isDestroyed())
                    parent->focusNextChild(TabFocusReason);
            });
        }
        event.accept();
        return;
    }

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isExplicitlyEnabled() || !child->isVisible())
            continue;

        // key events go only to containers or focused child
        if(child->hasChildren() || child->hasFocus()) {
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
        if(!child->isExplicitlyEnabled() || !child->isVisible())
            continue;

        // key events go only to containers or focused child
        if(child->hasChildren() || child->hasFocus()) {
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
        if(!child->isExplicitlyEnabled() || !child->isVisible())
            continue;

        // mouse press events only go to children that contains the mouse position
        if(child->getGeometry().contains(event.pos()) && child == getChildByPos(event.pos())) {
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
        if(!child->isExplicitlyEnabled() || !child->isVisible())
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
        if(!child->isExplicitlyEnabled() || !child->isVisible())
            continue;

        // check if the mouse is relally over this child
        bool overChild = (isHovered() &&
                          child->getGeometry().contains(event.pos()) &&
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
        if(!child->isExplicitlyEnabled() || !child->isVisible())
            continue;

        // mouse wheel events only go to children that contains the mouse position
        if(child->getGeometry().contains(event.pos()) && child == getChildByPos(event.pos())) {
            event.accept();
            child->onMouseWheel(event);
        }

        if(event.isAccepted())
            break;
    }
}
