/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include "otmlnode.h"
#include "otmlemitter.h"
#include "otmldocument.h"

OTMLNodePtr OTMLNode::create(std::string tag, bool unique)
{
    OTMLNodePtr node(new OTMLNode);
    node->setTag(tag);
    node->setUnique(unique);
    return node;
}

OTMLNodePtr OTMLNode::create(std::string tag, std::string value)
{
    OTMLNodePtr node(new OTMLNode);
    node->setTag(tag);
    node->setValue(value);
    node->setUnique(true);
    return node;
}

bool OTMLNode::hasChildren()
{
    int count = 0;
    for(const OTMLNodePtr& child : m_children) {
        if(!child->isNull())
            count++;
    }
    return count > 0;
}

OTMLNodePtr OTMLNode::get(const std::string& childTag)
{
    for(const OTMLNodePtr& child : m_children) {
        if(child->tag() == childTag && !child->isNull())
            return child;
    }
    return nullptr;
}

OTMLNodePtr OTMLNode::getIndex(int childIndex)
{
    if(childIndex < size() && childIndex >= 0)
        return m_children[childIndex];
    return nullptr;
}

OTMLNodePtr OTMLNode::at(const std::string& childTag)
{
    OTMLNodePtr res;
    for(const OTMLNodePtr& child : m_children) {
        if(child->tag() == childTag && !child->isNull()) {
            res = child;
            break;
        }
    }
    if(!res)
        throw OTMLException(asOTMLNode(), stdext::format("child node with tag '%s' not found", childTag));
    return res;
}

OTMLNodePtr OTMLNode::atIndex(int childIndex)
{
    if(childIndex >= size() || childIndex < 0)
        throw OTMLException(asOTMLNode(), stdext::format("child node with index '%d' not found", childIndex));
    return m_children[childIndex];
}

void OTMLNode::addChild(const OTMLNodePtr& newChild)
{
    // replace is needed when the tag is marked as unique
    if(newChild->hasTag()) {
        for(const OTMLNodePtr& node : m_children) {
            if(node->tag() == newChild->tag() && (node->isUnique() || newChild->isUnique())) {
                newChild->setUnique(true);

                if(node->hasChildren() && newChild->hasChildren()) {
                    OTMLNodePtr tmpNode = node->clone();
                    tmpNode->merge(newChild);
                    newChild->copy(tmpNode);
                }

                replaceChild(node, newChild);

                // remove any other child with the same tag
                auto it = m_children.begin();
                while(it != m_children.end()) {
                    OTMLNodePtr node = (*it);
                    if(node != newChild && node->tag() == newChild->tag()) {
                        it = m_children.erase(it);
                    } else
                        ++it;
                }
                return;
            }
        }
    }

    m_children.push_back(newChild);
}

bool OTMLNode::removeChild(const OTMLNodePtr& oldChild)
{
    auto it = std::find(m_children.begin(), m_children.end(), oldChild);
    if(it != m_children.end()) {
        m_children.erase(it);
        return true;
    }
    return false;
}

bool OTMLNode::replaceChild(const OTMLNodePtr& oldChild, const OTMLNodePtr& newChild)
{
    auto it = std::find(m_children.begin(), m_children.end(), oldChild);
    if(it != m_children.end()) {
        it = m_children.erase(it);
        m_children.insert(it, newChild);
        return true;
    }
    return false;
}

void OTMLNode::copy(const OTMLNodePtr& node)
{
    setTag(node->tag());
    setValue(node->rawValue());
    setUnique(node->isUnique());
    setNull(node->isNull());
    setSource(node->source());
    clear();
    for(const OTMLNodePtr& child : node->m_children)
        addChild(child->clone());
}

void OTMLNode::merge(const OTMLNodePtr& node)
{
    for(const OTMLNodePtr& child : node->m_children)
        addChild(child->clone());
    setTag(node->tag());
    setSource(node->source());
}

void OTMLNode::clear()
{
    m_children.clear();
}

OTMLNodeList OTMLNode::children()
{
    OTMLNodeList children;
    for(const OTMLNodePtr& child : m_children)
        if(!child->isNull())
            children.push_back(child);
    return children;
}

OTMLNodePtr OTMLNode::clone()
{
    OTMLNodePtr myClone(new OTMLNode);
    myClone->setTag(m_tag);
    myClone->setValue(m_value);
    myClone->setUnique(m_unique);
    myClone->setNull(m_null);
    myClone->setSource(m_source);
    for(const OTMLNodePtr& child : m_children)
        myClone->addChild(child->clone());
    return myClone;
}

std::string OTMLNode::emit()
{
    return OTMLEmitter::emitNode(asOTMLNode(), 0);
}
