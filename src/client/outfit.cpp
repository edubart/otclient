/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include "outfit.h"

Outfit::Outfit()
{
    m_category = ThingCategoryCreature;
    m_id = 128;
    m_auxId = 0;
    resetClothes();
}

Color Outfit::getColor(int color)
{
    if(color >= HSI_H_STEPS * HSI_SI_VALUES)
        color = 0;

    float loc1 = 0, loc2 = 0, loc3 = 0;
    if(color % HSI_H_STEPS != 0) {
        loc1 = color % HSI_H_STEPS * 1.f/18.f;
        loc2 = 1;
        loc3 = 1;

        switch(int(color / HSI_H_STEPS)) {
        case 0:
            loc2 = 0.25f;
            loc3 = 1.f;
            break;
        case 1:
            loc2 = 0.25f;
            loc3 = 0.75f;
            break;
        case 2:
            loc2 = 0.5f;
            loc3 = 0.75f;
            break;
        case 3:
            loc2 = 0.667f;
            loc3 = 0.75f;
            break;
        case 4:
            loc2 = 1.f;
            loc3 = 1.f;
            break;
        case 5:
            loc2 = 1.f;
            loc3 = 0.75f;
            break;
        case 6:
            loc2 = 1.f;
            loc3 = 0.5f;
            break;
        }
    }
    else {
        loc1 = 0;
        loc2 = 0;
        loc3 =  1 - (float)color / HSI_H_STEPS / (float)HSI_SI_VALUES;
    }

    if(loc3 == 0)
        return Color(0, 0, 0);

    if(loc2 == 0) {
        int loc7 = int(loc3 * 255);
        return Color(loc7, loc7, loc7);
    }

    float red = 0, green = 0, blue = 0;

    if(loc1 < 1.f/6.f) {
        red = loc3;
        blue = loc3 * (1 - loc2);
        green = blue + (loc3 - blue) * 6 * loc1;
    }
    else if(loc1 < 2.f/6.f) {
        green = loc3;
        blue = loc3 * (1 - loc2);
        red = green - (loc3 - blue) * (6 * loc1 - 1);
    }
    else if(loc1 < 3.f/6.f) {
        green = loc3;
        red = loc3 * (1 - loc2);
        blue = red + (loc3 - red) * (6 * loc1 - 2);
    }
    else if(loc1 < 4.f/6.f) {
        blue = loc3;
        red = loc3 * (1 - loc2);
        green = blue - (loc3 - red) * (6 * loc1 - 3);
    }
    else if(loc1 < 5.f/6.f) {
        blue = loc3;
        green = loc3 * (1 - loc2);
        red = green + (loc3 - green) * (6 * loc1 - 4);
    }
    else {
        red = loc3;
        green = loc3 * (1 - loc2);
        blue = red - (loc3 - green) * (6 * loc1 - 5);
    }
    return Color(int(red * 255), int(green * 255), int(blue * 255));
}

void Outfit::resetClothes()
{
    setHead(0);
    setBody(0);
    setLegs(0);
    setFeet(0);
    setMount(0);
}
