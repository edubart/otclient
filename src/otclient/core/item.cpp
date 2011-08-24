#include "item.h"
#include "datmanager.h"
#include "spritemanager.h"
#include "thing.h"

Item::Item() : Thing(THING_ITEM)
{
    m_count = 0;
}

void Item::draw(int x, int y)
{
    const ThingAttributes& attributes = g_dat.getItemAttributes(m_id);

    int xdiv = 0, ydiv = 0, zdiv = 0, anim = 0;

    if(attributes.group == THING_GROUP_SPLASH || attributes.group == THING_GROUP_FLUID) {
        //xdiv = m_count % attributes.xdiv;
        //ydiv = m_count / attributes.ydiv;

    }
    else if(attributes.stackable) {
        if(m_count < 5) {
            xdiv = m_count-1;
            ydiv = 0;
        }
        else if(m_count < 10) {
            xdiv = 0;
            ydiv = 1;
        }
        else if(m_count < 25) {
            xdiv = 1;
            ydiv = 1;
        }
        else if(m_count < 50) {
            xdiv = 2;
            ydiv = 1;
        }
        else if(m_count <= 100) {
            xdiv = 3;
            ydiv = 1;
        }
    }
    else if(!attributes.moveable) {
        xdiv = m_position.x % attributes.xdiv;
        ydiv = m_position.y % attributes.ydiv;
        zdiv = m_position.z % attributes.zdiv;
    }

    for(int b = 0; b < attributes.blendframes; b++)
        internalDraw(x, y, b, xdiv, ydiv, zdiv, anim);
}

const ThingAttributes& Item::getAttributes()
{
    return g_dat.getItemAttributes(m_id);
}
