#ifndef OTCLIENT_CORE_DECLARATIONS_H
#define OTCLIENT_CORE_DECLARATIONS_H

#include <otclient/global.h>
#include "const.h"

class Tile;
class Thing;
class Item;
class Creature;
class Effect;
class Player;
class LocalPlayer;

typedef std::shared_ptr<Tile> TilePtr;
typedef std::shared_ptr<Thing> ThingPtr;
typedef std::shared_ptr<Item> ItemPtr;
typedef std::shared_ptr<Creature> CreaturePtr;
typedef std::shared_ptr<Effect> EffectPtr;
typedef std::shared_ptr<Player> PlayerPtr;
typedef std::shared_ptr<LocalPlayer> LocalPlayerPtr;

#endif
