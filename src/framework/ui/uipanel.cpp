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


#include "uipanel.h"


UIPanel::UIPanel()
{
    m_boderedImage = BorderedImagePtr(new BorderedImage("ui.png"));
    m_boderedImage->setTexCoords(Rect(0,214,5,32),
                                 Rect(6,214,5,32),
                                 Rect(43,214,32,5),
                                 Rect(43,220,32,5),
                                 Rect(43,225,5,5),
                                 Rect(49,225,5,5),
                                 Rect(43,230,5,5),
                                 Rect(48,231,5,5),
                                 Rect(11,214,32,32));
}

void UIPanel::render()
{
    m_boderedImage->draw(m_rect);
    UIContainer::render();
}
