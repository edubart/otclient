/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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

#ifndef PROTOCOLTIBIA87_H
#define PROTOCOLTIBIA87_H

#include "prerequisites.h"
#include "net/protocol.h"

class ProtocolTibia87 : public Protocol
{
public:
    ProtocolTibia87();

    virtual void begin();

protected:
    void login(const std::string& account, const std::string& password);
    void sendAccount(const std::string& account, const std::string& password);
    void parseCharacterList(NetworkMessagePtr networkMessage);

    void readCharacterList();

    void afterConnect();
    void afterSendAccount();

    virtual void onError(const boost::system::error_code& error, const std::string& msg);

private:
    static const char* rsa;
};

typedef std::shared_ptr<ProtocolTibia87> ProtocolTibia87Ptr;

#endif //PROTOCOLTIBIA87_H