#include "viewportcontrol.h"

#include "map.h"

ViewportControl::ViewportControl(const Otc::Direction directionWalking)
{
    m_top = maxViewportY;
    m_right = maxViewportX;
    m_bottom = m_top;
    m_left = m_right;

    if (directionWalking == Otc::North) {
        m_top += 1;
        m_bottom += 1;
    }
    else if (directionWalking == Otc::East) {
        m_right += 1;
        m_left += 1;
    }
    else if (directionWalking == Otc::South) {
        m_top += 1;
        m_bottom += 1;
    }
    else if (directionWalking == Otc::West) {
        m_left += 1;
        m_right += 1;
    }
    else if (directionWalking == Otc::NorthEast) {
        m_left += 1;
        m_bottom += 1;

        m_top += 1;
        m_right += 1;
    }
    else if (directionWalking == Otc::SouthEast) {
        m_right += 1;
        m_bottom += 1;

        m_top += 1;
        m_left += 1;
    }
    else if (directionWalking == Otc::SouthWest) {
        m_top += 1;
        m_right += 1;

        m_left += 1;
        m_bottom += 1;
    }
    else if (directionWalking == Otc::NorthWest) {
        m_right += 1;
        m_bottom += 1;

        m_top += 1;
        m_left += 1;
    }
}

bool ViewportControl::isValid(const TilePtr& tile, const Position cameraPosition) const {
    const Position tilePos = tile->getPosition();
    const int diff = std::abs(cameraPosition.z - tilePos.z);

    int left = m_left;
    int right = m_right;
    int top = m_top;
    int down = m_bottom;

    if (tilePos.z != cameraPosition.z) {
        if (tilePos.z > cameraPosition.z) {
            left += diff;
            right -= diff;
            top += diff;
            down -= diff;
        }
        else {
            left -= diff;
            right += diff;
            top -= diff;
            down += diff;
        }
    }

    // Check for non-visible tiles on the screen and ignore them
    return !(
        (cameraPosition.x - tilePos.x >= left) || (tilePos.x - cameraPosition.x == right && tile->isSingleDimension() /*&& !tile->hasDisplacement()*/) ||
        (cameraPosition.y - tilePos.y >= top) || (tilePos.y - cameraPosition.y == down && tile->isSingleDimension() /*&& !tile->hasDisplacement()*/) ||
        (tilePos.x - cameraPosition.x > right) || (tilePos.y - cameraPosition.y > down)
        );
}