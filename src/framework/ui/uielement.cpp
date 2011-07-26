#include <global.h>
#include <core/dispatcher.h>
#include <graphics/graphics.h>
#include <ui/uielement.h>
#include <ui/uiskins.h>
#include <ui/uielementskin.h>
#include <ui/uicontainer.h>
#include <ui/uianchorlayout.h>
#include <script/luainterface.h>

UIElement::UIElement(UI::ElementType type) :
    LuaObject(),
    m_type(type),
    m_visible(true),
    m_enabled(true),
    m_mouseOver(false),
    m_destroyed(false),
    m_marginLeft(0),
    m_marginRight(0),
    m_marginTop(0),
    m_marginBottom(0)
{
    // generate an unique id, this is need because anchoed layouts find elements by id
    static unsigned long id = 1;
    m_id = make_string("element", id++);
}

UIElement::~UIElement()
{
    //logTraceDebug(getId());
}

void UIElement::destroyLater()
{
    //logTraceDebug(getId());
    if(!m_destroyed)
        g_dispatcher.addTask(std::bind(&UIElement::destroy, asUIElement()));
}

void UIElement::destroy()
{
    //logTraceDebug(getId());
    if(!m_destroyed) {
        UIElementPtr me = asUIElement();

        // remove from parent
        if(getParent())
            getParent()->removeChild(me);

        g_dispatcher.addTask(std::bind(&UIElement::destroyCheck, me));

        m_destroyed = true;
    }
}

void UIElement::destroyCheck()
{
    //logTraceDebug(getId());

    for(int i=0;i<2;++i)
        g_lua.collectGarbage();

    UIElementPtr me = asUIElement();
    // check for leaks, the number of references must be always 2 here
    if(me.use_count() != 2 && me != UIContainer::getRoot()) {
        logWarning("destroyed element with id '",getId(),"', but it still have ",(me.use_count()-2)," references left");
    }
}

void UIElement::setSize(const Size& size)
{
    Rect rect = getRect();
    rect.setSize(size);
    setRect(rect);

    if(UILayoutPtr layout = getLayout())
        layout->recalculateElementLayout(asUIElement());
}

void UIElement::setRect(const Rect& rect)
{
    if(rect != m_rect) {
        m_rect = rect;

        // rect updated, recalculate children layout
        if(UILayoutPtr layout = getLayout())
            layout->recalculateChildrenLayout(asUIElement());

        onRectUpdate();
    }
}

void UIElement::applyDefaultSkin()
{
    setSkin(g_uiSkins.getElementSkin(getElementType(), "default"));
}

void UIElement::setSkin(const UIElementSkinPtr& skin)
{
    m_skin = skin;
    if(skin)
        skin->apply(this);
}

void UIElement::onLoad()
{
    UIElementPtr me = asUIElement();
    g_dispatcher.addTask([me] {
        me->callField("onLoad");
    });
}

void UIElement::render()
{
    if(m_skin)
        m_skin->draw(this);
    //g_graphics.drawBoundingRect(getRect());
}

UIElementPtr UIElement::backwardsGetElementById(const std::string& id)
{
    UIElementPtr element;
    if(getId() == id || id == "self")
        element = asUIElement();
    else if(id == "parent")
        element = getParent();
    else if(id == "root")
        element = UIContainer::getRoot();
    else if(id == "prev") {
        if(UIContainerPtr parent = getParent())
            element = parent->getChildBefore(asUIElement());
    } else if(id == "next") {
        if(UIContainerPtr parent = getParent())
            element = parent->getChildAfter(asUIElement());
    } else {
        if(asUIContainer()) {
            element = asUIContainer()->recursiveGetChildById(id);
            if(element)
                return element;
        }

        if(getParent())
            element = getParent()->backwardsGetElementById(id);
    }
    return element;
}

void UIElement::moveTo(Point pos)
{
    Rect newRect = getRect();
    newRect.moveTo(pos);

    // bound newRect to parent rect
    UIContainerPtr parent = getParent();
    if(parent) {
        Rect parentRect = parent->getRect();
        if(newRect.left() < parentRect.left())
            newRect.moveLeft(parentRect.left());
        if(newRect.top() < parentRect.top())
            newRect.moveTop(parentRect.top());
        if(newRect.bottom() > parentRect.bottom())
            newRect.moveBottom(parentRect.bottom());
        if(newRect.right() > parentRect.right())
            newRect.moveRight(parentRect.right());
    }
    setRect(newRect);
}

void UIElement::setLocked(bool locked)
{
    UIContainerPtr parent = getParent();
    if(parent) {
        if(locked)
            parent->lockElement(asUIElement());
        else
            parent->unlockElement(asUIElement());
    }
}

void UIElement::setParent(UIContainerPtr parent)
{
    UIElementPtr me = asUIElement();
    UIContainerPtr oldParent = m_parent.lock();
    m_parent.reset();
    if(oldParent && oldParent->hasChild(me)) {
        oldParent->removeChild(me);
    }
    if(parent) {
        m_parent = UIContainerWeakPtr(parent);
        if(!parent->hasChild(me))
            parent->addChild(me);
    }
}

bool UIElement::isFocused() const
{
    if(UIContainerPtr parent = m_parent.lock())
        return (parent->getFocusedElement() == shared_from_this());
    return false;
}

void UIElement::setFocused(bool focused)
{
    if(UIContainerPtr parent = m_parent.lock()) {
        if(focused) {
            parent->setFocusedElement(asUIElement());
        } else if(parent->getFocusedElement() == asUIElement()) {
            parent->setFocusedElement(UIElementPtr());
        }
    }
}

UILayoutPtr UIElement::getLayout() const
{
    if(m_layout)
        return m_layout;
    else if(getParent())
        return getParent()->getLayout();
    return UILayoutPtr();
}

void UIElement::centerIn(const std::string& targetId)
{
    addAnchor(AnchorHorizontalCenter, targetId, AnchorHorizontalCenter);
    addAnchor(AnchorVerticalCenter, targetId, AnchorVerticalCenter);
}

void UIElement::addAnchor(AnchorPoint edge, const std::string& targetId, AnchorPoint targetEdge)
{
    UIAnchorLayoutPtr layout = std::dynamic_pointer_cast<UIAnchorLayout>(getLayout());
    if(layout)
        layout->addAnchor(asUIElement(), edge, AnchorLine(targetId, targetEdge));
}
