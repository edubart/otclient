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
#include "framebuffer.h"
#include "graphics.h"
#include "texturemanager.h"
#include "logger.h"
#include "engine.h"

FrameBuffer *fbo;
TexturePtr background;

MenuState::MenuState()
{

}

MenuState::~MenuState()
{

}

void MenuState::onEnter()
{
    background = g_textures.get("background.png");
    background->enableBilinearFilter();
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
    // draw background
    background->bind();

    int x = 0;
    int y = 0;
    int screenWidth = g_graphics.getWidth();
    int screenHeight = g_graphics.getHeight();
    int textureWidth = background->getWidth();
    int textureHeight = background->getHeight();

    int texCoordX;
    int texCoordY;
    int texCoordWidth;
    int texCoordHeight;

    int wantedWidth = 1240;
    int wantedHeight = 880;

    float originalRatio = (float)wantedWidth/wantedHeight;
    float screenRatio = (float)screenWidth/screenHeight;
    if(screenRatio >= originalRatio) {
        texCoordHeight = wantedHeight;
        texCoordWidth = std::min((int)(wantedHeight*screenRatio), textureWidth);
    } else {
        texCoordWidth = wantedWidth;
        texCoordHeight = std::min((int)(wantedWidth/screenRatio), textureHeight);
    }
    texCoordX = textureWidth - texCoordWidth;
    texCoordY = textureHeight - texCoordHeight;


    glBegin(GL_QUADS);
    glTexCoord2f((float)texCoordX/textureWidth,                 (float)texCoordY/textureHeight); glVertex2i(x,       y);
    glTexCoord2f((float)texCoordX/textureWidth,                 (float)(texCoordY+texCoordHeight)/textureHeight); glVertex2i(x,       y+screenHeight);
    glTexCoord2f((float)(texCoordX+texCoordWidth)/textureWidth, (float)(texCoordY+texCoordHeight)/textureHeight); glVertex2i(x+screenWidth, y+screenHeight);
    glTexCoord2f((float)(texCoordX+texCoordWidth)/textureWidth, (float)texCoordY/textureHeight); glVertex2i(x+screenWidth, y);
    glEnd();
}

void MenuState::update(int elapsedTicks)
{

}
