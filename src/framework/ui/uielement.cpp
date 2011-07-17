#include <global.h>
#include <core/dispatcher.h>
#include <graphics/graphics.h>
#include <ui/uielement.h>
#include <ui/uiskins.h>
#include <ui/uielementskin.h>
#include <ui/uicontainer.h>
#include <ui/uianchorlayout.h>

UIElement::UIElement(UI::ElementType type) :
    ScriptObject(),
    m_type(type),
    m_visible(true),
    m_enabled(true),
    m_mouseOver(false),
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
    g_dispatcher.addTask(boost::bind(&UIElement::destroy, asUIElement()));
}

void UIElement::destroy()
{
    //logTraceDebug(getId());

    UIElementPtr me = asUIElement();
    callScriptTableField("onDestroy");

    // remove from parent
    if(getParent())
        getParent()->removeChild(me);

    // free script stuff
    releaseScriptObject();

    g_dispatcher.addTask(boost::bind(&UIElement::destroyCheck, me));
}

void UIElement::destroyCheck()
{
    //logTraceDebug(getId());

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
    g_dispatcher.addTask(boost::bind(&ScriptObject::callScriptTableField, shared_from_this(), "onLoad", 0));
}

void UIElement::render()
{
    if(m_skin)
        m_skin->draw(this);
    //g_graphics.drawBoundingRect(getRect());
}

UIElementPtr UIElement::backwardsGetElementById(const std::string& id)
{
    if(getId() == id || id == "self")
        return asUIElement();

    if(id == "parent")
        return getParent();

    if(id == "root")
        return UIContainer::getRoot();

    UIElementPtr element;
    if(asUIContainer()) {
        element = asUIContainer()->recursiveGetChildById(id);
        if(element)
            return element;
    }

    if(getParent())
        element = getParent()->backwardsGetElementById(id);
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
    addAnchor(AnchorHorizontalCenter, AnchorLine(targetId, AnchorHorizontalCenter));
    addAnchor(AnchorVerticalCenter, AnchorLine(targetId, AnchorVerticalCenter));
}

void UIElement::addAnchor(AnchorPoint anchoredEdge, AnchorLine anchorEdge)
{
    UIElementPtr target = backwardsGetElementById(anchorEdge.getElementId());
    if(!target)
        logWarning("warning: element id '", anchorEdge.getElementId(), "' doesn't exist while anchoring element '", getId(), "'");

    UIAnchorLayoutPtr layout = boost::dynamic_pointer_cast<UIAnchorLayout>(getLayout());
    if(layout)
        layout->addAnchor(asUIElement(), anchoredEdge, anchorEdge);
}
