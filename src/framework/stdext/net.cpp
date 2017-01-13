/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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
#include <boost/asio/ip/address_v4.hpp>

namespace stdext {

std::string ip_to_string(uint32 ip)
{
    ip = boost::asio::detail::socket_ops::network_to_host_long(ip);
    boost::asio::ip::address_v4 address_v4 = boost::asio::ip::address_v4(ip);
    return address_v4.to_string();
}

uint32 string_to_ip(const std::string& string)
{
    boost::asio::ip::address_v4 address_v4 = boost::asio::ip::address_v4::from_string(string);
    return boost::asio::detail::socket_ops::host_to_network_long(address_v4.to_ulong());
}

std::vector<uint32> listSubnetAddresses(uint32 address, uint8 mask)
{
    std::vector<uint32> list;
    if(mask < 32) {
        uint32 bitmask = (0xFFFFFFFF >> mask);
        for(uint32 i = 0; i <= bitmask; i++) {
            uint32 ip = boost::asio::detail::socket_ops::host_to_network_long((boost::asio::detail::socket_ops::network_to_host_long(address) & (~bitmask)) | i);
            if((ip >> 24) != 0 && (ip >> 24) != 0xFF)
                list.push_back(ip);
        }
    }
    else
        list.push_back(address);

    return list;
}

}
