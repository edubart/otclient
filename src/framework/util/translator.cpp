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
    else
        return AlignCenter;
}

AnchorEdge fw::translateAnchorEdge(const std::string& anchorPoint)
{
    if(anchorPoint == "left")
        return AnchorLeft;
    else if(anchorPoint == "right")
        return AnchorRight;
    else if(anchorPoint == "top")
        return AnchorTop;
    else if(anchorPoint == "bottom")
        return AnchorBottom;
    else if(anchorPoint == "horizontalCenter")
        return AnchorHorizontalCenter;
    else if(anchorPoint == "verticalCenter")
        return AnchorVerticalCenter;
    return AnchorNone;
}