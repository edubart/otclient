#ifndef OTMLEMITTER_H
#define OTMLEMITTER_H

#include "otmldeclarations.h"

class OTMLEmitter
{
public:
    /// Emits a node and it's children to a std::string
    static std::string emitNode(const OTMLNodePtr& node, int currentDepth = -1);
};

#endif
