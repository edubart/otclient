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


#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <net/connection.h>
#include <net/inputmessage.h>
#include <net/outputmessage.h>
#include <script/luaobject.h>

#define CIPSOFT_PUBLIC_RSA "1321277432058722840622950990822933849527763264961655079678763618" \
                           "4334395343554449668205332383339435179772895415509701210392836078" \
                           "6959821132214473291575712138800495033169914814069637740318278150" \
                           "2907336840325241747827401343576296990629870233111328210165697754" \
                           "88792221429527047321331896351555606801473202394175817"

//#define RSA "109120132967399429278860960508995541528237502902798129123468757937266291492576446330739696001110603907230888610072655818825358503429057592827629436413108566029093628212635953836686562675849720620786279431090218017681061521755056710823876476444260558147179707119674283982419152118103759076030616683978566631413"

class Protocol : public LuaObject
{
public:
    Protocol();

    void connect(const std::string& host, uint16 port, const std::function<void()>& callback);
    void send(OutputMessage *outputMessage);

    virtual void onRecv(InputMessage *inputMessage);
    virtual void onError(const boost::system::error_code& err);

    virtual const char* getLuaTypeName() const { return "Protocol"; }

protected:
    uint32 m_xteaKey[4];
    bool m_xteaEncryptionEnabled;

private:
    bool xteaDecrypt(InputMessage *inputMessage);
    void xteaEncrypt(OutputMessage *outputMessage);
    uint32 getAdlerChecksum(uint8 *buffer, uint16 size);

    ConnectionPtr m_connection;
};

typedef std::shared_ptr<Protocol> ProtocolPtr;

#endif
