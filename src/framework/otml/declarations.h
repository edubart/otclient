#ifndef FRAMEWORK_OTML_DECLARATIONS_H
#define FRAMEWORK_OTML_DECLARATIONS_H

#include <framework/global.h>

class OTMLNode;
class OTMLDocument;
class OTMLParser;
class OTMLEmitter;

typedef std::shared_ptr<OTMLNode> OTMLNodePtr;
typedef std::shared_ptr<OTMLDocument> OTMLDocumentPtr;
typedef std::weak_ptr<OTMLNode> OTMLNodeWeakPtr;
typedef std::vector<OTMLNodePtr> OTMLNodeList;

#endif
