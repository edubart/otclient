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

#include "protocoltibia87.h"
#include "../framework/util/rsa.h"

const char* ProtocolTibia87::rsa = "4673033022358411862216018001503683214873298680851934467521055526294025873980576"
                                   "6860224610646919605860206328024326703361630109888417839241959507572247284807035"
                                   "2355696191737922927869078457919049551036016528225191219083671878855092700253886"
                                   "41700821735345222087940578381210879116823013776808975766851829020659073";

ProtocolTibia87::ProtocolTibia87()
{

}

void ProtocolTibia87::begin()
{
    connect("icechaw.otland.net", 7171,
        [this](){
            this->afterConnect();
        }
    );
}

void ProtocolTibia87::login(const std::string& account, const std::string& password)
{
    sendAccount(account, password);
}

void ProtocolTibia87::sendAccount(const std::string& account, const std::string& password)
{
    NetworkMessagePtr networkMessage(new NetworkMessage);

    networkMessage->addByte(0x01);//login Server
    networkMessage->addU16(0x00);//OS
    networkMessage->addU16(0x00);//VERSION
    networkMessage->addPaddingBytes(12);//

    int32 m_notEncriptedLen = networkMessage->getMessageLength();

    //begin RSA encrypt
    networkMessage->addU32(1); //xtea
    networkMessage->addU32(2); //xtea
    networkMessage->addU32(3); //xtea
    networkMessage->addU32(4); //xtea

    networkMessage->addString(account);
    networkMessage->addString(password);

    Rsa::encrypt(networkMessage->getBodyBuffer() + m_notEncriptedLen, networkMessage->getMessageLength() - m_notEncriptedLen, ProtocolTibia87::rsa);

    networkMessage->setMessageLength(m_notEncriptedLen + 128);
    networkMessage->updateHeaderLength();

    send(networkMessage,
        [this](){
            this->afterSendAccount();
        }
    );
}

void ProtocolTibia87::afterConnect()
{
    login("9418347", "lollol");
}

void ProtocolTibia87::afterSendAccount()
{
    recv(
        [this](NetworkMessagePtr networkMessage){
            this->parseCharacterList(networkMessage);
        }
    );
}

void ProtocolTibia87::onError(const boost::system::error_code& error, const std::string& msg)
{
    logError("%s; %s", error.message().c_str(), msg.c_str());
}

void ProtocolTibia87::parseCharacterList(NetworkMessagePtr networkMessage)
{
    logInfo("Parsing characters. msglen: %d", networkMessage->getMessageLength());

}
