#include "otclient.h"

#include <framework/luascript/luainterface.h>
#include <otclient/core/tibiadat.h>
#include <otclient/core/tibiaspr.h>
#include <otclient/net/protocollogin.h>
#include <otclient/net/protocolgame.h>

void OTClient::registerLuaFunctions()
{
    // easy typing _1, _2, ...
    using namespace std::placeholders;

    g_lua.bindGlobalFunction("exit", std::bind(&OTClient::exit, &g_client));
    g_lua.bindGlobalFunction("setOnClose", std::bind(&OTClient::setOnClose, &g_client, _1));
    g_lua.bindGlobalFunction("importDat", std::bind(&TibiaDat::load, &g_tibiaDat, _1));
    g_lua.bindGlobalFunction("importSpr", std::bind(&TibiaSpr::load, &g_tibiaSpr, _1));

    g_lua.registerClass<ProtocolLogin, Protocol>();
    g_lua.bindClassStaticFunction<ProtocolLogin>("create", &ProtocolLogin::create);
    g_lua.bindClassMemberFunction("login", &ProtocolLogin::login);
    g_lua.bindClassMemberFunction("cancel", &ProtocolLogin::cancel);

    g_lua.registerClass<ProtocolGame, Protocol>();
}
