#ifndef OTMLDOCUMENT_H
#define OTMLDOCUMENT_H

#include "otmlnode.h"

class OTMLDocument : public OTMLNode
{
public:
    virtual ~OTMLDocument() { }

    /// Create a new OTML document for filling it with nodes
    static OTMLDocumentPtr create();

    /// Parse OTML from a file
    static OTMLDocumentPtr parse(const std::string& fileName);

    /// Parse OTML from input stream
    /// @param source is the file name that will be used to show errors messages
    static OTMLDocumentPtr parse(std::istream& in, const std::string& source);

    /// Emits this document and all it's children to a std::string
    std::string emit();

    /// Save this document to a file
    bool save(const std::string& fileName);

private:
    OTMLDocument() { }
};

#endif
