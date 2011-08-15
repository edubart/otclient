#ifndef OTMLEXCEPTION_H
#define OTMLEXCEPTION_H

#include "declarations.h"

/// All OTML errors throw this exception
class OTMLException : public std::exception
{
public:
    OTMLException(const OTMLNodePtr& node, const std::string& error);
    OTMLException(const OTMLDocumentPtr& doc, const std::string& error, int line = -1);
    virtual ~OTMLException() throw() { };

    virtual const char* what() const throw() { return m_what.c_str(); }

protected:
    OTMLException() { }
    void generateErrorMessage(const OTMLDocumentPtr& doc, const std::string& error, int line);
    void generateErrorMessage(const OTMLNodePtr& node, const std::string& error);
    std::string m_what;
};

#endif
