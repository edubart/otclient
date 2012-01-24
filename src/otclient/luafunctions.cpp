/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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
#include <otclient/luascript/luavaluecasts.h>
#include <otclient/core/game.h>
#include <otclient/core/tile.h>
#include <otclient/core/item.h>
#include <otclient/core/effect.h>
#include <otclient/core/missile.h>
#include <otclient/core/statictext.h>
#include <otclient/core/animatedtext.h>
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

    g_lua.registerStaticClass("g_thingsType");
    g_lua.bindClassStaticFunction("g_thingsType", "load", std::bind(&ThingsType::load, &g_thingsType, _1));
    g_lua.bindClassStaticFunction("g_thingsType", "isLoaded", std::bind(&ThingsType::isLoaded, &g_thingsType));
    g_lua.bindClassStaticFunction("g_thingsType", "getSignature", std::bind(&ThingsType::getSignature, &g_thingsType));

    g_lua.registerStaticClass("g_sprites");
    g_lua.bindClassStaticFunction("g_sprites", "load", std::bind(&SpriteManager::load, &g_sprites, _1));
    g_lua.bindClassStaticFunction("g_sprites", "isLoaded", std::bind(&SpriteManager::isLoaded, &g_sprites));
    g_lua.bindClassStaticFunction("g_sprites", "getSignature", std::bind(&SpriteManager::getSignature, &g_sprites));

    g_lua.registerStaticClass("g_map");
    g_lua.bindClassStaticFunction("g_map", "getFirstVisibleFloor", std::bind(&Map::getFirstVisibleFloor, &g_map));
    g_lua.bindClassStaticFunction("g_map", "isLookPossible", std::bind(&Map::isLookPossible, &g_map, _1));
    g_lua.bindClassStaticFunction("g_map", "isCovered", std::bind(&Map::isCovered, &g_map, _1, _2));
    g_lua.bindClassStaticFunction("g_map", "isCompletlyCovered", std::bind(&Map::isCompletlyCovered, &g_map, _1, _2));
    g_lua.bindClassStaticFunction("g_map", "addThing", std::bind(&Map::addThing, &g_map, _1, _2, _3));
    g_lua.bindClassStaticFunction("g_map", "getThing", std::bind(&Map::getThing, &g_map, _1, _2));
    g_lua.bindClassStaticFunction("g_map", "removeThingByPos", std::bind(&Map::removeThingByPos, &g_map, _1, _2));
    g_lua.bindClassStaticFunction("g_map", "removeThing", std::bind(&Map::removeThing, &g_map, _1));
    g_lua.bindClassStaticFunction("g_map", "cleanTile", std::bind(&Map::cleanTile, &g_map, _1));
    g_lua.bindClassStaticFunction("g_map", "getTile", std::bind(&Map::getTile, &g_map, _1));
    g_lua.bindClassStaticFunction("g_map", "setCentralPosition", std::bind(&Map::setCentralPosition, &g_map, _1));
    g_lua.bindClassStaticFunction("g_map", "getCentralPosition", std::bind(&Map::getCentralPosition, &g_map));
    g_lua.bindClassStaticFunction("g_map", "addCreature", std::bind(&Map::addCreature, &g_map, _1));
    g_lua.bindClassStaticFunction("g_map", "getCreatureById", std::bind(&Map::getCreatureById, &g_map, _1));
    g_lua.bindClassStaticFunction("g_map", "removeCreatureById", std::bind(&Map::removeCreatureById, &g_map, _1));
    g_lua.bindClassStaticFunction("g_map", "setVisibleSize", std::bind(&Map::setVisibleSize, &g_map, _1));
    g_lua.bindClassStaticFunction("g_map", "getVibibleSize", std::bind(&Map::getVibibleSize, &g_map));
    g_lua.bindClassStaticFunction("g_map", "getCentralOffset", std::bind(&Map::getCentralOffset, &g_map));
    g_lua.bindClassStaticFunction("g_map", "positionTo2D", std::bind(&Map::positionTo2D, &g_map, _1));

    g_lua.bindGlobalFunction("getOufitColor", Outfit::getColor);

    g_lua.registerClass<ProtocolLogin, Protocol>();
    g_lua.bindClassStaticFunction<ProtocolLogin>("create", &ProtocolLogin::create);
    g_lua.bindClassMemberFunction<ProtocolLogin>("login", &ProtocolLogin::login);
    g_lua.bindClassMemberFunction<ProtocolLogin>("cancelLogin", &ProtocolLogin::cancelLogin);

    g_lua.registerClass<ProtocolGame, Protocol>();

    g_lua.registerClass<Thing>();
    g_lua.bindClassMemberFunction<Thing>("setId", &Thing::setId);
    g_lua.bindClassMemberFunction<Thing>("setPos", &Thing::setPos);
    g_lua.bindClassMemberFunction<Thing>("getId", &Thing::getId);
    g_lua.bindClassMemberFunction<Thing>("getPos", &Thing::getPos);
    g_lua.bindClassMemberFunction<Thing>("getStackPriority", &Thing::getStackPriority);
    g_lua.bindClassMemberFunction<Thing>("getAnimationPhases", &Thing::getAnimationPhases);
    g_lua.bindClassMemberFunction<Thing>("setXPattern", &Thing::setXPattern);
    g_lua.bindClassMemberFunction<Thing>("setYPattern", &Thing::setYPattern);
    g_lua.bindClassMemberFunction<Thing>("setZPattern", &Thing::setZPattern);
    g_lua.bindClassMemberFunction<Thing>("asThing", &Thing::asThing);
    g_lua.bindClassMemberFunction<Thing>("asItem", &Thing::asItem);
    g_lua.bindClassMemberFunction<Thing>("asCreature", &Thing::asCreature);
    g_lua.bindClassMemberFunction<Thing>("asEffect", &Thing::asEffect);
    g_lua.bindClassMemberFunction<Thing>("asMissile", &Thing::asMissile);
    g_lua.bindClassMemberFunction<Thing>("asPlayer", &Thing::asPlayer);
    g_lua.bindClassMemberFunction<Thing>("asLocalPlayer", &Thing::asLocalPlayer);
    g_lua.bindClassMemberFunction<Thing>("asAnimatedText", &Thing::asAnimatedText);
    g_lua.bindClassMemberFunction<Thing>("asStaticText", &Thing::asStaticText);
    g_lua.bindClassMemberFunction<Thing>("isGround", &Thing::isGround);
    g_lua.bindClassMemberFunction<Thing>("isGroundBorder", &Thing::isGroundBorder);
    g_lua.bindClassMemberFunction<Thing>("isOnBottom", &Thing::isOnBottom);
    g_lua.bindClassMemberFunction<Thing>("isOnTop", &Thing::isOnTop);
    g_lua.bindClassMemberFunction<Thing>("isContainer", &Thing::isContainer);
    g_lua.bindClassMemberFunction<Thing>("isForceUse", &Thing::isForceUse);
    g_lua.bindClassMemberFunction<Thing>("isMultiUse", &Thing::isMultiUse);
    g_lua.bindClassMemberFunction<Thing>("isRotateable", &Thing::isRotateable);
    g_lua.bindClassMemberFunction<Thing>("isNotMoveable", &Thing::isNotMoveable);
    g_lua.bindClassMemberFunction<Thing>("isPickupable", &Thing::isPickupable);
    g_lua.bindClassMemberFunction<Thing>("ignoreLook", &Thing::ignoreLook);
    g_lua.bindClassMemberFunction<Thing>("isStackable", &Thing::isStackable);

    g_lua.registerClass<Creature, Thing>();
    g_lua.bindClassMemberFunction<Creature>("getName", &Creature::getName);
    g_lua.bindClassMemberFunction<Creature>("getShield", &Creature::getShield);
    g_lua.bindClassMemberFunction<Creature>("setOutfit", &Creature::setOutfit);
    g_lua.bindClassMemberFunction<Creature>("getOutfit", &Creature::getOutfit);
    g_lua.bindClassMemberFunction<Creature>("setSkullTexture", &Creature::setSkullTexture);
    g_lua.bindClassMemberFunction<Creature>("setShieldTexture", &Creature::setShieldTexture);
    g_lua.bindClassMemberFunction<Creature>("setEmblemTexture", &Creature::setEmblemTexture);
    g_lua.bindClassMemberFunction<Creature>("asMonster", &Creature::asMonster);

    g_lua.registerClass<Item, Thing>();
    g_lua.bindClassStaticFunction<Item>("create", &Item::create);
    g_lua.bindClassMemberFunction<Item>("getData", &Item::getData);

    g_lua.registerClass<Effect, Thing>();
    g_lua.registerClass<Missile, Thing>();
    g_lua.registerClass<StaticText, Thing>();
    g_lua.registerClass<AnimatedText, Thing>();

    g_lua.registerClass<Player, Creature>();
    g_lua.registerClass<Npc, Creature>();
    g_lua.registerClass<Monster, Creature>();
    g_lua.registerClass<LocalPlayer, Player>();
    g_lua.bindClassMemberFunction<LocalPlayer>("getAttackingCreature", &LocalPlayer::getAttackingCreature);
    g_lua.bindClassMemberFunction<LocalPlayer>("getFollowingCreature", &LocalPlayer::getFollowingCreature);
    g_lua.bindClassMemberFunction<LocalPlayer>("getHealth", &LocalPlayer::getHealth);
    g_lua.bindClassMemberFunction<LocalPlayer>("getMaxHealth", &LocalPlayer::getMaxHealth);
    g_lua.bindClassMemberFunction<LocalPlayer>("getFreeCapacity", &LocalPlayer::getFreeCapacity);
    g_lua.bindClassMemberFunction<LocalPlayer>("getExperience", &LocalPlayer::getExperience);
    g_lua.bindClassMemberFunction<LocalPlayer>("getLevel", &LocalPlayer::getLevel);
    g_lua.bindClassMemberFunction<LocalPlayer>("getLevelPercent", &LocalPlayer::getLevelPercent);
    g_lua.bindClassMemberFunction<LocalPlayer>("getMana", &LocalPlayer::getMana);
    g_lua.bindClassMemberFunction<LocalPlayer>("getMaxMana", &LocalPlayer::getMaxMana);
    g_lua.bindClassMemberFunction<LocalPlayer>("getMagicLevel", &LocalPlayer::getMagicLevel);
    g_lua.bindClassMemberFunction<LocalPlayer>("getMagicLevelPercent", &LocalPlayer::getMagicLevelPercent);
    g_lua.bindClassMemberFunction<LocalPlayer>("getSoul", &LocalPlayer::getSoul);
    g_lua.bindClassMemberFunction<LocalPlayer>("getStamina", &LocalPlayer::getStamina);

    g_lua.registerClass<Tile>();
    g_lua.bindClassMemberFunction<Tile>("clean", &Tile::clean);
    g_lua.bindClassMemberFunction<Tile>("addThing", &Tile::addThing);
    g_lua.bindClassMemberFunction<Tile>("getThing", &Tile::getThing);
    g_lua.bindClassMemberFunction<Tile>("getThingStackpos", &Tile::getThingStackpos);
    g_lua.bindClassMemberFunction<Tile>("getTopThing", &Tile::getTopThing);
    g_lua.bindClassMemberFunction<Tile>("removeThingByStackpos", &Tile::removeThingByStackpos);
    g_lua.bindClassMemberFunction<Tile>("removeThing", &Tile::removeThing);
    g_lua.bindClassMemberFunction<Tile>("getTopLookThing", &Tile::getTopLookThing);
    g_lua.bindClassMemberFunction<Tile>("getTopUseThing", &Tile::getTopUseThing);
    g_lua.bindClassMemberFunction<Tile>("getTopCreature", &Tile::getTopCreature);
    g_lua.bindClassMemberFunction<Tile>("getTopMoveThing", &Tile::getTopMoveThing);
    g_lua.bindClassMemberFunction<Tile>("getTopMultiUseThing", &Tile::getTopMultiUseThing);
    g_lua.bindClassMemberFunction<Tile>("getPos", &Tile::getPos);
    g_lua.bindClassMemberFunction<Tile>("getDrawElevation", &Tile::getDrawElevation);
    g_lua.bindClassMemberFunction<Tile>("getCreatures", &Tile::getCreatures);
    g_lua.bindClassMemberFunction<Tile>("getGround", &Tile::getGround);
    g_lua.bindClassMemberFunction<Tile>("isWalkable", &Tile::isWalkable);
    g_lua.bindClassMemberFunction<Tile>("isFullGround", &Tile::isFullGround);
    g_lua.bindClassMemberFunction<Tile>("isFullyOpaque", &Tile::isFullyOpaque);
    g_lua.bindClassMemberFunction<Tile>("isLookPossible", &Tile::isLookPossible);
    g_lua.bindClassMemberFunction<Tile>("hasCreature", &Tile::hasCreature);
    g_lua.bindClassMemberFunction<Tile>("isEmpty", &Tile::isEmpty);
    g_lua.bindClassMemberFunction<Tile>("isClickable", &Tile::isClickable);

    g_lua.registerClass<Map>();

    g_lua.registerClass<Game>();
    g_lua.bindClassStaticFunction<Game>("loginWorld", std::bind(&Game::loginWorld, &g_game, _1, _2, _3, _4, _5));
    g_lua.bindClassStaticFunction<Game>("logout", std::bind(&Game::logout, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("cancelLogin", std::bind(&Game::cancelLogin, &g_game));
    g_lua.bindClassStaticFunction<Game>("isOnline", std::bind(&Game::isOnline, &g_game));
    g_lua.bindClassStaticFunction<Game>("requestOutfit", std::bind(&Game::requestOutfit, &g_game));
    g_lua.bindClassStaticFunction<Game>("requestChannels", std::bind(&Game::requestChannels, &g_game));
    g_lua.bindClassStaticFunction<Game>("joinChannel", std::bind(&Game::joinChannel, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("setOutfit", std::bind(&Game::setOutfit, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("look", std::bind(&Game::look, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("open", std::bind(&Game::open, &g_game, _1, _2));
    g_lua.bindClassStaticFunction<Game>("use", std::bind(&Game::use, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("useWith", std::bind(&Game::useWith, &g_game, _1, _2));
    g_lua.bindClassStaticFunction<Game>("move", std::bind(&Game::move, &g_game, _1, _2, _3));
    g_lua.bindClassStaticFunction<Game>("useInventoryItem", std::bind(&Game::useInventoryItem, &g_game, _1, _2));
    g_lua.bindClassStaticFunction<Game>("walk", std::bind(&Game::walk, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("forceWalk", std::bind(&Game::forceWalk, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("attack", std::bind(&Game::attack, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("cancelAttack", std::bind(&Game::cancelAttack, &g_game));
    g_lua.bindClassStaticFunction<Game>("follow", std::bind(&Game::follow, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("cancelFollow", std::bind(&Game::cancelFollow, &g_game));
    g_lua.bindClassStaticFunction<Game>("rotate", std::bind(&Game::rotate, &g_game, _1));

    g_lua.bindClassStaticFunction<Game>("partyInvite", std::bind(&Game::partyInvite, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("partyJoin", std::bind(&Game::partyJoin, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("partyRevokeInvitation", std::bind(&Game::partyRevokeInvitation, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("partyPassLeadership", std::bind(&Game::partyPassLeadership, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("partyLeave", std::bind(&Game::partyLeave, &g_game));
    g_lua.bindClassStaticFunction<Game>("partyShareExperience", std::bind(&Game::partyShareExperience, &g_game, _1));

    g_lua.bindClassStaticFunction<Game>("addVip", std::bind(&Game::addVip, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("removeVip", std::bind(&Game::removeVip, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("talk", std::bind(&Game::talk, &g_game, _1));
    g_lua.bindClassStaticFunction<Game>("talkChannel", std::bind(&Game::talkChannel, &g_game, _1, _2, _3));
    g_lua.bindClassStaticFunction<Game>("talkPrivate", std::bind(&Game::talkPrivate, &g_game, _1, _2, _3));
    g_lua.bindClassStaticFunction<Game>("getLocalPlayer", std::bind(&Game::getLocalPlayer, &g_game));
    g_lua.bindClassStaticFunction<Game>("getProtocolVersion", std::bind(&Game::getProtocolVersion, &g_game));
    g_lua.bindClassStaticFunction<Game>("getProtocolGame", std::bind(&Game::getProtocolGame, &g_game));

    g_lua.registerClass<UIItem, UIWidget>();
    g_lua.bindClassStaticFunction<UIItem>("create", []{ return UIItemPtr(new UIItem); } );
    g_lua.bindClassMemberFunction<UIItem>("getItem", &UIItem::getItem);
    g_lua.bindClassMemberFunction<UIItem>("setItem", &UIItem::setItem);

    g_lua.registerClass<UICreature, UIWidget>();
    g_lua.bindClassStaticFunction<UICreature>("create", []{ return UICreaturePtr(new UICreature); } );
    g_lua.bindClassMemberFunction<UICreature>("getCreature", &UICreature::getCreature);
    g_lua.bindClassMemberFunction<UICreature>("setCreature", &UICreature::setCreature);

    g_lua.registerClass<UIMap, UIWidget>();
    g_lua.bindClassStaticFunction<UIMap>("create", []{ return UIMapPtr(new UIMap); } );
    g_lua.bindClassMemberFunction<UIMap>("getTile", &UIMap::getTile);

    g_lua.registerClass<UIGame, UIWidget>();
    g_lua.bindClassStaticFunction<UIGame>("create", []{ return UIGamePtr(new UIGame); } );

}
