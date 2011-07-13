#include "otmlemitter.h"
#include "otmlnode.h"

#include <boost/algorithm/string.hpp>

OTMLEmitter::OTMLEmitter() :
    m_rootNode(0)
{
}

OTMLEmitter::~OTMLEmitter()
{
    if(m_rootNode)
        delete m_rootNode;
}

OTMLNode* OTMLEmitter::createDocument()
{
    m_rootNode = new OTMLNode;
    return m_rootNode;
}

std::string OTMLEmitter::emitDocument()
{
    if(m_rootNode)
        return emitNode(m_rootNode, 0);
    return std::string();
}

std::string OTMLEmitter::emitNodeValue(OTMLNode* node)
{
    std::string value = node->value();
    if(!value.empty() && (value[0] == '"' || *value.rbegin() == '"'||
                          value[0] == ' ' || *value.rbegin() == ' '||
                          value[0] == '-' || value[0] == '{' || value[0] == '[' || value[0] == '|' ||
                          value.find("\n") != std::string::npos)) {
        boost::replace_all(value, "\\", "\\\\");
        boost::replace_all(value, "\"", "\\\"");
        boost::replace_all(value, "\n", "\\n");
        value.append("\"");
        value.insert(0, "\"");
    }
    return value;
}

std::string OTMLEmitter::emitNode(OTMLNode* node, int currentDepth)
{
    std::stringstream ss;
    for(int i=1;i<currentDepth;++i)
        ss << "  ";
    if(currentDepth > 0) {
        if(node->hasTag())
            ss << node->tag();
        if(node->hasValue())
            ss << (node->hasTag() ? ": " : "- ") << emitNodeValue(node);
        if(!node->hasTag() && !node->hasValue())
            ss << "-";
        ss << std::endl;
    }
    for(int i=0;i<node->size();++i)
        ss << emitNode(node->at(i), currentDepth+1);
    return ss.str();
}
