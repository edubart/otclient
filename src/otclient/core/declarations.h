#ifndef OTCLIENT_CORE_DECLARATIONS_H
#define OTCLIENT_CORE_DECLARATIONS_H

#include <otclient/global.h>
#include "const.h"

class Thing;
class Item;
class Tile;
class Creature;
class Player;
class Effect;

typedef std::shared_ptr<Thing> ThingPtr;
typedef std::shared_ptr<Item> ItemPtr;
typedef std::shared_ptr<Tile> TilePtr;
typedef std::shared_ptr<Creature> CreaturePtr;
typedef std::shared_ptr<Player> PlayerPtr;
typedef std::shared_ptr<Effect> EffectPtr;

#endif
