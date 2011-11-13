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

#include "outfit.h"

Color Outfit::internalGetColor(int color)
{
    if(color >= HSI_H_STEPS * HSI_SI_VALUES)
        color = 0;

    float loc1 = 0, loc2 = 0, loc3 = 0;
    if(color % HSI_H_STEPS != 0) {
        loc1 = color % HSI_H_STEPS * 1.0/18.0;
        loc2 = 1;
        loc3 = 1;

        switch(int(color / HSI_H_STEPS)) {
        case 0:
            loc2 = 0.25;
            loc3 = 1.00;
            break;
        case 1:
            loc2 = 0.25;
            loc3 = 0.75;
            break;
        case 2:
            loc2 = 0.50;
            loc3 = 0.75;
            break;
        case 3:
            loc2 = 0.667;
            loc3 = 0.75;
            break;
        case 4:
            loc2 = 1.00;
            loc3 = 1.00;
            break;
        case 5:
            loc2 = 1.00;
            loc3 = 0.75;
            break;
        case 6:
            loc2 = 1.00;
            loc3 = 0.50;
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

    if (loc2 == 0) {
        int loc7 = int(loc3 * 255);
        return Color(loc7, loc7, loc7);
    }

    float loc4 = 0, loc5 = 0, loc6 = 0;

    if(loc1 < 1.0/6.0) {
        loc4 = loc3;
        loc6 = loc3 * (1 - loc2);
        loc5 = loc6 + (loc3 - loc6) * 6 * loc1;
    }
    else if (loc1 < 2.0/6.0) {
        loc5 = loc3;
        loc6 = loc3 * (1 - loc2);
        loc4 = loc5 - (loc3 - loc6) * (6 * loc1 - 1);
    }
    else if(loc1 < 3.0/6.0) {
        loc5 = loc3;
        loc4 = loc3 * (1 - loc2);
        loc6 = loc4 + (loc3 - loc4) * (6 * loc1 - 2);
    }
    else if (loc1 < 4.0/6.0) {
        loc6 = loc3;
        loc4 = loc3 * (1 - loc2);
        loc5 = loc6 - (loc3 - loc4) * (6 * loc1 - 3);
    }
    else if (loc1 < 5.0/6.0) {
        loc6 = loc3;
        loc5 = loc3 * (1 - loc2);
        loc4 = loc5 + (loc3 - loc5) * (6 * loc1 - 4);
    }
    else {
        loc4 = loc3;
        loc5 = loc3 * (1 - loc2);
        loc6 = loc4 - (loc3 - loc5) * (6 * loc1 - 5);
    }
    return Color(int(loc4 * 255), int(loc5 * 255), int(loc6 * 255));
}
