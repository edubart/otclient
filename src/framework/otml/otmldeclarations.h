#ifndef OTMLDECLARATIONS_H
#define OTMLDECLARATIONS_H

#include <global.h>

class OTMLNode;
class OTMLDocument;
class OTMLParser;
class OTMLEmitter;

typedef std::shared_ptr<OTMLNode> OTMLNodePtr;
typedef std::shared_ptr<OTMLDocument> OTMLDocumentPtr;
typedef std::weak_ptr<OTMLNode> OTMLNodeWeakPtr;
typedef std::vector<OTMLNodePtr> OTMLNodeList;

#endif
