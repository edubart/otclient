/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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
#include "thingtypemanager.h"

class Outfit
{
    enum {
        HSI_SI_VALUES = 7,
        HSI_H_STEPS = 19
    };

public:
    Outfit();

    static Color getColor(int color);

    void setId(int id) { m_id = id; }
    void setAuxId(int id) { m_auxId = id; }
    void setHead(int head) { m_head = head; m_headColor = getColor(head); }
    void setBody(int body) { m_body = body; m_bodyColor = getColor(body); }
    void setLegs(int legs) { m_legs = legs; m_legsColor = getColor(legs); }
    void setFeet(int feet) { m_feet = feet; m_feetColor = getColor(feet); }
    void setAddons(int addons) { m_addons = addons; }
    void setMount(int mount) { m_mount = mount; }
    void setCategory(ThingCategory category) { m_category = category; }

    void resetClothes();

    int getId() const { return m_id; }
    int getAuxId() const { return m_auxId; }
    int getHead() const { return m_head; }
    int getBody() const { return m_body; }
    int getLegs() const { return m_legs; }
    int getFeet() const { return m_feet; }
    int getAddons() const { return m_addons; }
    int getMount() const { return m_mount; }
    ThingCategory getCategory() const { return m_category; }

    Color getHeadColor() const { return m_headColor; }
    Color getBodyColor() const { return m_bodyColor; }
    Color getLegsColor() const { return m_legsColor; }
    Color getFeetColor() const { return m_feetColor; }

private:
    ThingCategory m_category;
    int m_id, m_auxId, m_head, m_body, m_legs, m_feet, m_addons, m_mount;
    Color m_headColor, m_bodyColor, m_legsColor, m_feetColor;
};

#endif
