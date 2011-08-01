#ifndef PLAYER_H
#define PLAYER_H

#include "position.h"

class Player
{
public:

    void setPosition(Position position) { m_position = position; }
    Position getPosition() { return m_position; }

private:
    Position m_position;
};

#endif
