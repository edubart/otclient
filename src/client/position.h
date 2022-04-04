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

#ifndef POSITION_H
#define POSITION_H

#include <framework/const.h>
#include <framework/stdext/types.h>
#include <framework/util/point.h>
#include "config.h"
#include "const.h"

#include <array>
#include <istream>
#include <math.h>
#include <ostream>
#include <vector>
#include <string>

class Position
{
public:
    Position() = default;
    Position(int32_t x, int32_t y, uint8_t z) : x(x), y(y), z(z) {}
    Position(const Position& position) = default;

    Position translatedToDirection(Otc::Direction direction) const
    {
        Position pos = *this;
        switch (direction) {
            case Otc::North:
                --pos.y;
                break;
            case Otc::East:
                ++pos.x;
                break;
            case Otc::South:
                ++pos.y;
                break;
            case Otc::West:
                --pos.x;
                break;
            case Otc::NorthEast:
                ++pos.x;
                --pos.y;
                break;
            case Otc::SouthEast:
                ++pos.x;
                ++pos.y;
                break;
            case Otc::SouthWest:
                --pos.x;
                ++pos.y;
                break;
            case Otc::NorthWest:
                --pos.x;
                --pos.y;
                break;
            default:
                break;
        }
        return pos;
    }

    Position translatedToReverseDirection(Otc::Direction direction)  const
    {
        Position pos = *this;
        switch (direction) {
            case Otc::North:
                ++pos.y;
                break;
            case Otc::East:
                --pos.x;
                break;
            case Otc::South:
                --pos.y;
                break;
            case Otc::West:
                ++pos.x;
                break;
            case Otc::NorthEast:
                --pos.x;
                ++pos.y;
                break;
            case Otc::SouthEast:
                --pos.x;
                --pos.y;
                break;
            case Otc::SouthWest:
                ++pos.x;
                --pos.y;
                break;
            case Otc::NorthWest:
                ++pos.x;
                ++pos.y;
                break;
            default:
                break;
        }
        return pos;
    }

    std::vector<Position> translatedToDirections(const std::vector<Otc::Direction>& dirs) const
    {
        Position lastPos = *this;
        std::vector<Position> positions;

        if (!lastPos.isValid())
            return positions;

        positions.push_back(lastPos);

        for (const auto dir : dirs) {
            lastPos = lastPos.translatedToDirection(dir);
            if (!lastPos.isValid())
                break;
            positions.push_back(lastPos);
        }

        return positions;
    }

    static bool isDiagonal(const Otc::Direction dir) { return dir == Otc::NorthWest || dir == Otc::NorthEast || dir == Otc::SouthWest || dir == Otc::SouthEast; };

    static double getAngleFromPositions(const Position& fromPos, const Position& toPos)
    {
        // Returns angle in radians from 0 to 2Pi. -1 means positions are equal.
        const int32_t dx = toPos.x - fromPos.x;
        const int32_t dy = toPos.y - fromPos.y;
        if (dx == 0 && dy == 0)
            return -1;

        double angle = std::atan2<int32_t>(dy * -1, dx);
        if (angle < 0)
            angle += 2 * Fw::pi;

        return angle;
    }

    double getAngleFromPosition(const Position& position) const
    {
        return getAngleFromPositions(*this, position);
    }

    static Otc::Direction getDirectionFromPositions(const Position& fromPos, const Position& toPos)
    {
        const double angle = getAngleFromPositions(fromPos, toPos) * RAD_TO_DEC;

        if (angle >= 360 - 22.5 || angle < 0 + 22.5)
            return Otc::East;

        if (angle >= 45 - 22.5 && angle < 45 + 22.5)
            return Otc::NorthEast;

        if (angle >= 90 - 22.5 && angle < 90 + 22.5)
            return Otc::North;

        if (angle >= 135 - 22.5 && angle < 135 + 22.5)
            return Otc::NorthWest;

        if (angle >= 180 - 22.5 && angle < 180 + 22.5)
            return Otc::West;

        if (angle >= 225 - 22.5 && angle < 225 + 22.5)
            return Otc::SouthWest;

        if (angle >= 270 - 22.5 && angle < 270 + 22.5)
            return Otc::South;

        if (angle >= 315 - 22.5 && angle < 315 + 22.5)
            return Otc::SouthEast;

        return Otc::InvalidDirection;
    }

    Otc::Direction getDirectionFromPosition(const Position& position) const
    {
        return getDirectionFromPositions(*this, position);
    }

    bool isMapPosition() const { return ((x >= 0) && (y >= 0) && (x < UINT16_MAX) && (y < UINT16_MAX) && (z <= MAX_Z)); }
    bool isValid() const { return !(x == UINT16_MAX && y == UINT16_MAX && z == UINT8_MAX); }
    double distance(const Position& pos) const { return sqrt(pow<int32_t>(pos.x - x, 2) + pow<int32_t>(pos.y - y, 2)); }
    uint16 manhattanDistance(const Position& pos) const { return static_cast<uint16>(std::abs(pos.x - x) + std::abs(pos.y - y)); }

    void translate(int32_t dx, int32_t dy, int8_t dz = 0) { x += dx; y += dy; z += dz; }
    Position translated(int32_t dx, int32_t dy, int8_t dz = 0) const { Position pos = *this; pos.x += dx; pos.y += dy; pos.z += dz; return pos; }

    std::array<Position, 8> getPositionsAround() const
    {
        std::array<Position, 8> positions;
        int_fast8_t i = -1;
        for (int_fast32_t xi = -1; xi <= 1; ++xi) {
            for (int_fast32_t yi = -1; yi <= 1; ++yi) {
                const Position pos = translated(xi, yi);
                if (pos == *this)
                    continue;

                positions[++i] = pos;
            }
        }

        return positions;
    }

    Position operator+(const Position& other) const { return { x + other.x, y + other.y, static_cast<uint8_t>(z + other.z) }; }
    Position& operator+=(const Position& other) { x += other.x; y += other.y; z += other.z; return *this; }
    Position operator-(const Position& other) const { return { x - other.x, y - other.y, static_cast<uint8_t>(z - other.z) }; }
    Position& operator-=(const Position& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    // Point conversion(s)
    Position operator+(const Point& other) const { return { x + other.x, y + other.y, z }; }
    Position& operator+=(const Point& other) { x += other.x; y += other.y; return *this; }

    Position& operator=(const Position& other) = default;
    bool operator==(const Position& other) const { return other.x == x && other.y == y && other.z == z; }
    bool operator!=(const Position& other) const { return other.x != x || other.y != y || other.z != z; }
    bool isInRange(const Position& pos, uint16 xRange, uint16 yRange, const bool ignoreZ = false) const
    {
        auto _pos = pos;
        if (pos.z != z) {
            if (!ignoreZ) return false;
            _pos.coveredUp(pos.z - z);
        }

        return std::abs(x - _pos.x) <= xRange && std::abs(y - _pos.y) <= yRange && z == pos.z;
    }
    bool isInRange(const Position& pos, uint16 minXRange, uint16 maxXRange, uint16 minYRange, uint16 maxYRange, const bool ignoreZ = false) const
    {
        auto _pos = pos;
        if (pos.z != z) {
            if (!ignoreZ) return false;
            _pos.coveredUp(pos.z - z);
        }

        return _pos.x >= x - minXRange && _pos.x <= x + maxXRange && _pos.y >= y - minYRange && _pos.y <= y + maxYRange;
    }
    // operator less than for std::map
    bool operator<(const Position& other) const { return x < other.x || y < other.y || z < other.z; }

    bool up(int8_t n = 1)
    {
        const int8_t nz = z - n;
        if (nz >= 0 && nz <= MAX_Z) {
            z = nz;
            return true;
        }
        return false;
    }

    bool down(int8_t n = 1)
    {
        const int8_t nz = z + n;
        if (nz >= 0 && nz <= MAX_Z) {
            z = nz;
            return true;
        }

        return false;
    }

    bool coveredUp(int8_t n = 1)
    {
        const int32_t nx = x + n, ny = y + n;
        const int8_t nz = z - n;
        if (nx >= 0 && nx <= UINT16_MAX && ny >= 0 && ny <= UINT16_MAX && nz >= 0 && nz <= MAX_Z) {
            x = nx; y = ny; z = nz;
            return true;
        }

        return false;
    }

    bool coveredDown(int8_t n = 1)
    {
        const int32_t nx = x - n, ny = y - n;
        const int8_t nz = z + n;
        if (nx >= 0 && nx <= UINT16_MAX && ny >= 0 && ny <= UINT16_MAX && nz >= 0 && nz <= MAX_Z) {
            x = nx; y = ny; z = nz;
            return true;
        }

        return false;
    }

    std::string toString()
    {
        return std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z);
    }

    int32_t x{ UINT16_MAX };
    int32_t y{ UINT16_MAX };
    uint8_t z{ UINT8_MAX };

    // NOTE: This does not increase the size of the struct.
    struct Hasher
    {
        std::size_t operator() (const Position& pos) const
        {
            return (((pos.x * 8192) + pos.y) * 16) + pos.z;
        }
    };
};

inline std::ostream& operator<<(std::ostream& out, const Position& pos)
{
    out << static_cast<int>(pos.x) << " " << static_cast<int>(pos.y) << " " << static_cast<int>(pos.z);
    return out;
}

inline std::istream& operator>>(std::istream& in, Position& pos)
{
    int32_t x, y;
    uint8_t z;
    in >> x >> y >> z;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    return in;
}

#endif
