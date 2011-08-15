#include "item.h"
#include "datmanager.h"
#include "spritemanager.h"
#include <framework/graphics/graphics.h>
#include "thing.h"

Item::Item()
{
    m_type = Thing::TYPE_ITEM;
}

const ThingAttributes& Item::getAttributes()
{
    return g_dat.getItemAttributes(m_id);
}

void Item::draw(int x, int y)
{
    auto attributes = g_dat.getItemAttributes(m_id);

    int xdiv = 0, ydiv = 0, zdiv = 0, anim = 0;

    if(attributes.group == THING_GROUP_SPLASH || attributes.group == THING_GROUP_FLUID || attributes.stackable) {
        //cDivX = subType % itemAttributes->xdiv;
        //cDivY = subType / itemAttributes->xdiv;
    }
    else if(!attributes.moveable) {
        xdiv = m_position.x % attributes.xdiv;
        ydiv = m_position.y % attributes.ydiv;
        zdiv = m_position.z % attributes.zdiv;
    }

    for(int b = 0; b < attributes.blendframes; b++)
        internalDraw(x, y, b, xdiv, ydiv, zdiv, anim);
}
