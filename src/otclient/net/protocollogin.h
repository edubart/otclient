#ifndef PROTOCOLLOGIN_H
#define PROTOCOLLOGIN_H

#include "declarations.h"
#include <framework/net/protocol.h>

class ProtocolLogin;
typedef std::shared_ptr<ProtocolLogin> ProtocolLoginPtr;

class ProtocolLogin : public Protocol
{
public:
    ProtocolLogin();

    static ProtocolLoginPtr create() { return ProtocolLoginPtr(new ProtocolLogin); }

    void login(const std::string& accountName, const std::string& accountPassword);

    void onConnect();
    void onRecv(InputMessage& inputMessage);
    void onError(const boost::system::error_code& error);

    void cancel() { /* TODO: this func */ }

    ProtocolLoginPtr asProtocolLogin() { return std::static_pointer_cast<ProtocolLogin>(shared_from_this()); }
    virtual const char* getLuaTypeName() const { return "ProtocolLogin"; }

private:
    void sendLoginPacket();

    void parseError(InputMessage& inputMessage);
    void parseMOTD(InputMessage& inputMessage);
    void parseCharacterList(InputMessage& inputMessage);

    std::string m_accountName, m_accountPassword;

};

#endif
