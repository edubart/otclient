#ifndef UIANCHORLAYOUT_H
#define UIANCHORLAYOUT_H

#include <global.h>
#include <ui/uiconstants.h>
#include <ui/uilayout.h>
#include <ui/uielement.h>

class AnchorLine
{
public:
    AnchorLine(const std::string& elementId, UI::AnchorPoint edge) : m_elementId(elementId), m_edge(edge) { }
    AnchorLine(const AnchorLine& other) : m_elementId(other.m_elementId), m_edge(other.m_edge) { }
    UI::AnchorPoint getEdge() const { return m_edge; }
    const std::string& getElementId() const { return m_elementId; }

private:
    std::string m_elementId;
    UI::AnchorPoint m_edge;
};

class Anchor
{
public:
    Anchor(const UIElementPtr& anchoredElement, UI::AnchorPoint anchoredEdge, const AnchorLine& anchorLine)
    : m_anchoredElement(anchoredElement), m_anchoredEdge(anchoredEdge), m_anchorLine(anchorLine) { }
    UIElementPtr getAnchorLineElement() const ;
    UIElementPtr getAnchoredElement() const { return m_anchoredElement.lock(); }
    UI::AnchorPoint getAnchoredEdge() const { return m_anchoredEdge; }
    int getAnchorLinePoint() const;

private:
    UIElementWeakPtr m_anchoredElement;
    UI::AnchorPoint m_anchoredEdge;
    AnchorLine m_anchorLine;
};

class UIAnchorLayout : public UILayout
{
public:
    bool addAnchor(const UIElementPtr& anchoredElement, UI::AnchorPoint anchoredEdge, const AnchorLine& anchorLine);
    void recalculateElementLayout(const UIElementPtr& element);
    void recalculateChildrenLayout(const UIElementPtr& parent);

    bool hasElementInAnchorTree(const UIElementPtr& element, const UIElementPtr& treeAnchor);
    static UI::AnchorPoint parseAnchorPoint(const std::string& anchorPointStr);

private:
    std::vector<Anchor> m_anchors;
};

typedef boost::shared_ptr<UIAnchorLayout> UIAnchorLayoutPtr;

#endif // UIANCHORLAYOUT_H
