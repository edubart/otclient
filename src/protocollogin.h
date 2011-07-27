#ifndef PROTOCOLLOGIN_H
#define PROTOCOLLOGIN_H

#include <net/protocol.h>

class ProtocolLogin;
typedef std::shared_ptr<ProtocolLogin> ProtocolLoginPtr;

class ProtocolLogin : public Protocol
{
public:
    ProtocolLogin();
    ~ProtocolLogin();

    static ProtocolLoginPtr create() { return ProtocolLoginPtr(new ProtocolLogin); }

    void login(const std::string& accountName, const std::string& accountPassword);

    void onConnect();

    void sendPacket();
    void onRecv(InputMessage* inputMessage);

    void cancel() { /* TODO: this func */ }

    ProtocolLoginPtr asProtocolLogin() { return std::static_pointer_cast<ProtocolLogin>(shared_from_this()); }

    virtual const char* getLuaTypeName() const { return "ProtocolLogin"; }

private:
    void parseError(InputMessage* inputMessage);
    void parseMOTD(InputMessage* inputMessage);
    void parseCharacterList(InputMessage* inputMessage);

    std::string m_accountName, m_accountPassword;

};

#endif
