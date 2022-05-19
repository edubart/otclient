/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

Fw::AlignmentFlag Fw::translateAlignment(std::string aligment)
{
    stdext::tolower(aligment);
    stdext::eraseWhiteSpace(aligment);

    if (aligment == "topleft")
        return AlignTopLeft;

    if (aligment == "topright")
        return AlignTopRight;

    if (aligment == "bottomleft")
        return AlignBottomLeft;

    if (aligment == "bottomright")
        return AlignBottomRight;

    if (aligment == "left")
        return AlignLeftCenter;

    if (aligment == "right")
        return AlignRightCenter;

    if (aligment == "top")
        return AlignTopCenter;

    if (aligment == "bottom")
        return AlignBottomCenter;

    if (aligment == "center")
        return AlignCenter;

    return AlignNone;
}

Fw::AnchorEdge Fw::translateAnchorEdge(std::string anchorEdge)
{
    stdext::tolower(anchorEdge);
    stdext::eraseWhiteSpace(anchorEdge);

    if (anchorEdge == "left")
        return AnchorLeft;

    if (anchorEdge == "right")
        return AnchorRight;

    if (anchorEdge == "top")
        return AnchorTop;

    if (anchorEdge == "bottom")
        return AnchorBottom;

    if (anchorEdge == "horizontalcenter")
        return AnchorHorizontalCenter;

    if (anchorEdge == "verticalcenter")
        return AnchorVerticalCenter;

    return AnchorNone;
}

Fw::WidgetState Fw::translateState(std::string state)
{
    stdext::tolower(state);
    stdext::trim(state);
    if (state == "active")
        return ActiveState;
    if (state == "focus")
        return FocusState;
    if (state == "hover")
        return HoverState;
    if (state == "pressed")
        return PressedState;
    if (state == "checked")
        return CheckedState;
    if (state == "disabled")
        return DisabledState;
    if (state == "on")
        return OnState;
    if (state == "first")
        return FirstState;
    if (state == "middle")
        return MiddleState;
    if (state == "last")
        return LastState;
    if (state == "alternate")
        return AlternateState;
    if (state == "dragging")
        return DraggingState;
    if (state == "hidden")
        return HiddenState;
    return InvalidState;
}

Fw::AutoFocusPolicy Fw::translateAutoFocusPolicy(std::string policy)
{
    stdext::tolower(policy);
    stdext::trim(policy);
    if (policy == "first")
        return AutoFocusFirst;
    if (policy == "last")
        return AutoFocusLast;
    return AutoFocusNone;
}
