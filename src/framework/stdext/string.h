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

#ifndef STDEXT_STRING_H
#define STDEXT_STRING_H

#include <string>
#include <cstring>
#include <vector>

#include "types.h"
#include "cast.h"

namespace stdext {

template<typename T> std::string to_string(const T& t) { return unsafe_cast<std::string, T>(t); }
template<typename T> T from_string(const std::string& str, T def = T()) { return unsafe_cast<T, std::string>(str, def); }

/// Resolve a file path by combining sourcePath with filePath
std::string resolve_path(const std::string& filePath, std::string sourcePath);
/// Get current date and time in a std::string
std::string date_time_string(const char* format = "%b %d %Y %H:%M:%S");

std::string dec_to_hex(uint64_t num);
uint64_t hex_to_dec(const std::string& str);
void tolower(std::string& str);
void toupper(std::string& str);
void trim(std::string& str);
void ucwords(std::string& str);
char upchar(char c);
char lochar(char c);
bool ends_with(const std::string& str, const std::string& test);
bool starts_with(const std::string& str, const std::string& test);
void replace_all(std::string& str, const std::string& search, const std::string& replacement);

bool is_valid_utf8(const std::string& src);
std::string utf8_to_latin1(const std::string& src);
std::string latin1_to_utf8(const std::string& src);

#ifdef WIN32
std::wstring utf8_to_utf16(const std::string& src);
std::string utf16_to_utf8(const std::wstring& src);
std::string utf16_to_latin1(const std::wstring& src);
std::wstring latin1_to_utf16(const std::string& src);
#endif

// always returns at least one element in vector
std::vector<std::string> split(const std::string& str, const std::string& separators = " ");
template<typename T> std::vector<T> split(const std::string& str, const std::string& separators = " ") {
    std::vector<std::string> splitted = split(str, separators);
    std::vector<T> results(splitted.size());
    for(uint i = 0; i < splitted.size(); ++i)
        results[i] = safe_cast<T>(splitted[i]);
    return results;
}

}

#endif
