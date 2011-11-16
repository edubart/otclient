/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#include "otclient.h"

#include <framework/luascript/luainterface.h>
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
#include <otclient/core/outfit.h>

void push_luavalue(const Outfit& outfit);
bool luavalue_cast(int index, Outfit& outfit);

void OTClient::registerLuaFunctions()
{
    g_lua.bindGlobalFunction("exit", std::bind(&OTClient::exit, &g_client));
    g_lua.bindGlobalFunction("setOnClose", std::bind(&OTClient::setOnClose, &g_client, _1));
    g_lua.bindGlobalFunction("importDat", std::bind(&ThingsType::load, &g_thingsType, _1));
    g_lua.bindGlobalFunction("importSpr", std::bind(&SpriteManager::load, &g_sprites, _1));
    g_lua.bindGlobalFunction("getOufitColor", Outfit::getColor);

    g_lua.registerClass<ProtocolLogin, Protocol>();
    g_lua.bindClassStaticFunction<ProtocolLogin>("create", &ProtocolLogin::create);
    g_lua.bindClassMemberFunction("login", &ProtocolLogin::login);
    g_lua.bindClassMemberFunction("cancelLogin", &ProtocolLogin::cancelLogin);

    g_lua.registerClass<ProtocolGame, Protocol>();

    g_lua.registerClass<Thing>();

    g_lua.registerClass<Creature>();
    g_lua.bindClassMemberFunction("setOutfit", &Creature::setOutfit);
    g_lua.bindClassMemberFunction("getOutfit", &Creature::getOutfit);

    g_lua.registerClass<Player, Creature>();
    g_lua.registerClass<LocalPlayer, Player>();
    g_lua.registerClass<Item>();
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
    g_lua.bindClassMemberFunction<UICreature>("setOutfitType", &UICreature::setOutfitType);


    g_lua.registerClass<UIMap, UIWidget>();
    g_lua.bindClassStaticFunction<UIMap>("create", &UIWidget::create<UIMap>);

#ifdef FORBIDDEN_FUNCTIONS
    g_lua.bindClassStaticFunction<Game>("talkChannel", std::bind(&Game::talkChannel, &g_game, _1, _2, _3));
    g_lua.bindClassStaticFunction<Game>("talkPrivate", std::bind(&Game::talkPrivate, &g_game, _1, _2, _3));
#endif
}

void push_luavalue(const Outfit& outfit)
{
    g_lua.newTable();
    g_lua.pushInteger(outfit.getType());
    g_lua.setField("type");
    g_lua.pushInteger(outfit.getAddons());
    g_lua.setField("addons");
    g_lua.pushInteger(outfit.getHead());
    g_lua.setField("head");
    g_lua.pushInteger(outfit.getBody());
    g_lua.setField("body");
    g_lua.pushInteger(outfit.getLegs());
    g_lua.setField("legs");
    g_lua.pushInteger(outfit.getFeet());
    g_lua.setField("feet");
}

bool luavalue_cast(int index, Outfit& outfit)
{
    if(g_lua.isTable(index)) {
        g_lua.getField("type", index);
        outfit.setType(g_lua.popInteger());
        g_lua.getField("addons", index);
        outfit.setAddons(g_lua.popInteger());
        g_lua.getField("head", index);
        outfit.setHead(g_lua.popInteger());
        g_lua.getField("body", index);
        outfit.setBody(g_lua.popInteger());
        g_lua.getField("legs", index);
        outfit.setLegs(g_lua.popInteger());
        g_lua.getField("feet", index);
        outfit.setFeet(g_lua.popInteger());
        return true;
    }
    return false;
}
