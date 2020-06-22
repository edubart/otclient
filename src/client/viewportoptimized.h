#ifndef VIEWPORT_OPTIMIZED_H
#define VIEWPORT_OPTIMIZED_H

#include "declarations.h"
#include "const.h"

class ViewportOptimized {
public:
    ViewportOptimized(const Otc::Direction directionWalking = Otc::InvalidDirection);

    bool isValid(const TilePtr& tile, const Position cameraPosition) const;

    int top() const { return m_top; }
    int right() const { return m_right; }
    int bottom() const { return m_bottom; }
    int left() const { return m_left; }

private:
    int m_top;
    int m_right;
    int m_bottom;
    int m_left;
};

#endif
