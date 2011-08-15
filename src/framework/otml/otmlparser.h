#ifndef OTMLPARSER_H
#define OTMLPARSER_H

#include "declarations.h"

class OTMLParser
{
public:
    OTMLParser(OTMLDocumentPtr doc, std::istream& in);

    /// Parse the entire document
    void parse();

private:
    /// Retrieve next line from the input stream
    std::string getNextLine();
    /// Counts depth of a line (every 2 spaces increments one depth)
    int getLineDepth(const std::string& line, bool multilining = false);

    /// Parse each line of the input stream
    void parseLine(std::string line);
    /// Parse nodes tag and value
    void parseNode(const std::string& data);

    int currentDepth;
    int currentLine;
    OTMLDocumentPtr doc;
    OTMLNodePtr currentParent;
    OTMLNodePtr previousNode;
    std::istream& in;
};

#endif
