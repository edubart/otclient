#include "translator.h"
#include <boost/algorithm/string.hpp>

AlignmentFlag parseAlignment(std::string aligment)
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
