#include "otmlexception.h"
#include "otmldocument.h"

OTMLException::OTMLException(const OTMLNodePtr& node, const std::string& error)
{
    std::stringstream ss;
    ss << "OTML error";
    if(!node->source().empty())
        ss << " in '" << node->source() << "'";
    ss << ": " << error;
    m_what = ss.str();
}

OTMLException::OTMLException(const OTMLDocumentPtr& doc, const std::string& error, int line)
{
    std::stringstream ss;
    ss << "OTML error";
    if(doc && !doc->source().empty()) {
        ss  << " in '" << doc->source() << "'";
        if(line >= 0)
            ss << " at line " << line;
    }
    ss << ": "  << error;
    m_what = ss.str();
}
