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

#ifndef OTCLIENT_DECLARATIONS_H
#define OTCLIENT_DECLARATIONS_H

#include "global.h"
#include <framework/net/declarations.h>
#include <framework/ui/declarations.h>

// core
class Map;
class Game;
class MapView;
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
class ThingType;
class ItemType;
class House;
class Town;
class CreatureType;

typedef boost::intrusive_ptr<MapView> MapViewPtr;
typedef boost::intrusive_ptr<Tile> TilePtr;
typedef boost::intrusive_ptr<Thing> ThingPtr;
typedef boost::intrusive_ptr<Item> ItemPtr;
typedef boost::intrusive_ptr<Container> ContainerPtr;
typedef boost::intrusive_ptr<Creature> CreaturePtr;
typedef boost::intrusive_ptr<Monster> MonsterPtr;
typedef boost::intrusive_ptr<Npc> NpcPtr;
typedef boost::intrusive_ptr<Player> PlayerPtr;
typedef boost::intrusive_ptr<LocalPlayer> LocalPlayerPtr;
typedef boost::intrusive_ptr<Effect> EffectPtr;
typedef boost::intrusive_ptr<Missile> MissilePtr;
typedef boost::intrusive_ptr<AnimatedText> AnimatedTextPtr;
typedef boost::intrusive_ptr<StaticText> StaticTextPtr;
typedef boost::intrusive_ptr<ThingType> ThingTypePtr;
typedef boost::intrusive_ptr<ItemType> ItemTypePtr;
typedef boost::intrusive_ptr<House> HousePtr;
typedef boost::intrusive_ptr<Town> TownPtr;
typedef boost::intrusive_ptr<CreatureType> CreatureTypePtr;

typedef std::vector<ThingPtr> ThingList;
typedef std::vector<ThingTypePtr> ThingTypeList;
typedef std::vector<ItemTypePtr> ItemTypeList;
typedef std::vector<HousePtr> HouseList;
typedef std::vector<TownPtr> TownList;
typedef std::unordered_map<Position, TilePtr, PositionHasher> TileMap;

// net
class ProtocolLogin;
class ProtocolGame;

typedef boost::intrusive_ptr<ProtocolGame> ProtocolGamePtr;
typedef boost::intrusive_ptr<ProtocolLogin> ProtocolLoginPtr;

// ui
class UIItem;
class UICreature;
class UIMap;
class UIProgressRect;

typedef boost::intrusive_ptr<UIItem> UIItemPtr;
typedef boost::intrusive_ptr<UICreature> UICreaturePtr;
typedef boost::intrusive_ptr<UIMap> UIMapPtr;
typedef boost::intrusive_ptr<UIProgressRect> UIProgressRectPtr;

#endif
