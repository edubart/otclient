/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

using MapViewPtr = stdext::shared_object_ptr<MapView>;
using LightViewPtr = stdext::shared_object_ptr<LightView>;
using TilePtr = stdext::shared_object_ptr<Tile>;
using ThingPtr = stdext::shared_object_ptr<Thing>;
using ItemPtr = stdext::shared_object_ptr<Item>;
using ContainerPtr = stdext::shared_object_ptr<Container>;
using CreaturePtr = stdext::shared_object_ptr<Creature>;
using MonsterPtr = stdext::shared_object_ptr<Monster>;
using NpcPtr = stdext::shared_object_ptr<Npc>;
using PlayerPtr = stdext::shared_object_ptr<Player>;
using LocalPlayerPtr = stdext::shared_object_ptr<LocalPlayer>;
using EffectPtr = stdext::shared_object_ptr<Effect>;
using MissilePtr = stdext::shared_object_ptr<Missile>;
using AnimatedTextPtr = stdext::shared_object_ptr<AnimatedText>;
using StaticTextPtr = stdext::shared_object_ptr<StaticText>;
using AnimatorPtr = stdext::shared_object_ptr<Animator>;
using ThingTypePtr = stdext::shared_object_ptr<ThingType>;
using ItemTypePtr = stdext::shared_object_ptr<ItemType>;
using HousePtr = stdext::shared_object_ptr<House>;
using TownPtr = stdext::shared_object_ptr<Town>;
using CreatureTypePtr = stdext::shared_object_ptr<CreatureType>;
using SpawnPtr = stdext::shared_object_ptr<Spawn>;

using ThingList = std::vector<ThingPtr>;
using ThingTypeList = std::vector<ThingTypePtr>;
using ItemTypeList = std::vector<ItemTypePtr>;
using HouseList = std::list<HousePtr>;
using TownList = std::list<TownPtr>;
using TileList = std::list<TilePtr>;
using ItemVector = std::vector<ItemPtr>;
using TileMap = std::unordered_map<Position, TilePtr, Position::Hasher>;
using CreatureMap = std::unordered_map<Position, CreatureTypePtr, Position::Hasher>;
using SpawnMap = std::unordered_map<Position, SpawnPtr, Position::Hasher>;

// net
class ProtocolLogin;
class ProtocolGame;

using ProtocolGamePtr = stdext::shared_object_ptr<ProtocolGame>;
using ProtocolLoginPtr = stdext::shared_object_ptr<ProtocolLogin>;

// ui
class UIItem;
class UICreature;
class UIMap;
class UIMinimap;
class UIProgressRect;
class UIMapAnchorLayout;
class UIPositionAnchor;
class UISprite;

using UIItemPtr = stdext::shared_object_ptr<UIItem>;
using UICreaturePtr = stdext::shared_object_ptr<UICreature>;
using UISpritePtr = stdext::shared_object_ptr<UISprite>;
using UIMapPtr = stdext::shared_object_ptr<UIMap>;
using UIMinimapPtr = stdext::shared_object_ptr<UIMinimap>;
using UIProgressRectPtr = stdext::shared_object_ptr<UIProgressRect>;
using UIMapAnchorLayoutPtr = stdext::shared_object_ptr<UIMapAnchorLayout>;
using UIPositionAnchorPtr = stdext::shared_object_ptr<UIPositionAnchor>;

#endif
