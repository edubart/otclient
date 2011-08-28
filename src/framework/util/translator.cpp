/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#include "translator.h"
#include <boost/algorithm/string.hpp>

AlignmentFlag fw::translateAlignment(std::string aligment)
{
    boost::to_lower(aligment);
    boost::erase_all(aligment, " ");
    if(aligment == "topleft")
        return AlignTopLeft;
    else if(aligment == "topright")
        return AlignTopRight;
    else if(aligment == "bottomleft")
        return AlignBottomLeft;
    else if(aligment == "bottomright")
        return AlignBottomRight;
    else if(aligment == "left")
        return AlignLeftCenter;
    else if(aligment == "right")
        return AlignRightCenter;
    else if(aligment == "top")
        return AlignTopCenter;
    else if(aligment == "bottom")
        return AlignBottomCenter;
    else if(aligment == "center")
        return AlignCenter;
    return AlignNone;
}

AnchorEdge fw::translateAnchorEdge(std::string anchorEdge)
{
    boost::to_lower(anchorEdge);
    boost::erase_all(anchorEdge, " ");
    if(anchorEdge == "left")
        return AnchorLeft;
    else if(anchorEdge == "right")
        return AnchorRight;
    else if(anchorEdge == "top")
        return AnchorTop;
    else if(anchorEdge == "bottom")
        return AnchorBottom;
    else if(anchorEdge == "horizontalcenter")
        return AnchorHorizontalCenter;
    else if(anchorEdge == "verticalcenter")
        return AnchorVerticalCenter;
    return AnchorNone;
}