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

#ifndef CLIENT_DECLARATIONS_H
#define CLIENT_DECLARATIONS_H

#include "global.h"
#include <framework/net/declarations.h>
#include <framework/ui/declarations.h>

// core
class Map;
class Game;
class MapView;
class LightView;
class Tile;
class Thing;
class Item;
class Container;
class Creature;
class Monster;
class Npc;
class Player;
class LocalPlayer;
class Effect;
class Missile;
class AnimatedText;
class StaticText;
class Animator;
class ThingType;
class ItemType;
class House;
class Town;
class CreatureType;
class Spawn;
class TileBlock;

typedef stdext::shared_object_ptr<MapView> MapViewPtr;
typedef stdext::shared_object_ptr<LightView> LightViewPtr;
typedef stdext::shared_object_ptr<Tile> TilePtr;
typedef stdext::shared_object_ptr<Thing> ThingPtr;
typedef stdext::shared_object_ptr<Item> ItemPtr;
typedef stdext::shared_object_ptr<Container> ContainerPtr;
typedef stdext::shared_object_ptr<Creature> CreaturePtr;
typedef stdext::shared_object_ptr<Monster> MonsterPtr;
typedef stdext::shared_object_ptr<Npc> NpcPtr;
typedef stdext::shared_object_ptr<Player> PlayerPtr;
typedef stdext::shared_object_ptr<LocalPlayer> LocalPlayerPtr;
typedef stdext::shared_object_ptr<Effect> EffectPtr;
typedef stdext::shared_object_ptr<Missile> MissilePtr;
typedef stdext::shared_object_ptr<AnimatedText> AnimatedTextPtr;
typedef stdext::shared_object_ptr<StaticText> StaticTextPtr;
typedef stdext::shared_object_ptr<Animator> AnimatorPtr;
typedef stdext::shared_object_ptr<ThingType> ThingTypePtr;
typedef stdext::shared_object_ptr<ItemType> ItemTypePtr;
typedef stdext::shared_object_ptr<House> HousePtr;
typedef stdext::shared_object_ptr<Town> TownPtr;
typedef stdext::shared_object_ptr<CreatureType> CreatureTypePtr;
typedef stdext::shared_object_ptr<Spawn> SpawnPtr;

typedef std::vector<ThingPtr> ThingList;
typedef std::vector<ThingTypePtr> ThingTypeList;
typedef std::vector<ItemTypePtr> ItemTypeList;
typedef std::list<HousePtr> HouseList;
typedef std::list<TownPtr> TownList;
typedef std::list<ItemPtr> ItemList;
typedef std::list<TilePtr> TileList;
typedef std::vector<ItemPtr> ItemVector;
typedef std::unordered_map<Position, TilePtr, PositionHasher> TileMap;
typedef std::unordered_map<Position, CreatureTypePtr, PositionHasher> CreatureMap;
typedef std::unordered_map<Position, SpawnPtr, PositionHasher> SpawnMap;

// net
class ProtocolLogin;
class ProtocolGame;

typedef stdext::shared_object_ptr<ProtocolGame> ProtocolGamePtr;
typedef stdext::shared_object_ptr<ProtocolLogin> ProtocolLoginPtr;

// ui
class UIItem;
class UICreature;
class UIMap;
class UIMinimap;
class UIProgressRect;
class UIMapAnchorLayout;
class UIPositionAnchor;
class UISprite;

typedef stdext::shared_object_ptr<UIItem> UIItemPtr;
typedef stdext::shared_object_ptr<UICreature> UICreaturePtr;
typedef stdext::shared_object_ptr<UISprite> UISpritePtr;
typedef stdext::shared_object_ptr<UIMap> UIMapPtr;
typedef stdext::shared_object_ptr<UIMinimap> UIMinimapPtr;
typedef stdext::shared_object_ptr<UIProgressRect> UIProgressRectPtr;
typedef stdext::shared_object_ptr<UIMapAnchorLayout> UIMapAnchorLayoutPtr;
typedef stdext::shared_object_ptr<UIPositionAnchor> UIPositionAnchorPtr;

#endif
