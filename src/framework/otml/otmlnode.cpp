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

bool OTMLNode::hasChildren() const
{
    int count = 0;
    for(const OTMLNodePtr& child : m_children) {
        if(!child->isNull())
            count++;
    }
    return count > 0;
}

OTMLNodePtr OTMLNode::get(const std::string& childTag) const
{
    for(const OTMLNodePtr& child : m_children) {
        if(child->tag() == childTag && !child->isNull())
            return child;
    }
    return nullptr;
}

OTMLNodePtr OTMLNode::getIndex(int childIndex) const
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
        throw OTMLException(shared_from_this(), fw::mkstr("child node with tag '", childTag, "' not found"));
    return res;
}

OTMLNodePtr OTMLNode::atIndex(int childIndex)
{
    if(childIndex >= size() || childIndex < 0)
        throw OTMLException(shared_from_this(), fw::mkstr("child node with index '", childIndex, "' not found"));
    return m_children[childIndex];
}

void OTMLNode::addChild(const OTMLNodePtr& newChild)
{
    // replace is needed when the tag is marked as unique
    if(newChild->hasTag()) {
        for(const OTMLNodePtr& node : m_children) {
            if(node->tag() == newChild->tag() && (node->isUnique() || newChild->isUnique())) {
                newChild->setUnique(true);
                replaceChild(node, newChild);

                // remove any other child with the same tag
                auto it = m_children.begin();
                while(it != m_children.end()) {
                    OTMLNodePtr node = (*it);
                    if(node != newChild && node->tag() == newChild->tag()) {
                        node->setParent(nullptr);
                        it = m_children.erase(it);
                    } else
                        ++it;
                }
                return;
            }
        }
    }

    m_children.push_back(newChild);
    newChild->setParent(shared_from_this());
}

bool OTMLNode::removeChild(const OTMLNodePtr& oldChild)
{
    auto it = std::find(m_children.begin(), m_children.end(), oldChild);
    if(it != m_children.end()) {
        m_children.erase(it);
        oldChild->setParent(nullptr);
        return true;
    }
    return false;
}

bool OTMLNode::replaceChild(const OTMLNodePtr& oldChild, const OTMLNodePtr& newChild)
{
    auto it = std::find(m_children.begin(), m_children.end(), oldChild);
    if(it != m_children.end()) {
        oldChild->setParent(nullptr);
        newChild->setParent(shared_from_this());
        it = m_children.erase(it);
        m_children.insert(it, newChild);
        return true;
    }
    return false;
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
    for(const OTMLNodePtr& child : m_children)
        child->setParent(nullptr);
    m_children.clear();
}

OTMLNodeList OTMLNode::children() const
{
    OTMLNodeList children;
    for(const OTMLNodePtr& child : m_children)
        if(!child->isNull())
            children.push_back(child);
    return children;
}

OTMLNodePtr OTMLNode::clone() const
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
    return OTMLEmitter::emitNode(shared_from_this(), 0);
}

