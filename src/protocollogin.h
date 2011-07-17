#ifndef PROTOCOLLOGIN_H
#define PROTOCOLLOGIN_H

#include <net/protocol.h>

class ProtocolLogin : public Protocol
{
public:
    ProtocolLogin();
    ~ProtocolLogin();

    void login(const std::string& accountName, const std::string& accountPassword);

    void onConnect();

    void sendPacket();
    void onRecv(InputMessage *inputMessage);

    const char *getScriptObjectType() const { return "ProtocolLogin"; }

private:
    void parseError(InputMessage *inputMessage);
    void parseMOTD(InputMessage *inputMessage);
    void parseCharacterList(InputMessage *inputMessage);

    std::string m_accountName, m_accountPassword;

};

typedef boost::shared_ptr<ProtocolLogin> ProtocolLoginPtr;

#endif
