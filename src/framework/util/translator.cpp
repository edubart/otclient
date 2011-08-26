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