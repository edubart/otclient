#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "netdeclarations.h"
#include "inputmessage.h"
#include "outputmessage.h"

#include <script/luaobject.h>

class Protocol : public LuaObject
{
public:
    Protocol();

    void connect(const std::string& host, uint16 port);
    void send(OutputMessage& outputMessage);
    void recv();
    void internalRecvHeader(uint8* buffer, uint16 size);
    void internalRecvData(uint8* buffer, uint16 size);

    virtual void onConnect() = 0;
    virtual void onRecv(InputMessage& inputMessage) = 0;
    virtual void onError(const boost::system::error_code& err);

    ProtocolPtr asProtocol() { return std::static_pointer_cast<Protocol>(shared_from_this()); }

    virtual const char* getLuaTypeName() const { return "Protocol"; }

protected:
    uint32 m_xteaKey[4];
    bool m_checksumEnabled, m_xteaEncryptionEnabled;
    InputMessage m_inputMessage;

private:
    bool xteaDecrypt(InputMessage& inputMessage);
    void xteaEncrypt(OutputMessage& outputMessage);
    uint32 getAdlerChecksum(uint8* buffer, uint16 size);

    ConnectionPtr m_connection;
};

#endif
