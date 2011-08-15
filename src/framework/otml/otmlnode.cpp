#include "otmlnode.h"
#include "otmlemitter.h"
#include "otmldocument.h"

OTMLNode::OTMLNode()
{
    m_unique = false;
}

std::string OTMLNode::tag() const
{
    return m_tag;
}

std::string OTMLNode::value() const
{
    // ~ is an alias for no value
    if(m_value == "~")
        return fw::empty_string;
    return m_value;
}

int OTMLNode::size() const
{
    return m_childNodes.size();
}

OTMLNodePtr OTMLNode::parent() const
{
    return m_parent.lock();
}

const OTMLNodeList& OTMLNode::childNodes() const
{
    return m_childNodes;
}

std::string OTMLNode::source() const
{
    return m_source;
}

bool OTMLNode::hasTag() const
{
    return !m_tag.empty();
}

bool OTMLNode::hasValue() const
{
    return (!m_value.empty() && m_value != "~");
}

bool OTMLNode::hasChildNodes() const
{
    return size() > 0;
}

bool OTMLNode::hasChild(const std::string& childTag) const
{
    return !!get(childTag);
}

bool OTMLNode::hasChild(int index) const
{
    return !!get(index);
}

bool OTMLNode::isUnique() const
{
    return m_unique;
}

void OTMLNode::setTag(std::string tag)
{
    m_tag = tag;

    // valued nodes that has tags are always unique
    if(!m_value.empty() && hasTag())
        setUnique();
}

void OTMLNode::setValue(const std::string& value)
{
    m_value = value;

    // valued nodes that has tags are always unique
    if(!m_value.empty() && hasTag())
        setUnique();
}

void OTMLNode::setParent(const OTMLNodePtr& parent)
{
    m_parent = parent;
}

void OTMLNode::setUnique(bool unique)
{
    m_unique = unique;
}

void OTMLNode::setSource(const std::string& source)
{
    m_source = source;
}

OTMLNodePtr OTMLNode::at(const std::string& childTag)
{
    for(const OTMLNodePtr& child : m_childNodes) {
        if(child->tag() == childTag)
            return child;
    }
    throw OTMLException(shared_from_this(), fw::mkstr("child node with tag '", childTag, "' not found"));
    return nullptr;
}

OTMLNodePtr OTMLNode::at(int childIndex)
{
    if(childIndex < size() && childIndex >= 0)
        return m_childNodes[childIndex];
    throw OTMLException(shared_from_this(), fw::mkstr("child node at index '", childIndex, "' not found"));
    return nullptr;
}

OTMLNodePtr OTMLNode::get(const std::string& childTag) const
{
    for(const OTMLNodePtr& child : m_childNodes) {
        if(child->tag() == childTag)
            return child;
    }
    return nullptr;
}

OTMLNodePtr OTMLNode::get(int childIndex) const
{
    if(childIndex < size() && childIndex >= 0)
        return m_childNodes[childIndex];
    return nullptr;
}

void OTMLNode::addChild(const OTMLNodePtr& newChild)
{
    // replace is needed when the tag is marked as unique
    if(newChild->hasTag()) {
        for(OTMLNodePtr node : m_childNodes) {
            if(node->tag() == newChild->tag() && (node->isUnique() || newChild->isUnique())) {
                newChild->setUnique();
                replaceChild(node, newChild);

                // remove any other child with the same tag
                auto it = m_childNodes.begin();
                while(it != m_childNodes.end()) {
                    OTMLNodePtr node = (*it);
                    if(node != newChild && node->tag() == newChild->tag()) {
                        node->setParent(nullptr);
                        it = m_childNodes.erase(it);
                    } else
                        ++it;
                }
                return;
            }
        }
    }

    m_childNodes.push_back(newChild);
    newChild->setParent(shared_from_this());
}

bool OTMLNode::removeChild(const OTMLNodePtr& oldChild)
{
    for(auto it = m_childNodes.begin(); it != m_childNodes.end(); ++it) {
        if((*it) == oldChild) {
            m_childNodes.erase(it);
            oldChild->setParent(nullptr);
            return true;
        }
    }
    return false;
}

bool OTMLNode::replaceChild(const OTMLNodePtr& oldChild, const OTMLNodePtr& newChild)
{
    for(auto it = m_childNodes.begin(); it != m_childNodes.end(); ++it) {
        if((*it) == oldChild) {
            oldChild->setParent(nullptr);
            newChild->setParent(shared_from_this());
            it = m_childNodes.erase(it);
            m_childNodes.insert(it, newChild);
            return true;
        }
    }
    return false;
}

void OTMLNode::clear()
{
    m_childNodes.clear();
}

void OTMLNode::merge(const OTMLNodePtr& node)
{
    for(const OTMLNodePtr& child : node->childNodes()) {
        OTMLNodePtr newNode(new OTMLNode);
        newNode->setUnique(child->isUnique());
        newNode->setTag(child->tag());
        newNode->setValue(child->value());
        addChild(newNode);
        newNode->merge(child);
    }
}

OTMLNodePtr OTMLNode::clone() const
{
    OTMLNodePtr myClone(new OTMLNode);
    myClone->setTag(tag());
    myClone->setValue(value());
    myClone->setUnique(isUnique());
    for(OTMLNodePtr child : childNodes())
        myClone->addChild(child->clone());
    return myClone;
}

std::string OTMLNode::emit()
{
    return OTMLEmitter::emitNode(shared_from_this(), 0);
}
