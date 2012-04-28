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
#include <otclient/core/container.h>
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
#include <otclient/core/outfit.h>

void OTClient::registerLuaFunctions()
{
    Application::registerLuaFunctions();

    g_lua.registerStaticClass("g_thingsType");
    g_lua.bindClassStaticFunction("g_thingsType", "load", std::bind(&ThingsType::load, &g_thingsType, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_thingsType", "isLoaded", std::bind(&ThingsType::isLoaded, &g_thingsType));
    g_lua.bindClassStaticFunction("g_thingsType", "getSignature", std::bind(&ThingsType::getSignature, &g_thingsType));

    g_lua.registerStaticClass("g_sprites");
    g_lua.bindClassStaticFunction("g_sprites", "load", std::bind(&SpriteManager::load, &g_sprites, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_sprites", "isLoaded", std::bind(&SpriteManager::isLoaded, &g_sprites));
    g_lua.bindClassStaticFunction("g_sprites", "getSignature", std::bind(&SpriteManager::getSignature, &g_sprites));
    g_lua.bindClassStaticFunction("g_sprites", "preloadSprites", std::bind(&SpriteManager::preloadSprites, &g_sprites));
    g_lua.bindClassStaticFunction("g_sprites", "export", std::bind(&SpriteManager::exportSprites, &g_sprites));

    g_lua.registerStaticClass("g_map");
    g_lua.bindClassStaticFunction("g_map", "isLookPossible", std::bind(&Map::isLookPossible, &g_map, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_map", "isCovered", std::bind(&Map::isCovered, &g_map, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_map", "isCompletelyCovered", std::bind(&Map::isCompletelyCovered, &g_map, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_map", "addThing", std::bind(&Map::addThing, &g_map, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    g_lua.bindClassStaticFunction("g_map", "getThing", std::bind(&Map::getThing, &g_map, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_map", "removeThingByPos", std::bind(&Map::removeThingByPos, &g_map, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_map", "removeThing", std::bind(&Map::removeThing, &g_map, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_map", "cleanTile", std::bind(&Map::cleanTile, &g_map, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_map", "getTile", std::bind(&Map::getTile, &g_map, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_map", "setCentralPosition", std::bind(&Map::setCentralPosition, &g_map, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_map", "getCentralPosition", std::bind(&Map::getCentralPosition, &g_map));
    g_lua.bindClassStaticFunction("g_map", "getCreatureById", std::bind(&Map::getCreatureById, &g_map, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_map", "removeCreatureById", std::bind(&Map::removeCreatureById, &g_map, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_map", "getSpectators", std::bind(&Map::getSpectators, &g_map, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_map", "findPath", std::bind(&Map::findPath, &g_map, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    g_lua.registerStaticClass("g_game");
    g_lua.bindClassStaticFunction("g_game", "loginWorld", std::bind(&Game::loginWorld, &g_game, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
    g_lua.bindClassStaticFunction("g_game", "cancelLogin", std::bind(&Game::cancelLogin, &g_game));
    g_lua.bindClassStaticFunction("g_game", "forceLogout", std::bind(&Game::forceLogout, &g_game));
    g_lua.bindClassStaticFunction("g_game", "safeLogout", std::bind(&Game::safeLogout, &g_game));
    g_lua.bindClassStaticFunction("g_game", "walk", std::bind(&Game::walk, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "autoWalk", std::bind(&Game::autoWalk, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "forceWalk", std::bind(&Game::forceWalk, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "turn", std::bind(&Game::turn, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "stop", std::bind(&Game::stop, &g_game));
    g_lua.bindClassStaticFunction("g_game", "look", std::bind(&Game::look, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "move", std::bind(&Game::move, &g_game, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    g_lua.bindClassStaticFunction("g_game", "moveToParentContainer", std::bind(&Game::moveToParentContainer, &g_game, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_game", "rotate", std::bind(&Game::rotate, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "use", std::bind(&Game::use, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "useWith", std::bind(&Game::useWith, &g_game, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_game", "useInventoryItem", std::bind(&Game::useInventoryItem, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "useInventoryItemWith", std::bind(&Game::useInventoryItemWith, &g_game, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_game", "open", std::bind(&Game::open, &g_game, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_game", "openParent", std::bind(&Game::openParent, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "close", std::bind(&Game::close, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "refreshContainer", std::bind(&Game::refreshContainer, &g_game));
    g_lua.bindClassStaticFunction("g_game", "attack", std::bind(&Game::attack, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "cancelAttack", std::bind(&Game::cancelAttack, &g_game));
    g_lua.bindClassStaticFunction("g_game", "follow", std::bind(&Game::follow, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "cancelFollow", std::bind(&Game::cancelFollow, &g_game));
    g_lua.bindClassStaticFunction("g_game", "cancelAttackAndFollow", std::bind(&Game::cancelAttackAndFollow, &g_game));
    g_lua.bindClassStaticFunction("g_game", "talk", std::bind(&Game::talk, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "talkChannel", std::bind(&Game::talkChannel, &g_game, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    g_lua.bindClassStaticFunction("g_game", "talkPrivate", std::bind(&Game::talkPrivate, &g_game, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    g_lua.bindClassStaticFunction("g_game", "openPrivateChannel", std::bind(&Game::openPrivateChannel, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "requestChannels", std::bind(&Game::requestChannels, &g_game));
    g_lua.bindClassStaticFunction("g_game", "joinChannel", std::bind(&Game::joinChannel, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "leaveChannel", std::bind(&Game::leaveChannel, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "closeNpcChannel", std::bind(&Game::closeNpcChannel, &g_game));
    g_lua.bindClassStaticFunction("g_game", "openOwnChannel", std::bind(&Game::openOwnChannel, &g_game));
    g_lua.bindClassStaticFunction("g_game", "inviteToOwnChannel", std::bind(&Game::inviteToOwnChannel, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "excludeFromOwnChannel", std::bind(&Game::excludeFromOwnChannel, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "partyInvite", std::bind(&Game::partyInvite, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "partyJoin", std::bind(&Game::partyJoin, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "partyRevokeInvitation", std::bind(&Game::partyRevokeInvitation, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "partyPassLeadership", std::bind(&Game::partyPassLeadership, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "partyLeave", std::bind(&Game::partyLeave, &g_game));
    g_lua.bindClassStaticFunction("g_game", "partyShareExperience", std::bind(&Game::partyShareExperience, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "requestOutfit", std::bind(&Game::requestOutfit, &g_game));
    g_lua.bindClassStaticFunction("g_game", "changeOutfit", std::bind(&Game::changeOutfit, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "addVip", std::bind(&Game::addVip, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "removeVip", std::bind(&Game::removeVip, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "setChaseMode", std::bind(&Game::setChaseMode, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "setFightMode", std::bind(&Game::setFightMode, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "setSafeFight", std::bind(&Game::setSafeFight, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "getChaseMode", std::bind(&Game::getChaseMode, &g_game));
    g_lua.bindClassStaticFunction("g_game", "getFightMode", std::bind(&Game::getFightMode, &g_game));
    g_lua.bindClassStaticFunction("g_game", "isSafeFight", std::bind(&Game::isSafeFight, &g_game));
    g_lua.bindClassStaticFunction("g_game", "inspectNpcTrade", std::bind(&Game::inspectNpcTrade, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "buyItem", std::bind(&Game::buyItem, &g_game, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    g_lua.bindClassStaticFunction("g_game", "sellItem", std::bind(&Game::sellItem, &g_game, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    g_lua.bindClassStaticFunction("g_game", "closeNpcTrade", std::bind(&Game::closeNpcTrade, &g_game));
    g_lua.bindClassStaticFunction("g_game", "requestTrade", std::bind(&Game::requestTrade, &g_game, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_game", "inspectTrade", std::bind(&Game::inspectTrade, &g_game, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_game", "acceptTrade", std::bind(&Game::acceptTrade, &g_game));
    g_lua.bindClassStaticFunction("g_game", "rejectTrade", std::bind(&Game::rejectTrade, &g_game));
    g_lua.bindClassStaticFunction("g_game", "editText", std::bind(&Game::editText, &g_game, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_game", "editList", std::bind(&Game::editList, &g_game, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    g_lua.bindClassStaticFunction("g_game", "requestQuestLog", std::bind(&Game::requestQuestLog, &g_game));
    g_lua.bindClassStaticFunction("g_game", "requestQuestLine", std::bind(&Game::requestQuestLine, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "canPerformGameAction", std::bind(&Game::canPerformGameAction, &g_game));
    g_lua.bindClassStaticFunction("g_game", "checkBotProtection", std::bind(&Game::checkBotProtection, &g_game));
    g_lua.bindClassStaticFunction("g_game", "isOnline", std::bind(&Game::isOnline, &g_game));
    g_lua.bindClassStaticFunction("g_game", "isDead", std::bind(&Game::isDead, &g_game));
    g_lua.bindClassStaticFunction("g_game", "isAttacking", std::bind(&Game::isAttacking, &g_game));
    g_lua.bindClassStaticFunction("g_game", "isFollowing", std::bind(&Game::isFollowing, &g_game));
    g_lua.bindClassStaticFunction("g_game", "getContainer", std::bind(&Game::getContainer, &g_game, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_game", "getContainers", std::bind(&Game::getContainers, &g_game));
    g_lua.bindClassStaticFunction("g_game", "getVips", std::bind(&Game::getVips, &g_game));
    g_lua.bindClassStaticFunction("g_game", "getAttackingCreature", std::bind(&Game::getAttackingCreature, &g_game));
    g_lua.bindClassStaticFunction("g_game", "getFollowingCreature", std::bind(&Game::getFollowingCreature, &g_game));
    g_lua.bindClassStaticFunction("g_game", "getServerBeat", std::bind(&Game::getServerBeat, &g_game));
    g_lua.bindClassStaticFunction("g_game", "getLocalPlayer", std::bind(&Game::getLocalPlayer, &g_game));
    g_lua.bindClassStaticFunction("g_game", "getProtocolGame", std::bind(&Game::getProtocolGame, &g_game));
    g_lua.bindClassStaticFunction("g_game", "getProtocolVersion", std::bind(&Game::getProtocolVersion, &g_game));
    g_lua.bindClassStaticFunction("g_game", "getWorldName", std::bind(&Game::getWorldName, &g_game));

    g_lua.bindGlobalFunction("getOufitColor", Outfit::getColor);

    g_lua.registerClass<ProtocolLogin, Protocol>();
    g_lua.bindClassStaticFunction<ProtocolLogin>("create", &ProtocolLogin::create);
    g_lua.bindClassMemberFunction<ProtocolLogin>("login", &ProtocolLogin::login);
    g_lua.bindClassMemberFunction<ProtocolLogin>("cancelLogin", &ProtocolLogin::cancelLogin);

    g_lua.registerClass<ProtocolGame, Protocol>();

    g_lua.registerClass<Container>();
    g_lua.bindClassStaticFunction<Container>("create", []{ return ContainerPtr(new Container); });
    g_lua.bindClassMemberFunction<Container>("open", &Container::open);
    g_lua.bindClassMemberFunction<Container>("close", &Container::close);
    g_lua.bindClassMemberFunction<Container>("addItem", &Container::addItem);
    g_lua.bindClassMemberFunction<Container>("addItems", &Container::addItems);
    g_lua.bindClassMemberFunction<Container>("updateItem", &Container::updateItem);
    g_lua.bindClassMemberFunction<Container>("removeItem", &Container::removeItem);
    g_lua.bindClassMemberFunction<Container>("getItem", &Container::getItem);
    g_lua.bindClassMemberFunction<Container>("getSlotPosition", &Container::getSlotPosition);
    g_lua.bindClassMemberFunction<Container>("setId", &Container::setId);
    g_lua.bindClassMemberFunction<Container>("setCapacity", &Container::setCapacity);
    g_lua.bindClassMemberFunction<Container>("setName", &Container::setName);
    g_lua.bindClassMemberFunction<Container>("setItemId", &Container::setItemId);
    g_lua.bindClassMemberFunction<Container>("setHasParent", &Container::setHasParent);
    g_lua.bindClassMemberFunction<Container>("getName", &Container::getName);
    g_lua.bindClassMemberFunction<Container>("getId", &Container::getId);
    g_lua.bindClassMemberFunction<Container>("getCapacity", &Container::getCapacity);
    g_lua.bindClassMemberFunction<Container>("getItemsCount", &Container::getItemsCount);
    g_lua.bindClassMemberFunction<Container>("getItemId", &Container::getItemId);
    g_lua.bindClassMemberFunction<Container>("hasParent", &Container::hasParent);

    g_lua.registerClass<Thing>();
    g_lua.bindClassMemberFunction<Thing>("setId", &Thing::setId);
    g_lua.bindClassMemberFunction<Thing>("setPosition", &Thing::setPosition);
    g_lua.bindClassMemberFunction<Thing>("getId", &Thing::getId);
    g_lua.bindClassMemberFunction<Thing>("getPosition", &Thing::getPosition);
    g_lua.bindClassMemberFunction<Thing>("getStackPriority", &Thing::getStackPriority);
    g_lua.bindClassMemberFunction<Thing>("getAnimationPhases", &Thing::getAnimationPhases);
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
    g_lua.bindClassMemberFunction<Thing>("isIgnoreLook", &Thing::isIgnoreLook);
    g_lua.bindClassMemberFunction<Thing>("isStackable", &Thing::isStackable);
    g_lua.bindClassMemberFunction<Thing>("isHookSouth", &Thing::isHookSouth);
    g_lua.bindClassMemberFunction<Thing>("isTranslucent", &Thing::isTranslucent);
    g_lua.bindClassMemberFunction<Thing>("isFullGround", &Thing::isFullGround);
    g_lua.bindClassMemberFunction<Thing>("getParentContainer", &Thing::getParentContainer);

    g_lua.registerClass<Creature, Thing>();
    g_lua.bindClassMemberFunction<Creature>("getId", &Creature::getId);
    g_lua.bindClassMemberFunction<Creature>("getName", &Creature::getName);
    g_lua.bindClassMemberFunction<Creature>("getHealthPercent", &Creature::getHealthPercent);
    g_lua.bindClassMemberFunction<Creature>("getSkull", &Creature::getSkull);
    g_lua.bindClassMemberFunction<Creature>("getShield", &Creature::getShield);
    g_lua.bindClassMemberFunction<Creature>("getEmblem", &Creature::getEmblem);
    g_lua.bindClassMemberFunction<Creature>("setOutfit", &Creature::setOutfit);
    g_lua.bindClassMemberFunction<Creature>("getOutfit", &Creature::getOutfit);
    g_lua.bindClassMemberFunction<Creature>("setSkullTexture", &Creature::setSkullTexture);
    g_lua.bindClassMemberFunction<Creature>("setShieldTexture", &Creature::setShieldTexture);
    g_lua.bindClassMemberFunction<Creature>("setEmblemTexture", &Creature::setEmblemTexture);
    g_lua.bindClassMemberFunction<Creature>("showStaticSquare", &Creature::showStaticSquare);
    g_lua.bindClassMemberFunction<Creature>("hideStaticSquare", &Creature::hideStaticSquare);
    g_lua.bindClassMemberFunction<Creature>("isWalking", &Creature::isWalking);
    g_lua.bindClassMemberFunction<Creature>("asMonster", &Creature::asMonster);
    g_lua.bindClassMemberFunction<Creature>("asNpc", &Creature::asNpc);

    g_lua.registerClass<Item, Thing>();
    g_lua.bindClassStaticFunction<Item>("create", &Item::create);
    g_lua.bindClassMemberFunction<Item>("setCount", &Item::setCount);
    g_lua.bindClassMemberFunction<Item>("getCount", &Item::getCount);
    g_lua.bindClassMemberFunction<Item>("getId", &Item::getId);
    g_lua.bindClassMemberFunction<Item>("isStackable", &Item::isStackable);


    g_lua.registerClass<Effect, Thing>();
    g_lua.registerClass<Missile, Thing>();
    g_lua.registerClass<StaticText, Thing>();
    g_lua.registerClass<AnimatedText, Thing>();

    g_lua.registerClass<Player, Creature>();
    g_lua.registerClass<Npc, Creature>();
    g_lua.registerClass<Monster, Creature>();

    g_lua.registerClass<LocalPlayer, Player>();
    g_lua.bindClassMemberFunction<LocalPlayer>("unlockWalk", &LocalPlayer::unlockWalk);
    g_lua.bindClassMemberFunction<LocalPlayer>("lockWalk", &LocalPlayer::lockWalk);
    g_lua.bindClassMemberFunction<LocalPlayer>("canWalk", &LocalPlayer::canWalk);
    g_lua.bindClassMemberFunction<LocalPlayer>("setStates", &LocalPlayer::setStates);
    g_lua.bindClassMemberFunction<LocalPlayer>("setSkill", &LocalPlayer::setSkill);
    g_lua.bindClassMemberFunction<LocalPlayer>("setHealth", &LocalPlayer::setHealth);
    g_lua.bindClassMemberFunction<LocalPlayer>("setFreeCapacity", &LocalPlayer::setFreeCapacity);
    g_lua.bindClassMemberFunction<LocalPlayer>("setExperience", &LocalPlayer::setExperience);
    g_lua.bindClassMemberFunction<LocalPlayer>("setLevel", &LocalPlayer::setLevel);
    g_lua.bindClassMemberFunction<LocalPlayer>("setMana", &LocalPlayer::setMana);
    g_lua.bindClassMemberFunction<LocalPlayer>("setMagicLevel", &LocalPlayer::setMagicLevel);
    g_lua.bindClassMemberFunction<LocalPlayer>("setSoul", &LocalPlayer::setSoul);
    g_lua.bindClassMemberFunction<LocalPlayer>("setStamina", &LocalPlayer::setStamina);
    g_lua.bindClassMemberFunction<LocalPlayer>("setKnown", &LocalPlayer::setKnown);
    g_lua.bindClassMemberFunction<LocalPlayer>("setInventoryItem", &LocalPlayer::setInventoryItem);
    g_lua.bindClassMemberFunction<LocalPlayer>("getStates", &LocalPlayer::getStates);
    g_lua.bindClassMemberFunction<LocalPlayer>("getSkillLevel", &LocalPlayer::getSkillLevel);
    g_lua.bindClassMemberFunction<LocalPlayer>("getSkillLevelPercent", &LocalPlayer::getSkillLevelPercent);
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
    g_lua.bindClassMemberFunction<LocalPlayer>("getInventoryItem", &LocalPlayer::getInventoryItem);
    g_lua.bindClassMemberFunction<LocalPlayer>("isKnown", &LocalPlayer::isKnown);
    g_lua.bindClassMemberFunction<LocalPlayer>("isPreWalking", &LocalPlayer::isPreWalking);
    g_lua.bindClassMemberFunction<LocalPlayer>("asLocalPlayer", &LocalPlayer::asLocalPlayer);

    g_lua.registerClass<Tile>();
    g_lua.bindClassMemberFunction<Tile>("clean", &Tile::clean);
    g_lua.bindClassMemberFunction<Tile>("addThing", &Tile::addThing);
    g_lua.bindClassMemberFunction<Tile>("getThing", &Tile::getThing);
    g_lua.bindClassMemberFunction<Tile>("getThingStackpos", &Tile::getThingStackpos);
    g_lua.bindClassMemberFunction<Tile>("getTopThing", &Tile::getTopThing);
    g_lua.bindClassMemberFunction<Tile>("removeThing", &Tile::removeThing);
    g_lua.bindClassMemberFunction<Tile>("getTopLookThing", &Tile::getTopLookThing);
    g_lua.bindClassMemberFunction<Tile>("getTopUseThing", &Tile::getTopUseThing);
    g_lua.bindClassMemberFunction<Tile>("getTopCreature", &Tile::getTopCreature);
    g_lua.bindClassMemberFunction<Tile>("getTopMoveThing", &Tile::getTopMoveThing);
    g_lua.bindClassMemberFunction<Tile>("getTopMultiUseThing", &Tile::getTopMultiUseThing);
    g_lua.bindClassMemberFunction<Tile>("getPosition", &Tile::getPosition);
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

    g_lua.registerClass<UIItem, UIWidget>();
    g_lua.bindClassStaticFunction<UIItem>("create", []{ return UIItemPtr(new UIItem); });
    g_lua.bindClassMemberFunction<UIItem>("setItemId", &UIItem::setItemId);
    g_lua.bindClassMemberFunction<UIItem>("setItemCount", &UIItem::setItemCount);
    g_lua.bindClassMemberFunction<UIItem>("setItemSubType", &UIItem::setItemSubType);
    g_lua.bindClassMemberFunction<UIItem>("setItem", &UIItem::setItem);
    g_lua.bindClassMemberFunction<UIItem>("setVirtual", &UIItem::setVirtual);
    g_lua.bindClassMemberFunction<UIItem>("clearItem", &UIItem::clearItem);
    g_lua.bindClassMemberFunction<UIItem>("getItemId", &UIItem::getItemId);
    g_lua.bindClassMemberFunction<UIItem>("getItemCount", &UIItem::getItemCount);
    g_lua.bindClassMemberFunction<UIItem>("getItemSubType", &UIItem::getItemSubType);
    g_lua.bindClassMemberFunction<UIItem>("getItem", &UIItem::getItem);
    g_lua.bindClassMemberFunction<UIItem>("isVirtual", &UIItem::isVirtual);

    g_lua.registerClass<UICreature, UIWidget>();
    g_lua.bindClassStaticFunction<UICreature>("create", []{ return UICreaturePtr(new UICreature); } );
    g_lua.bindClassMemberFunction<UICreature>("setCreature", &UICreature::setCreature);
    g_lua.bindClassMemberFunction<UICreature>("setFixedCreatureSize", &UICreature::setFixedCreatureSize);
    g_lua.bindClassMemberFunction<UICreature>("getCreature", &UICreature::getCreature);
    g_lua.bindClassMemberFunction<UICreature>("isFixedCreatureSize", &UICreature::isFixedCreatureSize);

    g_lua.registerClass<UIMap, UIWidget>();
    g_lua.bindClassStaticFunction<UIMap>("create", []{ return UIMapPtr(new UIMap); });
    g_lua.bindClassMemberFunction<UIMap>("drawSelf", &UIMap::drawSelf);
    g_lua.bindClassMemberFunction<UIMap>("setZoom", &UIMap::setZoom);
    g_lua.bindClassMemberFunction<UIMap>("zoomIn", &UIMap::zoomIn);
    g_lua.bindClassMemberFunction<UIMap>("zoomOut", &UIMap::zoomOut);
    g_lua.bindClassMemberFunction<UIMap>("followCreature", &UIMap::followCreature);
    g_lua.bindClassMemberFunction<UIMap>("setCameraPosition", &UIMap::setCameraPosition);
    g_lua.bindClassMemberFunction<UIMap>("setMaxZoomIn", &UIMap::setMaxZoomIn);
    g_lua.bindClassMemberFunction<UIMap>("setMaxZoomOut", &UIMap::setMaxZoomOut);
    g_lua.bindClassMemberFunction<UIMap>("setMultifloor", &UIMap::setMultifloor);
    g_lua.bindClassMemberFunction<UIMap>("setVisibleDimension", &UIMap::setVisibleDimension);
    g_lua.bindClassMemberFunction<UIMap>("setViewMode", &UIMap::setViewMode);
    g_lua.bindClassMemberFunction<UIMap>("setAutoViewMode", &UIMap::setAutoViewMode);
    g_lua.bindClassMemberFunction<UIMap>("setDrawFlags", &UIMap::setDrawFlags);
    g_lua.bindClassMemberFunction<UIMap>("setDrawTexts", &UIMap::setDrawTexts);
    g_lua.bindClassMemberFunction<UIMap>("setDrawMinimapColors", &UIMap::setDrawMinimapColors);
    g_lua.bindClassMemberFunction<UIMap>("setAnimated", &UIMap::setAnimated);
    g_lua.bindClassMemberFunction<UIMap>("setKeepAspectRatio", &UIMap::setKeepAspectRatio);
    g_lua.bindClassMemberFunction<UIMap>("isMultifloor", &UIMap::isMultifloor);
    g_lua.bindClassMemberFunction<UIMap>("isAutoViewModeEnabled", &UIMap::isAutoViewModeEnabled);
    g_lua.bindClassMemberFunction<UIMap>("isDrawingTexts", &UIMap::isDrawingTexts);
    g_lua.bindClassMemberFunction<UIMap>("isDrawingMinimapColors", &UIMap::isDrawingMinimapColors);
    g_lua.bindClassMemberFunction<UIMap>("isAnimating", &UIMap::isAnimating);
    g_lua.bindClassMemberFunction<UIMap>("isKeepAspectRatioEnabled", &UIMap::isKeepAspectRatioEnabled);
    g_lua.bindClassMemberFunction<UIMap>("getVisibleDimension", &UIMap::getVisibleDimension);
    g_lua.bindClassMemberFunction<UIMap>("getViewMode", &UIMap::getViewMode);
    g_lua.bindClassMemberFunction<UIMap>("getFollowingCreature", &UIMap::getFollowingCreature);
    g_lua.bindClassMemberFunction<UIMap>("getDrawFlags", &UIMap::getDrawFlags);
    g_lua.bindClassMemberFunction<UIMap>("getCameraPosition", &UIMap::getCameraPosition);
    g_lua.bindClassMemberFunction<UIMap>("getTile", &UIMap::getTile);
    g_lua.bindClassMemberFunction<UIMap>("getMaxZoomIn", &UIMap::getMaxZoomIn);
    g_lua.bindClassMemberFunction<UIMap>("getMaxZoomOut", &UIMap::getMaxZoomOut);
    g_lua.bindClassMemberFunction<UIMap>("getZoom", &UIMap::getZoom);
}
