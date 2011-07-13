#include "convert.h"

bool safe_convert(const std::string& input, bool& b) {
    static std::string validNames[2][4] = {{"true","yes","on","1"}, {"false","no","off","0"}};
    bool ret = false;
    for(int i=0;i<4;++i) {
        if(input == validNames[0][i]) {
            b = true;
            ret = true;
            break;
        } else if(input == validNames[1][i]) {
            b = false;
            ret = true;
            break;
        }
    }
    return ret;
}

bool safe_convert(const std::string& input, std::string& output)
{
    output = input;
    return true;
}

bool safe_convert(const bool& b, std::string& out)
{
    out = (b ? "true" : "false");
    return true;
}
