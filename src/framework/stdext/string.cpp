/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

#include "string.h"
#include "format.h"
#include <boost/algorithm/string.hpp>
#include <ctype.h>
#include <physfs.h>
#include <boost/locale.hpp>

namespace stdext {

std::string resolve_path(const std::string& filePath, std::string sourcePath)
{
    if(stdext::starts_with(filePath, "/"))
        return filePath;
    if(!stdext::ends_with(sourcePath, "/")) {
        std::size_t slashPos = sourcePath.find_last_of("/");
        if(slashPos == std::string::npos)
            throw_exception(format("invalid source path '%s', for file '%s'", sourcePath, filePath));
        sourcePath = sourcePath.substr(0, slashPos + 1);
    }
    return sourcePath + filePath;
}

std::string date_time_string()
{
    char date[32];
    std::time_t tnow;
    std::time(&tnow);
    std::tm *ts = std::localtime(&tnow);
    std::strftime(date, 32, "%b %d %Y %H:%M:%S", ts);
    return std::string(date);
}

std::string dec_to_hex(uint64_t num)
{
    std::string str;
    std::ostringstream o;
    o << std::hex << num;
    str = o.str();
    return str;
}

uint64_t hex_to_dec(const std::string& str)
{
    uint64_t num;
    std::istringstream i(str);
    i >> std::hex >> num;
    return num;
}

bool is_valid_utf8(const std::string& src)
{
    try {
        boost::locale::conv::from_utf(src, "ISO-8859-1", boost::locale::conv::stop);
        return true;
    } catch(...) {
        return false;
    }
}

std::string utf8_to_latin1(const std::string& src)
{
    return boost::locale::conv::from_utf(src, "ISO-8859-1");
}

std::wstring utf8_to_utf16(const std::string& src)
{
    return boost::locale::conv::utf_to_utf<wchar_t>(src);
}

std::string utf16_to_utf8(const std::wstring& src)
{
    return boost::locale::conv::utf_to_utf<char>(src);
}

std::string utf16_to_latin1(const std::wstring& src)
{
    return boost::locale::conv::from_utf(src, "ISO-8859-1");
}

std::string latin1_to_utf8(const std::string& src)
{
    return boost::locale::conv::to_utf<char>(src, "ISO-8859-1");
}

std::wstring latin1_to_utf16(const std::string& src)
{
    return boost::locale::conv::to_utf<wchar_t>(src, "ISO-8859-1");
}

void tolower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), lochar);
}

void toupper(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), upchar);
}

void trim(std::string& str)
{
    boost::trim(str);
}

char upchar(char c)
{
    if((c >= 97 && c <= 122) || (uchar)c >= 224)
        c -= 32;
    return c;
}

char lochar(char c)
{
    if((c >= 65 && c <= 90) || ((uchar)c >= 192 && (uchar)c <= 223))
        c += 32;
    return c;
}

void ucwords(std::string& str)
{
    uint32 strLen = str.length();
    if(strLen == 0)
        return;

    str[0] = upchar(str[0]);
    for(uint32 i = 1; i < strLen; ++i) {
        if(str[i - 1] == ' ')
            str[i] = upchar(str[i]);
    }
}

bool ends_with(const std::string& str, const std::string& test)
{
    return boost::ends_with(str, test);
}

bool starts_with(const std::string& str, const std::string& test)
{
    return boost::starts_with(str, test);
}

void replace_all(std::string& str, const std::string& search, const std::string& replacement)
{
    return boost::replace_all(str, search, replacement);
}

std::vector<std::string> split(const std::string& str, const std::string& separators)
{
    std::vector<std::string> splitted;
    boost::split(splitted, str, boost::is_any_of(std::string(separators)));
    return splitted;
}

}
