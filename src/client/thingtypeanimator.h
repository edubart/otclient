/*
 * Copyright (c) 2010-2014 OTClient <https://github.com/edubart/otclient>
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

#ifndef THINGTYPEANIMATOR_H
#define THINGTYPEANIMATOR_H

#include "declarations.h"

#include <framework/core/declarations.h>
#include <framework/otml/declarations.h>
#include <framework/graphics/texture.h>
#include <framework/graphics/coordsbuffer.h>
#include <framework/luaengine/luaobject.h>
#include <framework/net/server.h>

enum FrameDurations {
	FRAME_DURATION_ITEM = 500,
	FRAME_DURATION_OUTFIT = 300,
	FRAME_DURATION_EFFECT = 75
};

class ThingTypeAnimator : public LuaObject
{
public:
	ThingTypeAnimator(int id, ThingCategory category);

private:
    int m_lastPhase;
	int m_currentPhase;
	int m_startingPhase;
	int m_loopCount;

	int m_phases;
	int m_timer;

	int m_id;
	ThingCategory m_category;

};

#endif
