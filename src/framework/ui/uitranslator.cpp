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

#include "uitranslator.h"
#include <framework/stdext/string.h>
#include <boost/algorithm/string.hpp>

Fw::AlignmentFlag Fw::translateAlignment(std::string aligment)
{
    boost::to_lower(aligment);
    boost::erase_all(aligment, " ");
    if(aligment == "topleft")
        return Fw::AlignTopLeft;
    else if(aligment == "topright")
        return Fw::AlignTopRight;
    else if(aligment == "bottomleft")
        return Fw::AlignBottomLeft;
    else if(aligment == "bottomright")
        return Fw::AlignBottomRight;
    else if(aligment == "left")
        return Fw::AlignLeftCenter;
    else if(aligment == "right")
        return Fw::AlignRightCenter;
    else if(aligment == "top")
        return Fw::AlignTopCenter;
    else if(aligment == "bottom")
        return Fw::AlignBottomCenter;
    else if(aligment == "center")
        return Fw::AlignCenter;
    return Fw::AlignNone;
}

Fw::AnchorEdge Fw::translateAnchorEdge(std::string anchorEdge)
{
    boost::to_lower(anchorEdge);
    boost::erase_all(anchorEdge, " ");
    if(anchorEdge == "left")
        return Fw::AnchorLeft;
    else if(anchorEdge == "right")
        return Fw::AnchorRight;
    else if(anchorEdge == "top")
        return Fw::AnchorTop;
    else if(anchorEdge == "bottom")
        return Fw::AnchorBottom;
    else if(anchorEdge == "horizontalcenter")
        return Fw::AnchorHorizontalCenter;
    else if(anchorEdge == "verticalcenter")
        return Fw::AnchorVerticalCenter;
    return Fw::AnchorNone;
}

Fw::WidgetState Fw::translateState(std::string state)
{
    boost::to_lower(state);
    boost::trim(state);
    if(state == "active")
        return Fw::ActiveState;
    else if(state == "focus")
        return Fw::FocusState;
    else if(state == "hover")
        return Fw::HoverState;
    else if(state == "pressed")
        return Fw::PressedState;
    else if(state == "checked")
        return Fw::CheckedState;
    else if(state == "disabled")
        return Fw::DisabledState;
    else if(state == "on")
        return Fw::OnState;
    else if(state == "first")
        return Fw::FirstState;
    else if(state == "middle")
        return Fw::MiddleState;
    else if(state == "last")
        return Fw::LastState;
    else if(state == "alternate")
        return Fw::AlternateState;
    else if(state == "dragging")
        return Fw::DraggingState;
    else if(state == "hidden")
        return Fw::HiddenState;
    return Fw::InvalidState;
}

Fw::AutoFocusPolicy Fw::translateAutoFocusPolicy(std::string policy)
{
    boost::to_lower(policy);
    boost::trim(policy);
    if(policy == "first")
        return Fw::AutoFocusFirst;
    else if(policy == "last")
        return Fw::AutoFocusLast;
    return Fw::AutoFocusNone;
}
