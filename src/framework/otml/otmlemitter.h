#ifndef OTMLEMITTER_H
#define OTMLEMITTER_H

#include <string>

class OTMLNode;

class OTMLEmitter
{
public:
    OTMLEmitter();
    ~OTMLEmitter();

    OTMLNode* createDocument();
    std::string emitDocument();

    static std::string emitNodeValue(OTMLNode* node);
    static std::string emitNode(OTMLNode* node, int currentDepth = 0);

private:
    OTMLNode* m_rootNode;
};

#endif // OTMLEMITTER_H
