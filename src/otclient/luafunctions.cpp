#include "otclient.h"
#include <framework/luascript/luainterface.h>
#include <otclient/luascript/luavaluecasts.h>
#include <otclient/core/game.h>
#include <otclient/core/tile.h>
#include <otclient/core/item.h>
#include <otclient/core/creature.h>
#include <otclient/core/player.h>
#include <otclient/core/localplayer.h>
#include <otclient/core/map.h>
#include <otclient/core/thingstype.h>
#include <otclient/core/spritemanager.h>
#include <otclient/net/protocollogin.h>
#include <otclient/net/protocolgame.h>
#include <otclient/ui/uiitem.h>
#include <otclient/ui/uicreature.h>
#include <otclient/ui/uimap.h>
#include <otclient/ui/uigame.h>
#include <otclient/core/outfit.h>

void OTClient::registerLuaFunctions()
{
    Application::registerLuaFunctions();

    g_lua.bindGlobalFunction("exit", std::bind(&Application::exit, g_app));
    g_lua.bindGlobalFunction("importDat", std::bind(&ThingsType::load, &g_thingsType, _1));
    g_lua.bindGlobalFunction("importSpr", std::bind(&SpriteManager::load, &g_sprites, _1));
    g_lua.bindGlobalFunction("getOufitColor", Outfit::getColor);

    g_lua.registerClass<ProtocolLogin, Protocol>();
    g_lua.bindClassStaticFunction<ProtocolLogin>("create", &ProtocolLogin::create);
    g_lua.bindClassMemberFunction("login", &ProtocolLogin::login);
    g_lua.bindClassMemberFunction("cancelLogin", &ProtocolLogin::cancelLogin);

    g_lua.registerClass<ProtocolGame, Protocol>();

    g_lua.registerClass<Thing>();
    g_lua.bindClassMemberFunction<Thing>("getType", &Thing::getType);
    g_lua.bindClassMemberFunction<Thing>("isContainer", &Thing::isContainer);

    g_lua.registerClass<Creature, Thing>();
    g_lua.bindClassMemberFunction("setOutfit", &Creature::setOutfit);
    g_lua.bindClassMemberFunction("getOutfit", &Creature::getOutfit);

    g_lua.registerClass<Player, Creature>();
    g_lua.registerClass<LocalPlayer, Player>();
    g_lua.registerClass<Item, Thing>();
    g_lua.registerClass<Tile>();
    g_lua.registerClass<Map>();

    g_lua.registerClass<Game>();
    g_lua.bindClassStaticFunction<Game>("loginWorld", std::bind(&Game::loginWorld, &g_game, _1, _2, _3, _4, _5));
    g_lua.bindClassStaticFunction<Game>("logout", std::bind(&Game::logout, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("cancelLogin", std::bind(&Game::cancelLogin, &g_game));
    g_lua.bindClassStaticFunction<Game>("isOnline", std::bind(&Game::isOnline, &g_game));
    g_lua.bindClassStaticFunction<Game>("openOutfitWindow", std::bind(&Game::openOutfitWindow, &g_game));
    g_lua.bindClassStaticFunction<Game>("setOutfit", std::bind(&Game::setOutfit, &g_game, _1));

    g_lua.registerClass<UIItem, UIWidget>();
    g_lua.bindClassStaticFunction<UIItem>("create", &UIItem::create<UIItem>);
    g_lua.bindClassMemberFunction<UIItem>("getItem", &UIItem::getItem);
    g_lua.bindClassMemberFunction<UIItem>("setItem", &UIItem::setItem);

    g_lua.registerClass<UICreature, UIWidget>();
    g_lua.bindClassStaticFunction<UICreature>("create", &UICreature::create<UICreature>);
    g_lua.bindClassMemberFunction<UICreature>("getCreature", &UICreature::getCreature);
    g_lua.bindClassMemberFunction<UICreature>("setCreature", &UICreature::setCreature);

    g_lua.registerClass<UIMap, UIWidget>();
    g_lua.bindClassStaticFunction<UIMap>("create", &UIWidget::create<UIMap>);

    g_lua.registerClass<UIGame, UIWidget>();
    g_lua.bindClassStaticFunction<UIGame>("create", &UIWidget::create<UIGame>);

#ifdef FORBIDDEN_FUNCTIONS
    g_lua.bindClassStaticFunction<Game>("talkChannel", std::bind(&Game::talkChannel, &g_game, _1, _2, _3));
    g_lua.bindClassStaticFunction<Game>("talkPrivate", std::bind(&Game::talkPrivate, &g_game, _1, _2, _3));
#endif
}
