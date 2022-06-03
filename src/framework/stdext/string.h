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

#pragma once

#include <string>
#include <vector>

#include "cast.h"
#include "types.h"

namespace stdext
{
    template<typename T> std::string to_string(const T& t) { return unsafe_cast<std::string, T>(t); }
    template<typename T> T from_string(const std::string_view str, T def = T()) { return unsafe_cast<T, std::string_view>(str, def); }

    /// Resolve a file path by combining sourcePath with filePath
    std::string resolve_path(const std::string_view filePath, std::string_view sourcePath);
    /// Get current date and time in a std::string
    std::string date_time_string(const char* format = "%b %d %Y %H:%M:%S");

    std::string dec_to_hex(uint64_t num);
    uint64_t hex_to_dec(const std::string_view str);
    void tolower(std::string& str);
    void toupper(std::string& str);
    void ltrim(std::string& s);
    void rtrim(std::string& s);
    void trim(std::string& s);
    void ucwords(std::string& str);
    void eraseWhiteSpace(std::string& str);
    void replace_all(std::string& str, const std::string_view search, const std::string_view replacement);

    bool is_valid_utf8(const std::string_view src);
    std::string utf8_to_latin1(const std::string_view src);
    std::string latin1_to_utf8(const std::string_view src);

#ifdef WIN32
    std::wstring utf8_to_utf16(const std::string_view src);
    std::string utf16_to_utf8(const std::wstring_view src);
    std::string utf16_to_latin1(const std::wstring_view src);
    std::wstring latin1_to_utf16(const std::string_view src);
#endif

    // always returns at least one element in vector
    std::vector<std::string> split(const std::string_view str, const std::string_view separators = " ");
    template<typename T> std::vector<T> split(const std::string_view str, const std::string_view separators = " ")
    {
        const std::vector<std::string> splitted = split(str, separators);
        std::vector<T> results(splitted.size());
        for (uint i = 0; i < splitted.size(); ++i)
            results[i] = safe_cast<T>(splitted[i]);
        return results;
    }
}
