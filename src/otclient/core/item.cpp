#include "item.h"
#include "tibiadat.h"
#include "tibiaspr.h"
#include <framework/graphics/graphics.h>
#include "thing.h"

Item::Item()
{
    m_type = Thing::TYPE_ITEM;
}

ThingAttributes *Item::getAttributes()
{
    return g_tibiaDat.getItemAttributes(m_id);
}

void Item::draw(int x, int y)
{
    ThingAttributes *itemAttributes = getAttributes();

    int xdiv = 0, ydiv = 0, zdiv = 0, anim = 0;

    if(itemAttributes->group == THING_GROUP_SPLASH || itemAttributes->group == THING_GROUP_FLUID || itemAttributes->stackable) {
        //cDivX = subType % itemAttributes->xdiv;
        //cDivY = subType / itemAttributes->xdiv;
    }
    else if(!itemAttributes->moveable) {
        xdiv = m_position.x % itemAttributes->xdiv;
        ydiv = m_position.y % itemAttributes->ydiv;
        zdiv = m_position.z % itemAttributes->zdiv;
    }

    for(int b = 0; b < itemAttributes->blendframes; b++)
        internalDraw(x, y, b, xdiv, ydiv, zdiv, anim);
}
