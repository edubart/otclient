#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "../const.h"
#include <string>

namespace fw {

AlignmentFlag translateAlignment(std::string aligment);
AnchorEdge translateAnchorEdge(const std::string& anchorPoint);

};

#endif
