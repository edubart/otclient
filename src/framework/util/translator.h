#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "../const.h"
#include <string>

namespace fw {

AlignmentFlag translateAlignment(std::string aligment);
AnchorPoint translateAnchorPoint(const std::string& anchorPoint);

};

#endif
