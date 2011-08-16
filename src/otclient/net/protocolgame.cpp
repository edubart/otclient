#include <otclient/net/protocolgame.h>
#include <otclient/core/game.h>

ProtocolGame::ProtocolGame()
{
    m_waitingLoginPacket = false;
}

void ProtocolGame::login(const std::string& accountName, const std::string& accountPassword, uint32 ip, uint16 port, const std::string& characterName)
{
    if(accountName.empty() || accountPassword.empty()) {
        callLuaField("onError", "You must enter an account name and password.");
        return;
    }

    m_waitingLoginPacket = true;
    m_accountName = accountName;
    m_accountPassword = accountPassword;
    m_characterName = characterName;

    char host[16];
    sprintf(host, "%d.%d.%d.%d", (uint8)ip, (uint8)(ip >> 8), (uint8)(ip >> 16), (uint8)(ip >> 24));

    connect(host, port);
}

void ProtocolGame::onConnect()
{
    recv();
}

void ProtocolGame::onRecv(InputMessage& inputMessage)
{
    if(m_waitingLoginPacket) {
        inputMessage.skipBytes(3);
        uint32 timestamp = inputMessage.getU32();
        uint8 unknown = inputMessage.getU8();

        m_waitingLoginPacket = false;
        enableChecksum();
        sendLoginPacket(timestamp, unknown);
        recv();
    }
    else {
        parseMessage(inputMessage);
        recv();
    }
}

void ProtocolGame::onError(const boost::system::error_code& error)
{
    // already disconnected, just fire onLogout
    g_game.onLogout();
}

