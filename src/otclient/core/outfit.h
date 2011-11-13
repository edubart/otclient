/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#ifndef OUTFIT_H
#define OUTFIT_H

#include <framework/util/color.h>

class Outfit
{
    enum {
        HSI_SI_VALUES = 7,
        HSI_H_STEPS = 19
    };

public:
    void setType(int type) { m_type = type; }
    void setHead(int head) { m_head = internalGetColor(head); }
    void setBody(int body) { m_body = internalGetColor(body); }
    void setLegs(int legs) { m_legs = internalGetColor(legs); }
    void setFeet(int feet) { m_feet = internalGetColor(feet); }
    void setAddons(int addons) { m_addons = addons; }

    int getType() { return m_type; }
    Color getHead() { return m_head; }
    Color getBody() { return m_body; }
    Color getLegs() { return m_legs; }
    Color getFeet() { return m_feet; }
    int getAddons() { return m_addons; }

private:
    Color internalGetColor(int color);

    int m_type, m_addons;
    Color m_head, m_body, m_legs, m_feet;
};

#endif
