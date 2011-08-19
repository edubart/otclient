#include "otmldocument.h"
#include "otmlparser.h"
#include "otmlemitter.h"

#include <framework/core/resourcemanager.h>

OTMLDocumentPtr OTMLDocument::create()
{
    OTMLDocumentPtr doc(new OTMLDocument);
    doc->setTag("doc");
    return doc;
}

OTMLDocumentPtr OTMLDocument::parse(const std::string& fileName)
{
    std::stringstream fin;
    g_resources.loadFile(fileName, fin);
    return parse(fin, g_resources.resolvePath(fileName));
}

OTMLDocumentPtr OTMLDocument::parse(std::istream& in, const std::string& source)
{
    OTMLDocumentPtr doc(new OTMLDocument);
    doc->setSource(source);
    OTMLParser parser(doc, in);
    parser.parse();
    return doc;
}

std::string OTMLDocument::emit()
{
    return OTMLEmitter::emitNode(shared_from_this());
}

bool OTMLDocument::save(const std::string& fileName)
{
    m_source = fileName;
    return g_resources.saveFile(fileName, emit());
}

