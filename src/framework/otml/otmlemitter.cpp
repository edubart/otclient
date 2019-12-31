/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include "otmlemitter.h"
#include "otmldocument.h"

std::string OTMLEmitter::emitNode(const OTMLNodePtr& node, int currentDepth)
{
    std::stringstream ss;

    // emit nodes
    if(currentDepth >= 0) {
        // fill spaces for current depth
        for(int i=0;i<currentDepth;++i)
            ss << "  ";

        // emit node tag
        if(node->hasTag()) {
            ss << node->tag();

            // add ':' to if the node is unique or has value
            if(node->hasValue() || node->isUnique() || node->isNull())
                ss << ":";
        } else
            ss << "-";

        // emit node value
        if(node->isNull())
            ss << " ~";
        else if(node->hasValue()) {
            ss << " ";

            std::string value = node->value();

            // emit multiline values
            if(value.find("\n") != std::string::npos) {
                if(value[value.length()-1] == '\n' && value[value.length()-2] == '\n')
                    ss << "|+";
                else if(value[value.length()-1] == '\n')
                    ss << "|";
                else
                    ss << "|-";

                //  multilines
                for(std::size_t pos = 0; pos < value.length(); ++pos) {
                    ss << "\n";

                    // fill spaces for multiline depth
                    for(int i=0;i<currentDepth+1;++i)
                        ss << "  ";

                    // fill until a new line
                    while(pos < value.length()) {
                        if(value[pos] == '\n')
                            break;
                        ss << value[pos++];
                    }
                }
            // emit inline values
            } else
                ss << value;
        }
    }

    // emit children
    for(int i=0;i<node->size();++i) {
        if(currentDepth >= 0 || i != 0)
            ss << "\n";
        ss << emitNode(node->atIndex(i), currentDepth+1);
    }

    return ss.str();
}
