#ifndef POSITION_H
#define POSITION_H

#include <global.h>

enum Direction
{
    DIRECTION_NORTH,
    DIRECTION_EAST,
    DIRECTION_SOUTH,
    DIRECTION_WEST
};

class Position
{
public:
    Position(uint16 x = 0, uint16 y = 0, uint8 z = 0) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    bool operator==(const Position& other) const { return other.x == x && other.y == y && other.z == z; }
    const Position operator+(const Position& other) const { return Position(other.x + x, other.y + y, other.z + z); }
    const Position operator-(const Position& other) const { return Position(other.x - x, other.y - y, other.z - z); }

    uint16 x, y;
    uint8 z;
};

struct PositionHash : public std::unary_function<Position, size_t>
{
    size_t operator()(const Position& pos) const {
        return ((((pos.x * 65536) + pos.y) * 15) + pos.z) % (1 * 1000000);
    }
};

#endif
