/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#include "client.h"
#include "luavaluecasts.h"
#include "game.h"
#include "tile.h"
#include "houses.h"
#include "towns.h"
#include "container.h"
#include "item.h"
#include "effect.h"
#include "missile.h"
#include "statictext.h"
#include "animatedtext.h"
#include "creature.h"
#include "player.h"
#include "localplayer.h"
#include "map.h"
#include "minimap.h"
#include "thingtypemanager.h"
#include "spritemanager.h"
#include "shadermanager.h"
#include "protocolgame.h"
#include "uiitem.h"
#include "uicreature.h"
#include "uimap.h"
#include "uiminimap.h"
#include "uimapanchorlayout.h"
#include "uiprogressrect.h"
#include "uisprite.h"
#include "outfit.h"

#include <framework/luaengine/luainterface.h>

void Client::registerLuaFunctions()
{
    g_lua.registerSingletonClass("g_things");
    g_lua.bindSingletonFunction("g_things", "loadDat", &ThingTypeManager::loadDat, &g_things);
    g_lua.bindSingletonFunction("g_things", "saveDat", &ThingTypeManager::saveDat, &g_things);
    g_lua.bindSingletonFunction("g_things", "loadOtb", &ThingTypeManager::loadOtb, &g_things);
    g_lua.bindSingletonFunction("g_things", "loadXml", &ThingTypeManager::loadXml, &g_things);
    g_lua.bindSingletonFunction("g_things", "loadOtml", &ThingTypeManager::loadOtml, &g_things);
    g_lua.bindSingletonFunction("g_things", "isDatLoaded", &ThingTypeManager::isDatLoaded, &g_things);
    g_lua.bindSingletonFunction("g_things", "isOtbLoaded", &ThingTypeManager::isOtbLoaded, &g_things);
    g_lua.bindSingletonFunction("g_things", "getDatSignature", &ThingTypeManager::getDatSignature, &g_things);
    g_lua.bindSingletonFunction("g_things", "getContentRevision", &ThingTypeManager::getContentRevision, &g_things);
    g_lua.bindSingletonFunction("g_things", "getThingType", &ThingTypeManager::getThingType, &g_things);
    g_lua.bindSingletonFunction("g_things", "getItemType", &ThingTypeManager::getItemType, &g_things);
    g_lua.bindSingletonFunction("g_things", "getThingTypes", &ThingTypeManager::getThingTypes, &g_things);
    g_lua.bindSingletonFunction("g_things", "findItemTypeByClientId", &ThingTypeManager::findItemTypeByClientId, &g_things);
    g_lua.bindSingletonFunction("g_things", "findItemTypeByName", &ThingTypeManager::findItemTypeByName, &g_things);
    g_lua.bindSingletonFunction("g_things", "findItemTypesByName", &ThingTypeManager::findItemTypesByName, &g_things);
    g_lua.bindSingletonFunction("g_things", "findItemTypesByString", &ThingTypeManager::findItemTypesByString, &g_things);
    g_lua.bindSingletonFunction("g_things", "findItemTypeByCategory", &ThingTypeManager::findItemTypeByCategory, &g_things);
    g_lua.bindSingletonFunction("g_things", "findThingTypeByAttr", &ThingTypeManager::findThingTypeByAttr, &g_things);

    g_lua.registerSingletonClass("g_houses");
    g_lua.bindSingletonFunction("g_houses", "clear",          &HouseManager::clear,          &g_houses);
    g_lua.bindSingletonFunction("g_houses", "load",           &HouseManager::load,           &g_houses);
    g_lua.bindSingletonFunction("g_houses", "save",           &HouseManager::save,           &g_houses);
    g_lua.bindSingletonFunction("g_houses", "getHouse",       &HouseManager::getHouse,       &g_houses);
    g_lua.bindSingletonFunction("g_houses", "getHouseByName", &HouseManager::getHouseByName, &g_houses);
    g_lua.bindSingletonFunction("g_houses", "addHouse",       &HouseManager::addHouse,       &g_houses);
    g_lua.bindSingletonFunction("g_houses", "removeHouse",    &HouseManager::removeHouse,    &g_houses);
    g_lua.bindSingletonFunction("g_houses", "getHouseList",   &HouseManager::getHouseList,   &g_houses);
    g_lua.bindSingletonFunction("g_houses", "filterHouses",   &HouseManager::filterHouses,   &g_houses);
    g_lua.bindSingletonFunction("g_houses", "sort",           &HouseManager::sort,           &g_houses);

    g_lua.registerSingletonClass("g_towns");
    g_lua.bindSingletonFunction("g_towns", "getTown",      &TownManager::getTown,      &g_towns);
    g_lua.bindSingletonFunction("g_towns", "getTownByName",&TownManager::getTownByName,&g_towns);
    g_lua.bindSingletonFunction("g_towns", "addTown",      &TownManager::addTown,      &g_towns);
    g_lua.bindSingletonFunction("g_towns", "removeTown",   &TownManager::removeTown,   &g_towns);
    g_lua.bindSingletonFunction("g_towns", "getTowns",     &TownManager::getTowns,     &g_towns);
    g_lua.bindSingletonFunction("g_towns", "sort",         &TownManager::sort,         &g_towns);

    g_lua.registerSingletonClass("g_sprites");
    g_lua.bindSingletonFunction("g_sprites", "loadSpr", &SpriteManager::loadSpr, &g_sprites);
    g_lua.bindSingletonFunction("g_sprites", "saveSpr", &SpriteManager::saveSpr, &g_sprites);
    g_lua.bindSingletonFunction("g_sprites", "unload", &SpriteManager::unload, &g_sprites);
    g_lua.bindSingletonFunction("g_sprites", "isLoaded", &SpriteManager::isLoaded, &g_sprites);
    g_lua.bindSingletonFunction("g_sprites", "getSprSignature", &SpriteManager::getSignature, &g_sprites);
    g_lua.bindSingletonFunction("g_sprites", "getSpritesCount", &SpriteManager::getSpritesCount, &g_sprites);

    g_lua.registerSingletonClass("g_map");
    g_lua.bindSingletonFunction("g_map", "isLookPossible", &Map::isLookPossible, &g_map);
    g_lua.bindSingletonFunction("g_map", "isCovered", &Map::isCovered, &g_map);
    g_lua.bindSingletonFunction("g_map", "isCompletelyCovered", &Map::isCompletelyCovered, &g_map);
    g_lua.bindSingletonFunction("g_map", "addThing", &Map::addThing, &g_map);
    g_lua.bindSingletonFunction("g_map", "getThing", &Map::getThing, &g_map);
    g_lua.bindSingletonFunction("g_map", "removeThingByPos", &Map::removeThingByPos, &g_map);
    g_lua.bindSingletonFunction("g_map", "removeThing", &Map::removeThing, &g_map);
    g_lua.bindSingletonFunction("g_map", "colorizeThing", &Map::colorizeThing, &g_map);
    g_lua.bindSingletonFunction("g_map", "removeThingColor", &Map::removeThingColor, &g_map);
    g_lua.bindSingletonFunction("g_map", "clean", &Map::clean, &g_map);
    g_lua.bindSingletonFunction("g_map", "cleanTile", &Map::cleanTile, &g_map);
    g_lua.bindSingletonFunction("g_map", "cleanTexts", &Map::cleanTexts, &g_map);
    g_lua.bindSingletonFunction("g_map", "getTile", &Map::getTile, &g_map);
    g_lua.bindSingletonFunction("g_map", "getTiles", &Map::getTiles, &g_map);
    g_lua.bindSingletonFunction("g_map", "setCentralPosition", &Map::setCentralPosition, &g_map);
    g_lua.bindSingletonFunction("g_map", "getCentralPosition", &Map::getCentralPosition, &g_map);
    g_lua.bindSingletonFunction("g_map", "getCreatureById", &Map::getCreatureById, &g_map);
    g_lua.bindSingletonFunction("g_map", "removeCreatureById", &Map::removeCreatureById, &g_map);
    g_lua.bindSingletonFunction("g_map", "getSpectators", &Map::getSpectators, &g_map);
    g_lua.bindSingletonFunction("g_map", "findPath", &Map::findPath, &g_map);
    g_lua.bindSingletonFunction("g_map", "loadOtbm", &Map::loadOtbm, &g_map);
    g_lua.bindSingletonFunction("g_map", "saveOtbm", &Map::saveOtbm, &g_map);
    g_lua.bindSingletonFunction("g_map", "loadOtcm", &Map::loadOtcm, &g_map);
    g_lua.bindSingletonFunction("g_map", "saveOtcm", &Map::saveOtcm, &g_map);
    g_lua.bindSingletonFunction("g_map", "getHouseFile", &Map::getHouseFile, &g_map);
    g_lua.bindSingletonFunction("g_map", "setHouseFile", &Map::setHouseFile, &g_map);
    g_lua.bindSingletonFunction("g_map", "getSpawnFile", &Map::getSpawnFile, &g_map);
    g_lua.bindSingletonFunction("g_map", "setSpawnFile", &Map::setSpawnFile, &g_map);
    g_lua.bindSingletonFunction("g_map", "createTile", &Map::createTile, &g_map);
    g_lua.bindSingletonFunction("g_map", "setWidth", &Map::setWidth, &g_map);
    g_lua.bindSingletonFunction("g_map", "setHeight", &Map::setHeight, &g_map);
    g_lua.bindSingletonFunction("g_map", "getSize", &Map::getSize, &g_map);
    g_lua.bindSingletonFunction("g_map", "setDescription", &Map::setDescription, &g_map);
    g_lua.bindSingletonFunction("g_map", "getDescriptions", &Map::getDescriptions, &g_map);
    g_lua.bindSingletonFunction("g_map", "clearDescriptions", &Map::clearDescriptions, &g_map);
    g_lua.bindSingletonFunction("g_map", "setShowZone", &Map::setShowZone, &g_map);
    g_lua.bindSingletonFunction("g_map", "setShowZones", &Map::setShowZones, &g_map);
    g_lua.bindSingletonFunction("g_map", "setZoneColor", &Map::setZoneColor, &g_map);
    g_lua.bindSingletonFunction("g_map", "setZoneOpacity", &Map::setZoneOpacity, &g_map);
    g_lua.bindSingletonFunction("g_map", "getZoneOpacity", &Map::getZoneOpacity, &g_map);
    g_lua.bindSingletonFunction("g_map", "getZoneColor", &Map::getZoneColor, &g_map);
    g_lua.bindSingletonFunction("g_map", "showZones", &Map::showZones, &g_map);
    g_lua.bindSingletonFunction("g_map", "showZone", &Map::showZone, &g_map);
    g_lua.bindSingletonFunction("g_map", "setForceShowAnimations", &Map::setForceShowAnimations, &g_map);
    g_lua.bindSingletonFunction("g_map", "isForcingAnimations", &Map::isForcingAnimations, &g_map);
    g_lua.bindSingletonFunction("g_map", "isShowingAnimations", &Map::isShowingAnimations, &g_map);
    g_lua.bindSingletonFunction("g_map", "setShowAnimations", &Map::setShowAnimations, &g_map);
    g_lua.bindSingletonFunction("g_map", "beginGhostMode", &Map::beginGhostMode, &g_map);
    g_lua.bindSingletonFunction("g_map", "endGhostMode", &Map::endGhostMode, &g_map);
    g_lua.bindSingletonFunction("g_map", "findItemsById", &Map::findItemsById, &g_map);

    g_lua.registerSingletonClass("g_minimap");
    g_lua.bindSingletonFunction("g_minimap", "clean", &Minimap::clean, &g_minimap);
    g_lua.bindSingletonFunction("g_minimap", "loadImage", &Minimap::loadImage, &g_minimap);
    g_lua.bindSingletonFunction("g_minimap", "saveImage", &Minimap::saveImage, &g_minimap);
    g_lua.bindSingletonFunction("g_minimap", "loadOtmm", &Minimap::loadOtmm, &g_minimap);
    g_lua.bindSingletonFunction("g_minimap", "saveOtmm", &Minimap::saveOtmm, &g_minimap);

    g_lua.registerSingletonClass("g_creatures");
    g_lua.bindSingletonFunction("g_creatures", "getCreatures", &CreatureManager::getCreatures, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "getCreatureByName", &CreatureManager::getCreatureByName, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "getCreatureByLook", &CreatureManager::getCreatureByLook, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "getSpawn", &CreatureManager::getSpawn, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "getSpawnForPlacePos", &CreatureManager::getSpawnForPlacePos, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "addSpawn", &CreatureManager::addSpawn, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "loadMonsters", &CreatureManager::loadMonsters, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "loadNpcs", &CreatureManager::loadNpcs, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "loadSingleCreature", &CreatureManager::loadSingleCreature, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "loadSpawns", &CreatureManager::loadSpawns, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "saveSpawns", &CreatureManager::saveSpawns, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "isLoaded", &CreatureManager::isLoaded, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "isSpawnLoaded", &CreatureManager::isSpawnLoaded, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "clear", &CreatureManager::clear, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "clearSpawns", &CreatureManager::clearSpawns, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "getSpawns", &CreatureManager::getSpawns, &g_creatures);
    g_lua.bindSingletonFunction("g_creatures", "deleteSpawn", &CreatureManager::deleteSpawn, &g_creatures);

    g_lua.registerSingletonClass("g_game");
    g_lua.bindSingletonFunction("g_game", "loginWorld", &Game::loginWorld, &g_game);
    g_lua.bindSingletonFunction("g_game", "cancelLogin", &Game::cancelLogin, &g_game);
    g_lua.bindSingletonFunction("g_game", "forceLogout", &Game::forceLogout, &g_game);
    g_lua.bindSingletonFunction("g_game", "safeLogout", &Game::safeLogout, &g_game);
    g_lua.bindSingletonFunction("g_game", "walk", &Game::walk, &g_game);
    g_lua.bindSingletonFunction("g_game", "dashWalk", &Game::dashWalk, &g_game);
    g_lua.bindSingletonFunction("g_game", "autoWalk", &Game::autoWalk, &g_game);
    g_lua.bindSingletonFunction("g_game", "forceWalk", &Game::forceWalk, &g_game);
    g_lua.bindSingletonFunction("g_game", "turn", &Game::turn, &g_game);
    g_lua.bindSingletonFunction("g_game", "stop", &Game::stop, &g_game);
    g_lua.bindSingletonFunction("g_game", "look", &Game::look, &g_game);
    g_lua.bindSingletonFunction("g_game", "move", &Game::move, &g_game);
    g_lua.bindSingletonFunction("g_game", "moveToParentContainer", &Game::moveToParentContainer, &g_game);
    g_lua.bindSingletonFunction("g_game", "rotate", &Game::rotate, &g_game);
    g_lua.bindSingletonFunction("g_game", "use", &Game::use, &g_game);
    g_lua.bindSingletonFunction("g_game", "useWith", &Game::useWith, &g_game);
    g_lua.bindSingletonFunction("g_game", "useInventoryItem", &Game::useInventoryItem, &g_game);
    g_lua.bindSingletonFunction("g_game", "useInventoryItemWith", &Game::useInventoryItemWith, &g_game);
    g_lua.bindSingletonFunction("g_game", "findItemInContainers", &Game::findItemInContainers, &g_game);
    g_lua.bindSingletonFunction("g_game", "open", &Game::open, &g_game);
    g_lua.bindSingletonFunction("g_game", "openParent", &Game::openParent, &g_game);
    g_lua.bindSingletonFunction("g_game", "close", &Game::close, &g_game);
    g_lua.bindSingletonFunction("g_game", "refreshContainer", &Game::refreshContainer, &g_game);
    g_lua.bindSingletonFunction("g_game", "attack", &Game::attack, &g_game);
    g_lua.bindSingletonFunction("g_game", "cancelAttack", &Game::cancelAttack, &g_game);
    g_lua.bindSingletonFunction("g_game", "follow", &Game::follow, &g_game);
    g_lua.bindSingletonFunction("g_game", "cancelFollow", &Game::cancelFollow, &g_game);
    g_lua.bindSingletonFunction("g_game", "cancelAttackAndFollow", &Game::cancelAttackAndFollow, &g_game);
    g_lua.bindSingletonFunction("g_game", "talk", &Game::talk, &g_game);
    g_lua.bindSingletonFunction("g_game", "talkChannel", &Game::talkChannel, &g_game);
    g_lua.bindSingletonFunction("g_game", "talkPrivate", &Game::talkPrivate, &g_game);
    g_lua.bindSingletonFunction("g_game", "openPrivateChannel", &Game::openPrivateChannel, &g_game);
    g_lua.bindSingletonFunction("g_game", "requestChannels", &Game::requestChannels, &g_game);
    g_lua.bindSingletonFunction("g_game", "joinChannel", &Game::joinChannel, &g_game);
    g_lua.bindSingletonFunction("g_game", "leaveChannel", &Game::leaveChannel, &g_game);
    g_lua.bindSingletonFunction("g_game", "closeNpcChannel", &Game::closeNpcChannel, &g_game);
    g_lua.bindSingletonFunction("g_game", "openOwnChannel", &Game::openOwnChannel, &g_game);
    g_lua.bindSingletonFunction("g_game", "inviteToOwnChannel", &Game::inviteToOwnChannel, &g_game);
    g_lua.bindSingletonFunction("g_game", "excludeFromOwnChannel", &Game::excludeFromOwnChannel, &g_game);
    g_lua.bindSingletonFunction("g_game", "partyInvite", &Game::partyInvite, &g_game);
    g_lua.bindSingletonFunction("g_game", "partyJoin", &Game::partyJoin, &g_game);
    g_lua.bindSingletonFunction("g_game", "partyRevokeInvitation", &Game::partyRevokeInvitation, &g_game);
    g_lua.bindSingletonFunction("g_game", "partyPassLeadership", &Game::partyPassLeadership, &g_game);
    g_lua.bindSingletonFunction("g_game", "partyLeave", &Game::partyLeave, &g_game);
    g_lua.bindSingletonFunction("g_game", "partyShareExperience", &Game::partyShareExperience, &g_game);
    g_lua.bindSingletonFunction("g_game", "requestOutfit", &Game::requestOutfit, &g_game);
    g_lua.bindSingletonFunction("g_game", "changeOutfit", &Game::changeOutfit, &g_game);
    g_lua.bindSingletonFunction("g_game", "addVip", &Game::addVip, &g_game);
    g_lua.bindSingletonFunction("g_game", "removeVip", &Game::removeVip, &g_game);
    g_lua.bindSingletonFunction("g_game", "editVip", &Game::editVip, &g_game);
    g_lua.bindSingletonFunction("g_game", "setChaseMode", &Game::setChaseMode, &g_game);
    g_lua.bindSingletonFunction("g_game", "setFightMode", &Game::setFightMode, &g_game);
    g_lua.bindSingletonFunction("g_game", "setPVPMode", &Game::setPVPMode, &g_game);
    g_lua.bindSingletonFunction("g_game", "setSafeFight", &Game::setSafeFight, &g_game);
    g_lua.bindSingletonFunction("g_game", "getChaseMode", &Game::getChaseMode, &g_game);
    g_lua.bindSingletonFunction("g_game", "getFightMode", &Game::getFightMode, &g_game);
    g_lua.bindSingletonFunction("g_game", "getPVPMode", &Game::getPVPMode, &g_game);
    g_lua.bindSingletonFunction("g_game", "getUnjustifiedPoints", &Game::getUnjustifiedPoints, &g_game);
    g_lua.bindSingletonFunction("g_game", "getOpenPvpSituations", &Game::getOpenPvpSituations, &g_game);
    g_lua.bindSingletonFunction("g_game", "isSafeFight", &Game::isSafeFight, &g_game);
    g_lua.bindSingletonFunction("g_game", "inspectNpcTrade", &Game::inspectNpcTrade, &g_game);
    g_lua.bindSingletonFunction("g_game", "buyItem", &Game::buyItem, &g_game);
    g_lua.bindSingletonFunction("g_game", "sellItem", &Game::sellItem, &g_game);
    g_lua.bindSingletonFunction("g_game", "closeNpcTrade", &Game::closeNpcTrade, &g_game);
    g_lua.bindSingletonFunction("g_game", "requestTrade", &Game::requestTrade, &g_game);
    g_lua.bindSingletonFunction("g_game", "inspectTrade", &Game::inspectTrade, &g_game);
    g_lua.bindSingletonFunction("g_game", "acceptTrade", &Game::acceptTrade, &g_game);
    g_lua.bindSingletonFunction("g_game", "rejectTrade", &Game::rejectTrade, &g_game);
    g_lua.bindSingletonFunction("g_game", "openRuleViolation", &Game::openRuleViolation, &g_game);
    g_lua.bindSingletonFunction("g_game", "closeRuleViolation", &Game::closeRuleViolation, &g_game);
    g_lua.bindSingletonFunction("g_game", "cancelRuleViolation", &Game::cancelRuleViolation, &g_game);
    g_lua.bindSingletonFunction("g_game", "reportBug", &Game::reportBug, &g_game);
    g_lua.bindSingletonFunction("g_game", "reportRuleViolation", &Game::reportRuleViolation, &g_game);
    g_lua.bindSingletonFunction("g_game", "debugReport", &Game::debugReport, &g_game);
    g_lua.bindSingletonFunction("g_game", "editText", &Game::editText, &g_game);
    g_lua.bindSingletonFunction("g_game", "editList", &Game::editList, &g_game);
    g_lua.bindSingletonFunction("g_game", "requestQuestLog", &Game::requestQuestLog, &g_game);
    g_lua.bindSingletonFunction("g_game", "requestQuestLine", &Game::requestQuestLine, &g_game);
    g_lua.bindSingletonFunction("g_game", "equipItem", &Game::equipItem, &g_game);
    g_lua.bindSingletonFunction("g_game", "mount", &Game::mount, &g_game);
    g_lua.bindSingletonFunction("g_game", "requestItemInfo", &Game::requestItemInfo, &g_game);
    g_lua.bindSingletonFunction("g_game", "ping", &Game::ping, &g_game);
    g_lua.bindSingletonFunction("g_game", "setPingDelay", &Game::setPingDelay, &g_game);
    g_lua.bindSingletonFunction("g_game", "changeMapAwareRange", &Game::changeMapAwareRange, &g_game);
    g_lua.bindSingletonFunction("g_game", "canPerformGameAction", &Game::canPerformGameAction, &g_game);
    g_lua.bindSingletonFunction("g_game", "canReportBugs", &Game::canReportBugs, &g_game);
    g_lua.bindSingletonFunction("g_game", "checkBotProtection", &Game::checkBotProtection, &g_game);
    g_lua.bindSingletonFunction("g_game", "isOnline", &Game::isOnline, &g_game);
    g_lua.bindSingletonFunction("g_game", "isLogging", &Game::isLogging, &g_game);
    g_lua.bindSingletonFunction("g_game", "isDead", &Game::isDead, &g_game);
    g_lua.bindSingletonFunction("g_game", "isAttacking", &Game::isAttacking, &g_game);
    g_lua.bindSingletonFunction("g_game", "isFollowing", &Game::isFollowing, &g_game);
    g_lua.bindSingletonFunction("g_game", "isConnectionOk", &Game::isConnectionOk, &g_game);
    g_lua.bindSingletonFunction("g_game", "getPing", &Game::getPing, &g_game);
    g_lua.bindSingletonFunction("g_game", "getContainer", &Game::getContainer, &g_game);
    g_lua.bindSingletonFunction("g_game", "getContainers", &Game::getContainers, &g_game);
    g_lua.bindSingletonFunction("g_game", "getVips", &Game::getVips, &g_game);
    g_lua.bindSingletonFunction("g_game", "getAttackingCreature", &Game::getAttackingCreature, &g_game);
    g_lua.bindSingletonFunction("g_game", "getFollowingCreature", &Game::getFollowingCreature, &g_game);
    g_lua.bindSingletonFunction("g_game", "getServerBeat", &Game::getServerBeat, &g_game);
    g_lua.bindSingletonFunction("g_game", "getLocalPlayer", &Game::getLocalPlayer, &g_game);
    g_lua.bindSingletonFunction("g_game", "getProtocolGame", &Game::getProtocolGame, &g_game);
    g_lua.bindSingletonFunction("g_game", "getProtocolVersion", &Game::getProtocolVersion, &g_game);
    g_lua.bindSingletonFunction("g_game", "setProtocolVersion", &Game::setProtocolVersion, &g_game);
    g_lua.bindSingletonFunction("g_game", "getClientVersion", &Game::getClientVersion, &g_game);
    g_lua.bindSingletonFunction("g_game", "setClientVersion", &Game::setClientVersion, &g_game);
    g_lua.bindSingletonFunction("g_game", "setCustomOs", &Game::setCustomOs, &g_game);
    g_lua.bindSingletonFunction("g_game", "getOs", &Game::getOs, &g_game);
    g_lua.bindSingletonFunction("g_game", "getCharacterName", &Game::getCharacterName, &g_game);
    g_lua.bindSingletonFunction("g_game", "getWorldName", &Game::getWorldName, &g_game);
    g_lua.bindSingletonFunction("g_game", "getGMActions", &Game::getGMActions, &g_game);
    g_lua.bindSingletonFunction("g_game", "getFeature", &Game::getFeature, &g_game);
    g_lua.bindSingletonFunction("g_game", "setFeature", &Game::setFeature, &g_game);
    g_lua.bindSingletonFunction("g_game", "enableFeature", &Game::enableFeature, &g_game);
    g_lua.bindSingletonFunction("g_game", "disableFeature", &Game::disableFeature, &g_game);
    g_lua.bindSingletonFunction("g_game", "isGM", &Game::isGM, &g_game);
    g_lua.bindSingletonFunction("g_game", "answerModalDialog", &Game::answerModalDialog, &g_game);
    g_lua.bindSingletonFunction("g_game", "browseField", &Game::browseField, &g_game);
    g_lua.bindSingletonFunction("g_game", "seekInContainer", &Game::seekInContainer, &g_game);
    g_lua.bindSingletonFunction("g_game", "getLastWalkDir", &Game::getLastWalkDir, &g_game);
    g_lua.bindSingletonFunction("g_game", "buyStoreOffer", &Game::buyStoreOffer, &g_game);
    g_lua.bindSingletonFunction("g_game", "requestTransactionHistory", &Game::requestTransactionHistory, &g_game);
    g_lua.bindSingletonFunction("g_game", "requestStoreOffers", &Game::requestStoreOffers, &g_game);
    g_lua.bindSingletonFunction("g_game", "openStore", &Game::openStore, &g_game);
    g_lua.bindSingletonFunction("g_game", "transferCoins", &Game::transferCoins, &g_game);
    g_lua.bindSingletonFunction("g_game", "openTransactionHistory", &Game::openTransactionHistory, &g_game);

    g_lua.registerSingletonClass("g_shaders");
    g_lua.bindSingletonFunction("g_shaders", "createShader", &ShaderManager::createShader, &g_shaders);
    g_lua.bindSingletonFunction("g_shaders", "createFragmentShader", &ShaderManager::createFragmentShader, &g_shaders);
    g_lua.bindSingletonFunction("g_shaders", "createFragmentShaderFromCode", &ShaderManager::createFragmentShaderFromCode, &g_shaders);
    g_lua.bindSingletonFunction("g_shaders", "createItemShader", &ShaderManager::createItemShader, &g_shaders);
    g_lua.bindSingletonFunction("g_shaders", "createMapShader", &ShaderManager::createMapShader, &g_shaders);
    g_lua.bindSingletonFunction("g_shaders", "getDefaultItemShader", &ShaderManager::getDefaultItemShader, &g_shaders);
    g_lua.bindSingletonFunction("g_shaders", "getDefaultMapShader", &ShaderManager::getDefaultMapShader, &g_shaders);
    g_lua.bindSingletonFunction("g_shaders", "getShader", &ShaderManager::getShader, &g_shaders);

    g_lua.bindGlobalFunction("getOutfitColor", Outfit::getColor);
    g_lua.bindGlobalFunction("getAngleFromPos", Position::getAngleFromPositions);
    g_lua.bindGlobalFunction("getDirectionFromPos", Position::getDirectionFromPositions);

    g_lua.registerClass<ProtocolGame, Protocol>();
    g_lua.bindClassStaticFunction<ProtocolGame>("create", []{ return ProtocolGamePtr(new ProtocolGame); });
    g_lua.bindClassMemberFunction<ProtocolGame>("login", &ProtocolGame::login);
    g_lua.bindClassMemberFunction<ProtocolGame>("sendExtendedOpcode", &ProtocolGame::sendExtendedOpcode);
    g_lua.bindClassMemberFunction<ProtocolGame>("addPosition", &ProtocolGame::addPosition);
    g_lua.bindClassMemberFunction<ProtocolGame>("setMapDescription", &ProtocolGame::setMapDescription);
    g_lua.bindClassMemberFunction<ProtocolGame>("setFloorDescription", &ProtocolGame::setFloorDescription);
    g_lua.bindClassMemberFunction<ProtocolGame>("setTileDescription", &ProtocolGame::setTileDescription);
    g_lua.bindClassMemberFunction<ProtocolGame>("getOutfit", &ProtocolGame::getOutfit);
    g_lua.bindClassMemberFunction<ProtocolGame>("getThing", &ProtocolGame::getThing);
    g_lua.bindClassMemberFunction<ProtocolGame>("getCreature", &ProtocolGame::getCreature);
    g_lua.bindClassMemberFunction<ProtocolGame>("getItem", &ProtocolGame::getItem);
    g_lua.bindClassMemberFunction<ProtocolGame>("getPosition", &ProtocolGame::getPosition);

    g_lua.registerClass<Container>();
    g_lua.bindClassMemberFunction<Container>("getItem", &Container::getItem);
    g_lua.bindClassMemberFunction<Container>("getItems", &Container::getItems);
    g_lua.bindClassMemberFunction<Container>("getItemsCount", &Container::getItemsCount);
    g_lua.bindClassMemberFunction<Container>("getSlotPosition", &Container::getSlotPosition);
    g_lua.bindClassMemberFunction<Container>("getName", &Container::getName);
    g_lua.bindClassMemberFunction<Container>("getId", &Container::getId);
    g_lua.bindClassMemberFunction<Container>("getCapacity", &Container::getCapacity);
    g_lua.bindClassMemberFunction<Container>("getContainerItem", &Container::getContainerItem);
    g_lua.bindClassMemberFunction<Container>("hasParent", &Container::hasParent);
    g_lua.bindClassMemberFunction<Container>("isClosed", &Container::isClosed);
    g_lua.bindClassMemberFunction<Container>("isUnlocked", &Container::isUnlocked);
    g_lua.bindClassMemberFunction<Container>("hasPages", &Container::hasPages);
    g_lua.bindClassMemberFunction<Container>("getSize", &Container::getSize);
    g_lua.bindClassMemberFunction<Container>("getFirstIndex", &Container::getFirstIndex);

    g_lua.registerClass<Thing>();
    g_lua.bindClassMemberFunction<Thing>("setId", &Thing::setId);
    g_lua.bindClassMemberFunction<Thing>("setPosition", &Thing::setPosition);
    g_lua.bindClassMemberFunction<Thing>("getId", &Thing::getId);
    g_lua.bindClassMemberFunction<Thing>("getPosition", &Thing::getPosition);
    g_lua.bindClassMemberFunction<Thing>("getStackPriority", &Thing::getStackPriority);
    g_lua.bindClassMemberFunction<Thing>("getStackPos", &Thing::getStackPos);
    g_lua.bindClassMemberFunction<Thing>("getAnimationPhases", &Thing::getAnimationPhases);
    g_lua.bindClassMemberFunction<Thing>("getTile", &Thing::getTile);
    g_lua.bindClassMemberFunction<Thing>("isItem", &Thing::isItem);
    g_lua.bindClassMemberFunction<Thing>("isMonster", &Thing::isMonster);
    g_lua.bindClassMemberFunction<Thing>("isNpc", &Thing::isNpc);
    g_lua.bindClassMemberFunction<Thing>("isCreature", &Thing::isCreature);
    g_lua.bindClassMemberFunction<Thing>("isEffect", &Thing::isEffect);
    g_lua.bindClassMemberFunction<Thing>("isMissile", &Thing::isMissile);
    g_lua.bindClassMemberFunction<Thing>("isPlayer", &Thing::isPlayer);
    g_lua.bindClassMemberFunction<Thing>("isLocalPlayer", &Thing::isLocalPlayer);
    g_lua.bindClassMemberFunction<Thing>("isAnimatedText", &Thing::isAnimatedText);
    g_lua.bindClassMemberFunction<Thing>("isStaticText", &Thing::isStaticText);
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
    g_lua.bindClassMemberFunction<Thing>("isMarketable", &Thing::isMarketable);
    g_lua.bindClassMemberFunction<Thing>("getMarketData", &Thing::getMarketData);
    g_lua.bindClassMemberFunction<Thing>("isUsable", &Thing::isUsable);
    g_lua.bindClassMemberFunction<Thing>("isWrapable", &Thing::isWrapable);
    g_lua.bindClassMemberFunction<Thing>("isUnwrapable", &Thing::isUnwrapable);
    g_lua.bindClassMemberFunction<Thing>("isTopEffect", &Thing::isTopEffect);
    g_lua.bindClassMemberFunction<Thing>("isLyingCorpse", &Thing::isLyingCorpse);
    g_lua.bindClassMemberFunction<Thing>("getParentContainer", &Thing::getParentContainer);

    g_lua.registerClass<House>();
    g_lua.bindClassStaticFunction<House>("create", []{ return HousePtr(new House); });
    g_lua.bindClassMemberFunction<House>("setId", &House::setId);
    g_lua.bindClassMemberFunction<House>("getId", &House::getId);
    g_lua.bindClassMemberFunction<House>("setName", &House::setName);
    g_lua.bindClassMemberFunction<House>("getName", &House::getName);
    g_lua.bindClassMemberFunction<House>("setTownId", &House::setTownId);
    g_lua.bindClassMemberFunction<House>("getTownId", &House::getTownId);
    g_lua.bindClassMemberFunction<House>("setTile", &House::setTile);
    g_lua.bindClassMemberFunction<House>("getTile", &House::getTile);
    g_lua.bindClassMemberFunction<House>("setEntry", &House::setEntry);
    g_lua.bindClassMemberFunction<House>("getEntry", &House::getEntry);
    g_lua.bindClassMemberFunction<House>("addDoor", &House::addDoor);
    g_lua.bindClassMemberFunction<House>("removeDoor", &House::removeDoor);
    g_lua.bindClassMemberFunction<House>("removeDoorById", &House::removeDoorById);
    g_lua.bindClassMemberFunction<House>("setSize", &House::setSize);
    g_lua.bindClassMemberFunction<House>("getSize", &House::getSize);
    g_lua.bindClassMemberFunction<House>("setRent", &House::setRent);
    g_lua.bindClassMemberFunction<House>("getRent", &House::getRent);

    g_lua.registerClass<Spawn>();
    g_lua.bindClassStaticFunction<Spawn>("create", []{ return SpawnPtr(new Spawn); });
    g_lua.bindClassMemberFunction<Spawn>("setRadius", &Spawn::setRadius);
    g_lua.bindClassMemberFunction<Spawn>("getRadius", &Spawn::getRadius);
    g_lua.bindClassMemberFunction<Spawn>("setCenterPos", &Spawn::setCenterPos);
    g_lua.bindClassMemberFunction<Spawn>("getCenterPos", &Spawn::getCenterPos);
    g_lua.bindClassMemberFunction<Spawn>("addCreature", &Spawn::addCreature);
    g_lua.bindClassMemberFunction<Spawn>("removeCreature", &Spawn::removeCreature);
    g_lua.bindClassMemberFunction<Spawn>("getCreatures", &Spawn::getCreatures);

    g_lua.registerClass<Town>();
    g_lua.bindClassStaticFunction<Town>("create", []{ return TownPtr(new Town); });
    g_lua.bindClassMemberFunction<Town>("setId", &Town::setId);
    g_lua.bindClassMemberFunction<Town>("setName", &Town::setName);
    g_lua.bindClassMemberFunction<Town>("setPos", &Town::setPos);
    g_lua.bindClassMemberFunction<Town>("setTemplePos", &Town::setPos); // alternative method
    g_lua.bindClassMemberFunction<Town>("getId", &Town::getId);
    g_lua.bindClassMemberFunction<Town>("getName", &Town::getName);
    g_lua.bindClassMemberFunction<Town>("getPos", &Town::getPos);
    g_lua.bindClassMemberFunction<Town>("getTemplePos", &Town::getPos); // alternative method

    g_lua.registerClass<CreatureType>();
    g_lua.bindClassStaticFunction<CreatureType>("create", []{ return CreatureTypePtr(new CreatureType); });
    g_lua.bindClassMemberFunction<CreatureType>("setName", &CreatureType::setName);
    g_lua.bindClassMemberFunction<CreatureType>("setOutfit", &CreatureType::setOutfit);
    g_lua.bindClassMemberFunction<CreatureType>("setSpawnTime", &CreatureType::setSpawnTime);
    g_lua.bindClassMemberFunction<CreatureType>("getName", &CreatureType::getName);
    g_lua.bindClassMemberFunction<CreatureType>("getOutfit", &CreatureType::getOutfit);
    g_lua.bindClassMemberFunction<CreatureType>("getSpawnTime", &CreatureType::getSpawnTime);
    g_lua.bindClassMemberFunction<CreatureType>("cast", &CreatureType::cast);

    g_lua.registerClass<Creature, Thing>();
    g_lua.bindClassStaticFunction<Creature>("create", []{ return CreaturePtr(new Creature); });
    g_lua.bindClassMemberFunction<Creature>("getId", &Creature::getId);
    g_lua.bindClassMemberFunction<Creature>("getName", &Creature::getName);
    g_lua.bindClassMemberFunction<Creature>("getHealthPercent", &Creature::getHealthPercent);
    g_lua.bindClassMemberFunction<Creature>("getSpeed", &Creature::getSpeed);
    g_lua.bindClassMemberFunction<Creature>("getBaseSpeed", &Creature::getBaseSpeed);
    g_lua.bindClassMemberFunction<Creature>("getSkull", &Creature::getSkull);
    g_lua.bindClassMemberFunction<Creature>("getShield", &Creature::getShield);
    g_lua.bindClassMemberFunction<Creature>("getEmblem", &Creature::getEmblem);
    g_lua.bindClassMemberFunction<Creature>("getType", &Creature::getType);
    g_lua.bindClassMemberFunction<Creature>("getIcon", &Creature::getIcon);
    g_lua.bindClassMemberFunction<Creature>("setOutfit", &Creature::setOutfit);
    g_lua.bindClassMemberFunction<Creature>("getOutfit", &Creature::getOutfit);
    g_lua.bindClassMemberFunction<Creature>("setOutfitColor", &Creature::setOutfitColor);
    g_lua.bindClassMemberFunction<Creature>("getDirection", &Creature::getDirection);
    g_lua.bindClassMemberFunction<Creature>("getStepDuration", &Creature::getStepDuration);
    g_lua.bindClassMemberFunction<Creature>("getStepProgress", &Creature::getStepProgress);
    g_lua.bindClassMemberFunction<Creature>("getWalkTicksElapsed", &Creature::getWalkTicksElapsed);
    g_lua.bindClassMemberFunction<Creature>("getStepTicksLeft", &Creature::getStepTicksLeft);
    g_lua.bindClassMemberFunction<Creature>("setDirection", &Creature::setDirection);
    g_lua.bindClassMemberFunction<Creature>("setSkullTexture", &Creature::setSkullTexture);
    g_lua.bindClassMemberFunction<Creature>("setShieldTexture", &Creature::setShieldTexture);
    g_lua.bindClassMemberFunction<Creature>("setEmblemTexture", &Creature::setEmblemTexture);
    g_lua.bindClassMemberFunction<Creature>("setTypeTexture", &Creature::setTypeTexture);
    g_lua.bindClassMemberFunction<Creature>("setIconTexture", &Creature::setIconTexture);
    g_lua.bindClassMemberFunction<Creature>("showStaticSquare", &Creature::showStaticSquare);
    g_lua.bindClassMemberFunction<Creature>("hideStaticSquare", &Creature::hideStaticSquare);
    g_lua.bindClassMemberFunction<Creature>("isWalking", &Creature::isWalking);
    g_lua.bindClassMemberFunction<Creature>("isInvisible", &Creature::isInvisible);
    g_lua.bindClassMemberFunction<Creature>("isDead", &Creature::isDead);
    g_lua.bindClassMemberFunction<Creature>("isRemoved", &Creature::isRemoved);
    g_lua.bindClassMemberFunction<Creature>("canBeSeen", &Creature::canBeSeen);
    g_lua.bindClassMemberFunction<Creature>("jump", &Creature::jump);

    g_lua.registerClass<ItemType>();
    g_lua.bindClassMemberFunction<ItemType>("getServerId", &ItemType::getServerId);
    g_lua.bindClassMemberFunction<ItemType>("getClientId", &ItemType::getClientId);
    g_lua.bindClassMemberFunction<ItemType>("isWritable",  &ItemType::isWritable);

    g_lua.registerClass<ThingType>();
    g_lua.bindClassStaticFunction<ThingType>("create", []{ return ThingTypePtr(new ThingType); });
    g_lua.bindClassMemberFunction<ThingType>("getId", &ThingType::getId);
    g_lua.bindClassMemberFunction<ThingType>("getClothSlot", &ThingType::getClothSlot);
    g_lua.bindClassMemberFunction<ThingType>("getCategory", &ThingType::getCategory);
    g_lua.bindClassMemberFunction<ThingType>("getSize", &ThingType::getSize);
    g_lua.bindClassMemberFunction<ThingType>("getWidth", &ThingType::getWidth);
    g_lua.bindClassMemberFunction<ThingType>("getHeight", &ThingType::getHeight);
    g_lua.bindClassMemberFunction<ThingType>("getDisplacement", &ThingType::getDisplacement);
    g_lua.bindClassMemberFunction<ThingType>("getDisplacementX", &ThingType::getDisplacementX);
    g_lua.bindClassMemberFunction<ThingType>("getDisplacementY", &ThingType::getDisplacementY);
    g_lua.bindClassMemberFunction<ThingType>("getExactSize", &ThingType::getExactSize);
    g_lua.bindClassMemberFunction<ThingType>("getRealSize", &ThingType::getRealSize);
    g_lua.bindClassMemberFunction<ThingType>("getLayers", &ThingType::getLayers);
    g_lua.bindClassMemberFunction<ThingType>("getNumPatternX", &ThingType::getNumPatternX);
    g_lua.bindClassMemberFunction<ThingType>("getNumPatternY", &ThingType::getNumPatternY);
    g_lua.bindClassMemberFunction<ThingType>("getNumPatternZ", &ThingType::getNumPatternZ);
    g_lua.bindClassMemberFunction<ThingType>("getAnimationPhases", &ThingType::getAnimationPhases);
    g_lua.bindClassMemberFunction<ThingType>("getGroundSpeed", &ThingType::getGroundSpeed);
    g_lua.bindClassMemberFunction<ThingType>("getMaxTextLength", &ThingType::getMaxTextLength);
    g_lua.bindClassMemberFunction<ThingType>("getLight", &ThingType::getLight);
    g_lua.bindClassMemberFunction<ThingType>("getMinimapColor", &ThingType::getMinimapColor);
    g_lua.bindClassMemberFunction<ThingType>("getLensHelp", &ThingType::getLensHelp);
    g_lua.bindClassMemberFunction<ThingType>("getClothSlot", &ThingType::getClothSlot);
    g_lua.bindClassMemberFunction<ThingType>("getElevation", &ThingType::getElevation);
    g_lua.bindClassMemberFunction<ThingType>("isGround", &ThingType::isGround);
    g_lua.bindClassMemberFunction<ThingType>("isGroundBorder", &ThingType::isGroundBorder);
    g_lua.bindClassMemberFunction<ThingType>("isOnBottom", &ThingType::isOnBottom);
    g_lua.bindClassMemberFunction<ThingType>("isOnTop", &ThingType::isOnTop);
    g_lua.bindClassMemberFunction<ThingType>("isContainer", &ThingType::isContainer);
    g_lua.bindClassMemberFunction<ThingType>("isStackable", &ThingType::isStackable);
    g_lua.bindClassMemberFunction<ThingType>("isForceUse", &ThingType::isForceUse);
    g_lua.bindClassMemberFunction<ThingType>("isMultiUse", &ThingType::isMultiUse);
    g_lua.bindClassMemberFunction<ThingType>("isWritable", &ThingType::isWritable);
    g_lua.bindClassMemberFunction<ThingType>("isChargeable", &ThingType::isChargeable);
    g_lua.bindClassMemberFunction<ThingType>("isWritableOnce", &ThingType::isWritableOnce);
    g_lua.bindClassMemberFunction<ThingType>("isFluidContainer", &ThingType::isFluidContainer);
    g_lua.bindClassMemberFunction<ThingType>("isSplash", &ThingType::isSplash);
    g_lua.bindClassMemberFunction<ThingType>("isNotWalkable", &ThingType::isNotWalkable);
    g_lua.bindClassMemberFunction<ThingType>("isNotMoveable", &ThingType::isNotMoveable);
    g_lua.bindClassMemberFunction<ThingType>("blockProjectile", &ThingType::blockProjectile);
    g_lua.bindClassMemberFunction<ThingType>("isNotPathable", &ThingType::isNotPathable);
    g_lua.bindClassMemberFunction<ThingType>("setPathable", &ThingType::setPathable);
    g_lua.bindClassMemberFunction<ThingType>("isPickupable", &ThingType::isPickupable);
    g_lua.bindClassMemberFunction<ThingType>("isHangable", &ThingType::isHangable);
    g_lua.bindClassMemberFunction<ThingType>("isHookSouth", &ThingType::isHookSouth);
    g_lua.bindClassMemberFunction<ThingType>("isHookEast", &ThingType::isHookEast);
    g_lua.bindClassMemberFunction<ThingType>("isRotateable", &ThingType::isRotateable);
    g_lua.bindClassMemberFunction<ThingType>("hasLight", &ThingType::hasLight);
    g_lua.bindClassMemberFunction<ThingType>("isDontHide", &ThingType::isDontHide);
    g_lua.bindClassMemberFunction<ThingType>("isTranslucent", &ThingType::isTranslucent);
    g_lua.bindClassMemberFunction<ThingType>("hasDisplacement", &ThingType::hasDisplacement);
    g_lua.bindClassMemberFunction<ThingType>("hasElevation", &ThingType::hasElevation);
    g_lua.bindClassMemberFunction<ThingType>("isLyingCorpse", &ThingType::isLyingCorpse);
    g_lua.bindClassMemberFunction<ThingType>("isAnimateAlways", &ThingType::isAnimateAlways);
    g_lua.bindClassMemberFunction<ThingType>("hasMiniMapColor", &ThingType::hasMiniMapColor);
    g_lua.bindClassMemberFunction<ThingType>("hasLensHelp", &ThingType::hasLensHelp);
    g_lua.bindClassMemberFunction<ThingType>("isFullGround", &ThingType::isFullGround);
    g_lua.bindClassMemberFunction<ThingType>("isIgnoreLook", &ThingType::isIgnoreLook);
    g_lua.bindClassMemberFunction<ThingType>("isCloth", &ThingType::isCloth);
    g_lua.bindClassMemberFunction<ThingType>("isMarketable", &ThingType::isMarketable);
    g_lua.bindClassMemberFunction<ThingType>("getMarketData", &ThingType::getMarketData);
    g_lua.bindClassMemberFunction<ThingType>("isUsable", &ThingType::isUsable);
    g_lua.bindClassMemberFunction<ThingType>("isWrapable", &ThingType::isWrapable);
    g_lua.bindClassMemberFunction<ThingType>("isUnwrapable", &ThingType::isUnwrapable);
    g_lua.bindClassMemberFunction<ThingType>("isTopEffect", &ThingType::isTopEffect);
    g_lua.bindClassMemberFunction<ThingType>("getSprites", &ThingType::getSprites);
    g_lua.bindClassMemberFunction<ThingType>("hasAttribute", &ThingType::hasAttr);
    g_lua.bindClassMemberFunction<ThingType>("exportImage", &ThingType::exportImage);

    g_lua.registerClass<Item, Thing>();
    g_lua.bindClassStaticFunction<Item>("create", &Item::create);
    g_lua.bindClassStaticFunction<Item>("createOtb", &Item::createFromOtb);
    g_lua.bindClassMemberFunction<Item>("clone", &Item::clone);
    g_lua.bindClassMemberFunction<Item>("getContainerItems", &Item::getContainerItems);
    g_lua.bindClassMemberFunction<Item>("getContainerItem", &Item::getContainerItem);
    g_lua.bindClassMemberFunction<Item>("addContainerItem", &Item::addContainerItem);
    g_lua.bindClassMemberFunction<Item>("addContainerItemIndexed", &Item::addContainerItemIndexed);
    g_lua.bindClassMemberFunction<Item>("removeContainerItem", &Item::removeContainerItem);
    g_lua.bindClassMemberFunction<Item>("clearContainerItems", &Item::clearContainerItems);
    g_lua.bindClassMemberFunction<Item>("getContainerItem", &Item::getContainerItem);
    g_lua.bindClassMemberFunction<Item>("setCount", &Item::setCount);
    g_lua.bindClassMemberFunction<Item>("getCount", &Item::getCount);
    g_lua.bindClassMemberFunction<Item>("getSubType", &Item::getSubType);
    g_lua.bindClassMemberFunction<Item>("getId", &Item::getId);
    g_lua.bindClassMemberFunction<Item>("getServerId", &Item::getServerId);
    g_lua.bindClassMemberFunction<Item>("getName", &Item::getName);
    g_lua.bindClassMemberFunction<Item>("getDescription", &Item::getDescription);
    g_lua.bindClassMemberFunction<Item>("getText", &Item::getText);
    g_lua.bindClassMemberFunction<Item>("setDescription", &Item::setDescription);
    g_lua.bindClassMemberFunction<Item>("setText", &Item::setText);
    g_lua.bindClassMemberFunction<Item>("getUniqueId", &Item::getUniqueId);
    g_lua.bindClassMemberFunction<Item>("getActionId", &Item::getActionId);
    g_lua.bindClassMemberFunction<Item>("setUniqueId", &Item::setUniqueId);
    g_lua.bindClassMemberFunction<Item>("setActionId", &Item::setActionId);
    g_lua.bindClassMemberFunction<Item>("getTeleportDestination", &Item::getTeleportDestination);
    g_lua.bindClassMemberFunction<Item>("setTeleportDestination", &Item::setTeleportDestination);
    g_lua.bindClassMemberFunction<Item>("isStackable", &Item::isStackable);
    g_lua.bindClassMemberFunction<Item>("isMarketable", &Item::isMarketable);
    g_lua.bindClassMemberFunction<Item>("isFluidContainer", &Item::isFluidContainer);
    g_lua.bindClassMemberFunction<Item>("getMarketData", &Item::getMarketData);
    g_lua.bindClassMemberFunction<Item>("getClothSlot", &Item::getClothSlot);

    g_lua.registerClass<Effect, Thing>();
    g_lua.bindClassStaticFunction<Effect>("create", []{ return EffectPtr(new Effect); });
    g_lua.bindClassMemberFunction<Effect>("setId", &Effect::setId);

    g_lua.registerClass<Missile, Thing>();
    g_lua.bindClassStaticFunction<Missile>("create", []{ return MissilePtr(new Missile); });
    g_lua.bindClassMemberFunction<Missile>("setId", &Missile::setId);

    g_lua.registerClass<StaticText, Thing>();
    g_lua.bindClassStaticFunction<StaticText>("create", []{ return StaticTextPtr(new StaticText); });
    g_lua.bindClassMemberFunction<StaticText>("addMessage", &StaticText::addMessage);
    g_lua.bindClassMemberFunction<StaticText>("setText", &StaticText::setText);
    g_lua.bindClassMemberFunction<StaticText>("setFont", &StaticText::setFont);
    g_lua.bindClassMemberFunction<StaticText>("setColor", &StaticText::setColor);
    g_lua.bindClassMemberFunction<StaticText>("getColor", &StaticText::getColor);

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
    g_lua.bindClassMemberFunction<LocalPlayer>("setTotalCapacity", &LocalPlayer::setTotalCapacity);
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
    g_lua.bindClassMemberFunction<LocalPlayer>("getSkillBaseLevel", &LocalPlayer::getSkillBaseLevel);
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
    g_lua.bindClassMemberFunction<LocalPlayer>("getOfflineTrainingTime", &LocalPlayer::getOfflineTrainingTime);
    g_lua.bindClassMemberFunction<LocalPlayer>("getRegenerationTime", &LocalPlayer::getRegenerationTime);
    g_lua.bindClassMemberFunction<LocalPlayer>("getBaseMagicLevel", &LocalPlayer::getBaseMagicLevel);
    g_lua.bindClassMemberFunction<LocalPlayer>("getTotalCapacity", &LocalPlayer::getTotalCapacity);
    g_lua.bindClassMemberFunction<LocalPlayer>("getInventoryItem", &LocalPlayer::getInventoryItem);
    g_lua.bindClassMemberFunction<LocalPlayer>("getVocation", &LocalPlayer::getVocation);
    g_lua.bindClassMemberFunction<LocalPlayer>("getBlessings", &LocalPlayer::getBlessings);
    g_lua.bindClassMemberFunction<LocalPlayer>("isPremium", &LocalPlayer::isPremium);
    g_lua.bindClassMemberFunction<LocalPlayer>("isKnown", &LocalPlayer::isKnown);
    g_lua.bindClassMemberFunction<LocalPlayer>("isPreWalking", &LocalPlayer::isPreWalking);
    g_lua.bindClassMemberFunction<LocalPlayer>("hasSight", &LocalPlayer::hasSight);
    g_lua.bindClassMemberFunction<LocalPlayer>("isAutoWalking", &LocalPlayer::isAutoWalking);
    g_lua.bindClassMemberFunction<LocalPlayer>("isServerWalking", &LocalPlayer::isServerWalking);
    g_lua.bindClassMemberFunction<LocalPlayer>("stopAutoWalk", &LocalPlayer::stopAutoWalk);
    g_lua.bindClassMemberFunction<LocalPlayer>("autoWalk", &LocalPlayer::autoWalk);

    g_lua.registerClass<Tile>();
    g_lua.bindClassMemberFunction<Tile>("clean", &Tile::clean);
    g_lua.bindClassMemberFunction<Tile>("addThing", &Tile::addThing);
    g_lua.bindClassMemberFunction<Tile>("getThing", &Tile::getThing);
    g_lua.bindClassMemberFunction<Tile>("getThings", &Tile::getThings);
    g_lua.bindClassMemberFunction<Tile>("getItems", &Tile::getItems);
    g_lua.bindClassMemberFunction<Tile>("getThingStackPos", &Tile::getThingStackPos);
    g_lua.bindClassMemberFunction<Tile>("getThingCount", &Tile::getThingCount);
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
    g_lua.bindClassMemberFunction<Tile>("isHouseTile", &Tile::isHouseTile);
    g_lua.bindClassMemberFunction<Tile>("isFullGround", &Tile::isFullGround);
    g_lua.bindClassMemberFunction<Tile>("isFullyOpaque", &Tile::isFullyOpaque);
    g_lua.bindClassMemberFunction<Tile>("isLookPossible", &Tile::isLookPossible);
    g_lua.bindClassMemberFunction<Tile>("hasCreature", &Tile::hasCreature);
    g_lua.bindClassMemberFunction<Tile>("isEmpty", &Tile::isEmpty);
    g_lua.bindClassMemberFunction<Tile>("isClickable", &Tile::isClickable);
    g_lua.bindClassMemberFunction<Tile>("isPathable", &Tile::isPathable);
    g_lua.bindClassMemberFunction<Tile>("overwriteMinimapColor", &Tile::overwriteMinimapColor);
    g_lua.bindClassMemberFunction<Tile>("select", &Tile::select);
    g_lua.bindClassMemberFunction<Tile>("unselect", &Tile::unselect);
    g_lua.bindClassMemberFunction<Tile>("isSelected", &Tile::isSelected);
    g_lua.bindClassMemberFunction<Tile>("remFlag", &Tile::remFlag);
    g_lua.bindClassMemberFunction<Tile>("setFlag", &Tile::setFlag);
    g_lua.bindClassMemberFunction<Tile>("setFlags", &Tile::setFlags);
    g_lua.bindClassMemberFunction<Tile>("getFlags", &Tile::getFlags);
    g_lua.bindClassMemberFunction<Tile>("hasFlag", &Tile::hasFlag);

    g_lua.registerClass<UIItem, UIWidget>();
    g_lua.bindClassStaticFunction<UIItem>("create", []{ return UIItemPtr(new UIItem); });
    g_lua.bindClassMemberFunction<UIItem>("setItemId", &UIItem::setItemId);
    g_lua.bindClassMemberFunction<UIItem>("setItemCount", &UIItem::setItemCount);
    g_lua.bindClassMemberFunction<UIItem>("setItemSubType", &UIItem::setItemSubType);
    g_lua.bindClassMemberFunction<UIItem>("setItemVisible", &UIItem::setItemVisible);
    g_lua.bindClassMemberFunction<UIItem>("setItem", &UIItem::setItem);
    g_lua.bindClassMemberFunction<UIItem>("setVirtual", &UIItem::setVirtual);
    g_lua.bindClassMemberFunction<UIItem>("clearItem", &UIItem::clearItem);
    g_lua.bindClassMemberFunction<UIItem>("getItemId", &UIItem::getItemId);
    g_lua.bindClassMemberFunction<UIItem>("getItemCount", &UIItem::getItemCount);
    g_lua.bindClassMemberFunction<UIItem>("getItemSubType", &UIItem::getItemSubType);
    g_lua.bindClassMemberFunction<UIItem>("getItem", &UIItem::getItem);
    g_lua.bindClassMemberFunction<UIItem>("isVirtual", &UIItem::isVirtual);
    g_lua.bindClassMemberFunction<UIItem>("isItemVisible", &UIItem::isItemVisible);

    g_lua.registerClass<UISprite, UIWidget>();
    g_lua.bindClassStaticFunction<UISprite>("create", []{ return UISpritePtr(new UISprite); });
    g_lua.bindClassMemberFunction<UISprite>("setSpriteId", &UISprite::setSpriteId);
    g_lua.bindClassMemberFunction<UISprite>("clearSprite", &UISprite::clearSprite);
    g_lua.bindClassMemberFunction<UISprite>("getSpriteId", &UISprite::getSpriteId);
    g_lua.bindClassMemberFunction<UISprite>("setSpriteColor", &UISprite::setSpriteColor);
    g_lua.bindClassMemberFunction<UISprite>("hasSprite", &UISprite::hasSprite);

    g_lua.registerClass<UICreature, UIWidget>();
    g_lua.bindClassStaticFunction<UICreature>("create", []{ return UICreaturePtr(new UICreature); } );
    g_lua.bindClassMemberFunction<UICreature>("setCreature", &UICreature::setCreature);
    g_lua.bindClassMemberFunction<UICreature>("setOutfit", &UICreature::setOutfit);
    g_lua.bindClassMemberFunction<UICreature>("setFixedCreatureSize", &UICreature::setFixedCreatureSize);
    g_lua.bindClassMemberFunction<UICreature>("getCreature", &UICreature::getCreature);
    g_lua.bindClassMemberFunction<UICreature>("isFixedCreatureSize", &UICreature::isFixedCreatureSize);

    g_lua.registerClass<UIMap, UIWidget>();
    g_lua.bindClassStaticFunction<UIMap>("create", []{ return UIMapPtr(new UIMap); });
    g_lua.bindClassMemberFunction<UIMap>("drawSelf", &UIMap::drawSelf);
    g_lua.bindClassMemberFunction<UIMap>("movePixels", &UIMap::movePixels);
    g_lua.bindClassMemberFunction<UIMap>("setZoom", &UIMap::setZoom);
    g_lua.bindClassMemberFunction<UIMap>("zoomIn", &UIMap::zoomIn);
    g_lua.bindClassMemberFunction<UIMap>("zoomOut", &UIMap::zoomOut);
    g_lua.bindClassMemberFunction<UIMap>("followCreature", &UIMap::followCreature);
    g_lua.bindClassMemberFunction<UIMap>("setCameraPosition", &UIMap::setCameraPosition);
    g_lua.bindClassMemberFunction<UIMap>("setMaxZoomIn", &UIMap::setMaxZoomIn);
    g_lua.bindClassMemberFunction<UIMap>("setMaxZoomOut", &UIMap::setMaxZoomOut);
    g_lua.bindClassMemberFunction<UIMap>("setMultifloor", &UIMap::setMultifloor);
    g_lua.bindClassMemberFunction<UIMap>("lockVisibleFloor", &UIMap::lockVisibleFloor);
    g_lua.bindClassMemberFunction<UIMap>("unlockVisibleFloor", &UIMap::unlockVisibleFloor);
    g_lua.bindClassMemberFunction<UIMap>("setVisibleDimension", &UIMap::setVisibleDimension);
    g_lua.bindClassMemberFunction<UIMap>("setViewMode", &UIMap::setViewMode);
    g_lua.bindClassMemberFunction<UIMap>("setAutoViewMode", &UIMap::setAutoViewMode);
    g_lua.bindClassMemberFunction<UIMap>("setDrawFlags", &UIMap::setDrawFlags);
    g_lua.bindClassMemberFunction<UIMap>("setDrawTexts", &UIMap::setDrawTexts);
    g_lua.bindClassMemberFunction<UIMap>("setDrawNames", &UIMap::setDrawNames);
    g_lua.bindClassMemberFunction<UIMap>("setDrawHealthBars", &UIMap::setDrawHealthBars);
    g_lua.bindClassMemberFunction<UIMap>("setDrawLights", &UIMap::setDrawLights);
    g_lua.bindClassMemberFunction<UIMap>("setDrawManaBar", &UIMap::setDrawManaBar);
    g_lua.bindClassMemberFunction<UIMap>("setAnimated", &UIMap::setAnimated);
    g_lua.bindClassMemberFunction<UIMap>("setKeepAspectRatio", &UIMap::setKeepAspectRatio);
    g_lua.bindClassMemberFunction<UIMap>("setMapShader", &UIMap::setMapShader);
    g_lua.bindClassMemberFunction<UIMap>("setMinimumAmbientLight", &UIMap::setMinimumAmbientLight);
    g_lua.bindClassMemberFunction<UIMap>("setLimitVisibleRange", &UIMap::setLimitVisibleRange);
    g_lua.bindClassMemberFunction<UIMap>("setAddLightMethod", &UIMap::setAddLightMethod);
    g_lua.bindClassMemberFunction<UIMap>("isMultifloor", &UIMap::isMultifloor);
    g_lua.bindClassMemberFunction<UIMap>("isAutoViewModeEnabled", &UIMap::isAutoViewModeEnabled);
    g_lua.bindClassMemberFunction<UIMap>("isDrawingTexts", &UIMap::isDrawingTexts);
    g_lua.bindClassMemberFunction<UIMap>("isDrawingNames", &UIMap::isDrawingNames);
    g_lua.bindClassMemberFunction<UIMap>("isDrawingHealthBars", &UIMap::isDrawingHealthBars);
    g_lua.bindClassMemberFunction<UIMap>("isDrawingLights", &UIMap::isDrawingLights);
    g_lua.bindClassMemberFunction<UIMap>("isDrawingManaBar", &UIMap::isDrawingManaBar);
    g_lua.bindClassMemberFunction<UIMap>("isLimitVisibleRangeEnabled", &UIMap::isLimitVisibleRangeEnabled);
    g_lua.bindClassMemberFunction<UIMap>("isAnimating", &UIMap::isAnimating);
    g_lua.bindClassMemberFunction<UIMap>("isKeepAspectRatioEnabled", &UIMap::isKeepAspectRatioEnabled);
    g_lua.bindClassMemberFunction<UIMap>("getVisibleDimension", &UIMap::getVisibleDimension);
    g_lua.bindClassMemberFunction<UIMap>("getViewMode", &UIMap::getViewMode);
    g_lua.bindClassMemberFunction<UIMap>("getFollowingCreature", &UIMap::getFollowingCreature);
    g_lua.bindClassMemberFunction<UIMap>("getDrawFlags", &UIMap::getDrawFlags);
    g_lua.bindClassMemberFunction<UIMap>("getCameraPosition", &UIMap::getCameraPosition);
    g_lua.bindClassMemberFunction<UIMap>("getPosition", &UIMap::getPosition);
    g_lua.bindClassMemberFunction<UIMap>("getTile", &UIMap::getTile);
    g_lua.bindClassMemberFunction<UIMap>("getMaxZoomIn", &UIMap::getMaxZoomIn);
    g_lua.bindClassMemberFunction<UIMap>("getMaxZoomOut", &UIMap::getMaxZoomOut);
    g_lua.bindClassMemberFunction<UIMap>("getZoom", &UIMap::getZoom);
    g_lua.bindClassMemberFunction<UIMap>("getMapShader", &UIMap::getMapShader);
    g_lua.bindClassMemberFunction<UIMap>("getMinimumAmbientLight", &UIMap::getMinimumAmbientLight);

    g_lua.registerClass<UIMinimap, UIWidget>();
    g_lua.bindClassStaticFunction<UIMinimap>("create", []{ return UIMinimapPtr(new UIMinimap); });
    g_lua.bindClassMemberFunction<UIMinimap>("zoomIn", &UIMinimap::zoomIn);
    g_lua.bindClassMemberFunction<UIMinimap>("zoomOut", &UIMinimap::zoomOut);
    g_lua.bindClassMemberFunction<UIMinimap>("setZoom", &UIMinimap::setZoom);
    g_lua.bindClassMemberFunction<UIMinimap>("setMixZoom", &UIMinimap::setMinZoom);
    g_lua.bindClassMemberFunction<UIMinimap>("setMaxZoom", &UIMinimap::setMaxZoom);
    g_lua.bindClassMemberFunction<UIMinimap>("setCameraPosition", &UIMinimap::setCameraPosition);
    g_lua.bindClassMemberFunction<UIMinimap>("floorUp", &UIMinimap::floorUp);
    g_lua.bindClassMemberFunction<UIMinimap>("floorDown", &UIMinimap::floorDown);
    g_lua.bindClassMemberFunction<UIMinimap>("getTilePoint", &UIMinimap::getTilePoint);
    g_lua.bindClassMemberFunction<UIMinimap>("getTilePosition", &UIMinimap::getTilePosition);
    g_lua.bindClassMemberFunction<UIMinimap>("getTileRect", &UIMinimap::getTileRect);
    g_lua.bindClassMemberFunction<UIMinimap>("getCameraPosition", &UIMinimap::getCameraPosition);
    g_lua.bindClassMemberFunction<UIMinimap>("getMinZoom", &UIMinimap::getMinZoom);
    g_lua.bindClassMemberFunction<UIMinimap>("getMaxZoom", &UIMinimap::getMaxZoom);
    g_lua.bindClassMemberFunction<UIMinimap>("getZoom", &UIMinimap::getZoom);
    g_lua.bindClassMemberFunction<UIMinimap>("getScale", &UIMinimap::getScale);
    g_lua.bindClassMemberFunction<UIMinimap>("anchorPosition", &UIMinimap::anchorPosition);
    g_lua.bindClassMemberFunction<UIMinimap>("fillPosition", &UIMinimap::fillPosition);
    g_lua.bindClassMemberFunction<UIMinimap>("centerInPosition", &UIMinimap::centerInPosition);

    g_lua.registerClass<UIProgressRect, UIWidget>();
    g_lua.bindClassStaticFunction<UIProgressRect>("create", []{ return UIProgressRectPtr(new UIProgressRect); } );
    g_lua.bindClassMemberFunction<UIProgressRect>("setPercent", &UIProgressRect::setPercent);
    g_lua.bindClassMemberFunction<UIProgressRect>("getPercent", &UIProgressRect::getPercent);

    g_lua.registerClass<UIMapAnchorLayout, UIAnchorLayout>();
}
