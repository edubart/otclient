/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
    std::unordered_map<OTMLNodePtr, OTMLNodePtr> parentMap;
    OTMLNodePtr previousNode;
    std::istream& in;
};

#endif
