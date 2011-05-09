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


#ifndef UIANCHORLAYOUT_H
#define UIANCHORLAYOUT_H

#include <prerequisites.h>
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
