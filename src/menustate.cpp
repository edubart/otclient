/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#include "menustate.h"
#include "framework/framebuffer.h"
#include "framework/graphics.h"
#include "framework/textures.h"
#include "framework/logger.h"
#include "framework/engine.h"
#include "framework/rect.h"
#include "framework/fonts.h"
#include "framework/input.h"

TexturePtr background;

void MenuState::onEnter()
{
    m_background = g_textures.get("background.png");
    m_background->enableBilinearFilter();
}

void MenuState::onLeave()
{

}

void MenuState::onClose()
{
    g_engine.stop();
}

int x, y;
void MenuState::onInputEvent(InputEvent* event)
{
    static bool moving = false;
    static int lastX;
    static int lastY;
    if(event->type == EV_MOUSE_LDOWN) {
        moving = true;
        lastX = event->mouse.x;
        lastY = event->mouse.y;
    } else if(event->type == EV_MOUSE_LUP) {
        moving = false;
    } else if(event->type == EV_MOUSE_MOVE) {
        if(moving) {
            x = lastX - event->mouse.x;
            y = lastY - event->mouse.y;
        }
    }
}

void MenuState::render()
{
    static Size minTexCoordsSize(1240, 880);
    const Size& screenSize = g_graphics.getScreenSize();
    const Size& texSize = m_background->getSize();

    Size texCoordsSize = screenSize;
    if(texCoordsSize < minTexCoordsSize)
        texCoordsSize.scale(minTexCoordsSize, KEEP_ASPECT_RATIO_BY_EXPANDING);
    texCoordsSize = texCoordsSize.boundedTo(texSize);

    Rect texCoords(0, 0, texCoordsSize);
    texCoords.moveBottomRight(texSize.toPoint());
    g_graphics.drawTexturedRect(Rect(0, 0, screenSize), m_background.get(), texCoords);

    static const char *text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
                              "Nulla pulvinar odio ac arcu tempor consequat.\n"
                              "Praesent at enim sapien, at vestibulum ligula.\n"
                              "Aliquam eleifend ante eu sapien vehicula consectetur.\n"
                              "Nunc id ligula ligula, eget vestibulum magna.\n"
                              "In mattis nisi non nisl semper ultricies.\n";
    Size textSize = g_defaultFont->calculateTextSize(text);
    g_defaultFont->renderText(Rect(100, 100, textSize.width() - 120, textSize.height() - 15), text, Point(x,y), true);
}

void MenuState::update(int ticks, int elapsedTicks)
{

}
