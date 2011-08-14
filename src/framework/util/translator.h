#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <const.h>
#include <string>

AlignmentFlag parseAlignment(std::string aligment);
AnchorPoint parseAnchorPoint(const std::string& anchorPoint);
#endif
