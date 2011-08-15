#ifndef PLAYER_H
#define PLAYER_H

#include "creature.h"

class Player : public Creature
{
public:
    Player() { }
    virtual ~Player() { }

    PlayerPtr asPlayer() { return std::static_pointer_cast<Player>(shared_from_this()); }
};

#endif
