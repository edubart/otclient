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

#include "otmlexception.h"
#include "otmldocument.h"

OTMLException::OTMLException(const OTMLNodePtr& node, const std::string& error)
{
    std::stringstream ss;
    ss << "OTML error";
    if(!node->source().empty())
        ss << " in '" << node->source() << "'";
    ss << ": " << error;
    m_what = ss.str();
}

OTMLException::OTMLException(const OTMLDocumentPtr& doc, const std::string& error, int line)
{
    std::stringstream ss;
    ss << "OTML error";
    if(doc && !doc->source().empty()) {
        ss  << " in '" << doc->source() << "'";
        if(line >= 0)
            ss << " at line " << line;
    }
    ss << ": "  << error;
    m_what = ss.str();
}
