#include "otmlnode.h"

#include <boost/algorithm/string.hpp>

OTMLNode::OTMLNode(std::string what) :
    m_parent(0), m_line(0), m_what(what)
{
}

OTMLNode::~OTMLNode()
{
    for(int i=0;i<size();++i)
        delete at(i);
}

OTMLNode* OTMLNode::at(const std::string& childTag) const {
    int i=0;
    while(i<size() && at(i)->tag()!=childTag)
        ++i;
    return at(i);
}

OTMLNode* OTMLNode::at(int pos) const
{
    if(pos < (int)m_children.size() && pos >= 0)
        return m_children[pos];
    return 0;
}

OTMLNode* OTMLNode::atPath(const std::string& path) const
{
    std::vector<std::string> nodeTags;
    OTMLNode* node = const_cast<OTMLNode*>(this);
    std::string shortcutKey;

    boost::split(nodeTags, path, boost::is_any_of(std::string("/")));
    foreach(std::string& stag, nodeTags) {
        if(!shortcutKey.empty())
            shortcutKey += '.';
        shortcutKey += stag;
        if(node)
            node = node->at(stag);
    }

    if(node)
        return node;
    else
        return at(shortcutKey);
}

OTMLNode* OTMLNode::createNode(std::string tag)
{
    OTMLNode* node = new OTMLNode;
    node->setTag(tag);
    addNode(node);
    return node;
}

void OTMLNode::addNode(OTMLNode* node) {
    if(node->hasTag() && node->hasValue())
        if(OTMLNode* other = at(node->tag()))
            removeNode(other);
        m_children.push_back(node);
    node->setParent(this);
}

bool OTMLNode::removeNode(OTMLNode* node) {
    for(NodeList::iterator it = m_children.begin(); it != m_children.end(); ++it) {
        if((*it) == node) {
            m_children.erase(it);
            return true;
        }
    }
    return false;
}

std::string OTMLNode::generateErrorMessage(const std::string& message) const {
    std::stringstream ss;
    ss << "OTML error";
    if(!what().empty())
        ss << " in '" << what() << "'";
    if(m_line > 0)
        ss << " at line " << m_line;
    if(m_line > 0 && hasTag())
        ss << ", in node '" << tag() << "'";
    ss << ": " << message;
    return ss.str();
}

void OTMLNode::throwError(const std::string& message) const
{
    throw OTMLException(generateErrorMessage(message));
}
