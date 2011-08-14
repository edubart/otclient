#include "uiwidget.h"
#include "uimanager.h"
#include "uilayout.h"
#include "uianchorlayout.h"

#include <core/eventdispatcher.h>
#include <graphics/image.h>
#include <graphics/borderimage.h>
#include <graphics/fontmanager.h>
#include <otml/otmlnode.h>

UIWidget::UIWidget(UIWidgetType type)
{
    m_type = type;
    m_visible = true;
    m_enabled = true;
    m_hovered = false;
    m_focusable = false;
    m_destroyed = false;
    m_updateScheduled = false;
    m_marginLeft = m_marginRight = m_marginTop = m_marginBottom = 0;
    m_color = Color::white;

    // generate an unique id, this is need because anchored layouts find widgets by id
    static unsigned long id = 1;
    m_id = aux::make_string("widget", id++);
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

void UIWidget::loadStyleFromOTML(const OTMLNodePtr& styleNode)
{
    assert(!m_destroyed);

    // load styles used by all widgets
    for(const OTMLNodePtr& node : styleNode->childNodes()) {
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
        // color
        else if(node->tag() == "color") {
            setColor(node->read<Color>());
        }
        // size
        else if(node->tag() == "size") {
            resize(node->read<Size>());
        }
        else if(node->tag() == "width") {
            setWidth(node->read<int>());
        }
        else if(node->tag() == "height") {
            setHeight(node->read<int>());
        }
        // position
        else if(node->tag() == "position") {
            move(node->read<Point>());
        }
        else if(node->tag() == "x") {
            setX(node->read<int>());
        }
        else if(node->tag() == "y") {
            setY(node->read<int>());
        }
        // margins
        else if(node->tag() == "margin.left") {
            setMarginLeft(node->read<int>());
        }
        else if(node->tag() == "margin.right") {
            setMarginRight(node->read<int>());
        }
        else if(node->tag() == "margin.top") {
            setMarginTop(node->read<int>());
        }
        else if(node->tag() == "margin.bottom") {
            setMarginBottom(node->read<int>());
        }
        // anchors
        else if(boost::starts_with(node->tag(), "anchors.")) {
            std::string what = node->tag().substr(8);
            if(what == "fill") {
                fill(node->value());
            } else if(what == "centerIn") {
                centerIn(node->value());
            } else {
                AnchorPoint myEdge = parseAnchorPoint(what);

                std::string anchorDescription = node->value();
                std::vector<std::string> split;
                boost::split(split, anchorDescription, boost::is_any_of(std::string(".")));
                if(split.size() != 2)
                    throw OTMLException(node, "invalid anchor description");

                std::string target = split[0];
                AnchorPoint targetEdge = parseAnchorPoint(split[1]);

                if(myEdge == AnchorNone)
                    throw OTMLException(node, "invalid anchor edge");

                if(targetEdge == AnchorNone)
                    throw OTMLException(node, "invalid anchor target edge");

                addAnchor(myEdge, target, targetEdge);
            }
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
        if(child->isVisible())
            child->render();
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
        g_dispatcher.addEvent([self]() {
            self->m_updateScheduled = false;
            // this widget could be destroyed before the event happens
            if(!self->isDestroyed())
                self->onGeometryUpdate();
        });
        m_updateScheduled = true;
    }
}

void UIWidget::focusChild(const UIWidgetPtr& focusedChild)
{
    assert(!m_destroyed);

    if(focusedChild != m_focusedChild) {
        UIWidgetPtr oldFocused = m_focusedChild;
        m_focusedChild = focusedChild;

        if(oldFocused) {
            g_dispatcher.addEvent([oldFocused]() {
                // the widget can be destroyed before the event happens
                if(!oldFocused->isDestroyed())
                    oldFocused->onFocusChange(false);
            });
        }
        if(focusedChild) {
            g_dispatcher.addEvent([focusedChild]() {
                // the widget can be destroyed before the event happens
                if(!focusedChild->isDestroyed())
                    focusedChild->onFocusChange(true);
            });
        }
    }

    // when containers are focused they go to the top
    if(focusedChild && focusedChild->hasChildren())
        moveChildToTop(focusedChild);
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
            else
                return child->recursiveGetChildById(childId);
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

void UIWidget::addChild(const UIWidgetPtr& childToAdd)
{
    assert(!m_destroyed);

    assert(!hasChild(childToAdd));
    m_children.push_back(childToAdd);
    childToAdd->setParent(asUIWidget());

    // updates child position
    childToAdd->updateGeometry();

    // focus it if there is no focused child yet
    if(!m_focusedChild && childToAdd->isFocusable())
        focusChild(childToAdd);
}

void UIWidget::removeChild(const UIWidgetPtr& childToRemove)
{
    assert(!m_destroyed);

    // defocus if needed
    if(m_focusedChild == childToRemove)
        focusChild(nullptr);

    // remove from children list
    auto it = std::find(m_children.begin(), m_children.end(), childToRemove);
    assert(it != m_children.end());
    m_children.erase(it);

    // reset child parent
    assert(childToRemove->getParent() == asUIWidget());
    childToRemove->setParent(nullptr);
}

void UIWidget::focusNextChild()
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
        focusChild(toFocus);
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

void UIWidget::onKeyboardText(const std::string& text)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isEnabled() || !child->isVisible())
            continue;
        // key events go only to containers or focused child
        if(child->hasChildren() || child->hasFocus())
            child->onKeyboardText(text);
    }
}

void UIWidget::onKeyPress(const UIKeyEvent& event)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isEnabled() || !child->isVisible())
            continue;
        // key events go only to containers or focused child
        if(child->hasChildren() || child->hasFocus())
            child->onKeyPress(event);
    }
}

void UIWidget::onKeyRelease(const UIKeyEvent& event)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isEnabled() || !child->isVisible())
            continue;
        // key events go only to containers or focused child
        if(child->hasChildren() || child->hasFocus())
            child->onKeyRelease(event);
    }
}

void UIWidget::onMousePress(const UIMouseEvent& event)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isEnabled() || !child->isVisible())
            continue;
        // mouse press events only go to children that contains the mouse position
        if(child->getGeometry().contains(event.mousePos) && child == getChildByPos(event.mousePos)) {
            child->onMousePress(event);

            // focus it
            if(child->isFocusable())
                focusChild(child);
        }
    }
}

void UIWidget::onMouseRelease(const UIMouseEvent& event)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isEnabled() || !child->isVisible())
            continue;
        // mouse release events go to all children
        child->onMouseRelease(event);
    }
}

void UIWidget::onMouseMove(const UIMouseEvent& event)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isEnabled() || !child->isVisible())
            continue;

        // update child over status
        bool overChild = (child->getGeometry().contains(event.mousePos) && child == getChildByPos(event.mousePos));
        if(overChild != child->isHovered()) {
            child->setHovered(overChild);
            child->onHoverChange(overChild);
        }
        // mouse move events go to all children
        child->onMouseMove(event);
    }
}

void UIWidget::onMouseWheel(const UIMouseEvent& event)
{
    assert(!m_destroyed);

    // do a backup of children list, because it may change while looping it
    UIWidgetList children = m_children;
    for(const UIWidgetPtr& child : children) {
        if(!child->isEnabled() || !child->isVisible())
            continue;
        // mouse wheel events only go to children that contains the mouse position
        if(child->getGeometry().contains(event.mousePos) && child == getChildByPos(event.mousePos))
            child->onMouseWheel(event);
    }
}
