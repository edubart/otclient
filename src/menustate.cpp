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
#include "framework/dispatcher.h"
#include "framework/ui/ui.h"
#include "framework/net/connections.h"

void MenuState::onEnter()
{
    m_background = g_textures.get("background.png");
    m_background->enableBilinearFilter();

    /*
    UIPanelPtr panel(new UIPanel);
    panel.setAnchorsLeft(g_gui.left());
    panel.setAnchorsBottom(g_gui.right());
    panel.setMarginBottom(10);
    panel.setMarginLeft(10);
    panel.setSize(Size(100, 100));

    UIButtonPtr button(new UIButton);
    button.setAnchorsHorizontalCenter(panel.horizontalCenter());
    button.setTop
    g_gui.addChild(panel);
    */
}

void MenuState::onLeave()
{

}

void MenuState::onClose()
{
    g_engine.stop();
}

void MenuState::onInputEvent(InputEvent* event)
{

}

void MenuState::render()
{
    // render background
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
}

void MenuState::update(int ticks, int elapsedTicks)
{

}
