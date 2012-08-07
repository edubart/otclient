/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#include "net.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <arpa/inet.h>

namespace stdext {

std::string ip_to_string(uint32 ip)
{
    char host[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip, host, INET_ADDRSTRLEN);
    return std::string(host);
}

uint32 string_to_ip(const std::string& string)
{
    uint32 ip = 0;
    inet_pton(AF_INET, string.c_str(), &ip);
    return ip;
}

std::vector<uint32> listSubnetAddresses(const std::string& subnet)
{
    std::vector<uint32> list;
    std::vector<std::string> strVec;
    boost::split(strVec, subnet, boost::is_any_of("/"));
    uint32 address = string_to_ip(strVec[0]);
    if(address != INADDR_NONE && strVec.size() == 2) {
        uint32 mask = boost::lexical_cast<uint32>(strVec[1]);
        if(mask <= 32) {
            for(uint32 i=0; i <= (0xFFFFFFFF >> mask); i++) {
                uint32 ip = htonl((htonl(address) & (~(0xFFFFFFFF >> mask))) | i);
                if((ip >> 24) != 0 && (ip >> 24) != 0xFF) {
                    list.push_back(ip);
                }
            }
        }
    }
    return list;
}

}
