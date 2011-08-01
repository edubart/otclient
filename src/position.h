#ifndef POSITION_H
#define POSITION_H

class Position
{
public:
    Position(uint16 x = 0, uint16 y = 0, uint8 z = 0) {
        m_x = x;
        m_y = y;
        m_z = z;
    }

    void setX(uint16 x) { m_x = x; }
    void setY(uint16 y) { m_y = y; }
    void setZ(uint8  z) { m_z = z; }

    uint16 getX() { return m_x; }
    uint16 getY() { return m_y; }
    uint8  getZ() { return m_z; }

private:
    uint16 m_x, m_y;
    uint8 m_z;
};

#endif
