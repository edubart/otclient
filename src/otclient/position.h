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

#ifndef POSITION_H
#define POSITION_H

#include "const.h"
#include <framework/stdext/types.h>
#include <framework/const.h>
#include <framework/util/point.h>

class Position
{
public:
    Position() : x(65535), y(65535), z(255) { }
    Position(uint16 x, uint16 y, uint8 z) : x(x), y(y), z(z) { }

    Position translatedToDirection(Otc::Direction direction) {
        Position pos = *this;
        switch(direction) {
        case Otc::North:
            pos.y--;
            break;
        case Otc::East:
            pos.x++;
            break;
        case Otc::South:
            pos.y++;
            break;
        case Otc::West:
            pos.x--;
            break;
        case Otc::NorthEast:
            pos.x++;
            pos.y--;
            break;
        case Otc::SouthEast:
            pos.x++;
            pos.y++;
            break;
        case Otc::SouthWest:
            pos.x--;
            pos.y++;
            break;
        case Otc::NorthWest:
            pos.x--;
            pos.y--;
            break;
        default:
            break;
        }
        return pos;
    }

    Position translatedToReverseDirection(Otc::Direction direction) {
        Position pos = *this;
        switch(direction) {
        case Otc::North:
            pos.y++;
            break;
        case Otc::East:
            pos.x--;
            break;
        case Otc::South:
            pos.y--;
            break;
        case Otc::West:
            pos.x++;
            break;
        case Otc::NorthEast:
            pos.x--;
            pos.y++;
            break;
        case Otc::SouthEast:
            pos.x--;
            pos.y--;
            break;
        case Otc::SouthWest:
            pos.x++;
            pos.y--;
            break;
        case Otc::NorthWest:
            pos.x++;
            pos.y++;
            break;
        default:
            break;
        }
        return pos;
    }

    Otc::Direction getDirectionFromPosition(const Position& position) const {
        int dx = position.x - x;
        int dy = position.y - y;

        if(dx == 0 && dy == 0)
            return Otc::InvalidDirection;
        else if(dx == 0) {
            if(dy < 0)
                return Otc::North;
            else if(dy > 0)
                return Otc::South;
        }
        else if(dy == 0) {
            if(dx < 0)
                return Otc::West;
            else if(dx > 0)
                return Otc::East;
        }
        else {
            float angle = std::atan2(dy * -1, dx) * RAD_TO_DEC;
            if(angle < 0)
                angle += 360;

            if(angle >= 360 - 22.5 || angle < 0 + 22.5)
                return Otc::East;
            else if(angle >= 45 - 22.5 && angle < 45 + 22.5)
                return Otc::NorthEast;
            else if(angle >= 90 - 22.5 && angle < 90 + 22.5)
                return Otc::North;
            else if(angle >= 135 - 22.5 && angle < 135 + 22.5)
                return Otc::NorthWest;
            else if(angle >= 180 - 22.5 && angle < 180 + 22.5)
                return Otc::West;
            else if(angle >= 225 - 22.5 && angle < 225 + 22.5)
                return Otc::SouthWest;
            else if(angle >= 270 - 22.5 && angle < 270 + 22.5)
                return Otc::South;
            else if(angle >= 315 - 22.5 && angle < 315 + 22.5)
                return Otc::SouthEast;
        }
        return Otc::InvalidDirection;
    }

    bool isValid() const { return !(x == 65535 && y == 65535 && z == 255); }
    float distance(const Position& pos) const { return sqrt(pow((pos.x - x), 2) + pow((pos.y - y), 2)); }
    int manhattanDistance(const Position& pos) const { return std::abs(pos.x - x) + std::abs(pos.y - y); }

    void translate(int dx, int dy, short dz = 0) { x += dx; y += dy; z += dz; }
    Position translated(int dx, int dy, short dz = 0) const { Position pos = *this; pos.x += dx; pos.y += dy; pos.z += dz; return pos; }

    Position operator+(const Position& other) const { return Position(x + other.x, y + other.y, z + other.z);   }
    Position& operator+=(const Position& other) { x+=other.x; y+=other.y; z +=other.z; return *this; }
    Position operator-(const Position& other) const { return Position(x - other.x, y - other.y, z - other.z);   }
    Position& operator-=(const Position& other) { x-=other.x; y-=other.y; z-=other.z; return *this; }
    // Point conversion(s)
    Position operator+(const Point& other) const { return Position(x + other.x, y + other.y, z); }
    Position operator&(int a) const { return Position(x & a, y & a, z); }
    Position& operator+=(const Point& other) { x += other.x; y += other.y; return *this; }

    Position& operator=(const Position& other) { x = other.x; y = other.y; z = other.z; return *this; }
    bool operator==(const Position& other) const { return other.x == x && other.y == y && other.z == z; }
    bool operator!=(const Position& other) const { return other.x!=x || other.y!=y || other.z!=z; }
    bool isInRange(const Position& pos, int xRange, int yRange) const { return std::abs(x-pos.x) <= xRange && std::abs(y-pos.y) <= yRange && z == pos.z; }
    bool isInRange(const Position& pos, int minXRange, int maxXRange, int minYRange, int maxYRange) const {
        return (pos.x >= x-minXRange && pos.x <= x+maxXRange && pos.y >= y-minYRange && pos.y <= y+maxYRange && pos.z == z);
    }

    bool up(int n = 1) {
        int nz = z-n;
        if(nz >= 0 && nz <= Otc::MAX_Z) {
            z = nz;
            return true;
        }
        return false;
    }

    bool down(int n = 1) {
        int nz = z+n;
        if(nz >= 0 && nz <= Otc::MAX_Z) {
            z = nz;
            return true;
        }
        return false;
    }

    bool coveredUp(int n = 1) {
        int nx = x+n, ny = y+n, nz = z-n;
        if(nx >= 0 && nx <= 65535 && ny >= 0 && ny <= 65535 && nz >= 0 && nz <= Otc::MAX_Z) {
            x = nx; y = ny; z = nz;
            return true;
        }
        return false;
    }

    bool coveredDown(int n = 1) {
        int nx = x-n, ny = y-n, nz = z+n;
        if(nx >= 0 && nx <= 65535 && ny >= 0 && ny <= 65535 && nz >= 0 && nz <= Otc::MAX_Z) {
            x = nx; y = ny; z = nz;
            return true;
        }
        return false;
    }

    uint16 x;
    uint16 y;
    uint8 z;
};

struct PositionHasher : std::unary_function<Position, std::size_t> {
    std::size_t operator()(const Position& pos) const {
        return (((pos.x * 8192) + pos.y) * 16) + pos.z;
    }
};

inline std::ostream& operator<<(std::ostream& out, const Position& pos)
{
    out << (int)pos.x << " " << (int)pos.y << " " << (int)pos.z;
    return out;
}

inline std::istream& operator>>(std::istream& in, Position& pos)
{
    int x, y, z;
    in >> x >> y >> z;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    return in;
}

#endif
