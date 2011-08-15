#include "thing.h"
#include "spritemanager.h"
#include <framework/graphics/graphics.h>

void Thing::internalDraw(int x, int y, int blendframes, int xdiv, int ydiv, int zdiv, int anim)
{
    const ThingAttributes& attributes = getAttributes();

    for(int yi = 0; yi < attributes.height; yi++) {
        for(int xi = 0; xi < attributes.width; xi++) {
            int sprIndex = xi +
                    yi * attributes.width +
                    blendframes * attributes.width * attributes.height +
                    xdiv * attributes.width * attributes.height * attributes.blendframes +
                    ydiv * attributes.width * attributes.height * attributes.blendframes * attributes.xdiv +
                    zdiv * attributes.width * attributes.height * attributes.blendframes * attributes.xdiv * attributes.ydiv +
                    anim * attributes.width * attributes.height * attributes.blendframes * attributes.xdiv * attributes.ydiv * attributes.zdiv;

            int spriteId = attributes.sprites[sprIndex];
            if(!spriteId)
                continue;

            TexturePtr spriteTex = g_sprites.getSpriteTexture(spriteId);
            Rect drawRect((x - xi*32) - attributes.xOffset,
                          (y - yi*32) - attributes.yOffset,
                          32, 32);

            g_graphics.drawTexturedRect(drawRect, spriteTex);
        }
    }
}
