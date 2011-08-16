#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "declarations.h"
#include "inputmessage.h"
#include "outputmessage.h"

#include <framework/luascript/luaobject.h>

class Protocol : public LuaObject
{
public:
    Protocol();

    void connect(const std::string& host, uint16 port);
    void disconnect();

    void send(OutputMessage& outputMessage);
    void recv();

    void internalRecvHeader(uint8* buffer, uint16 size);
    void internalRecvData(uint8* buffer, uint16 size);

    virtual void onConnect() = 0;
    virtual void onRecv(InputMessage& inputMessage) = 0;
    virtual void onError(const boost::system::error_code& err) = 0;

    ProtocolPtr asProtocol() { return std::static_pointer_cast<Protocol>(shared_from_this()); }

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
