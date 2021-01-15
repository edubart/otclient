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

 // Define 1, to draw all ground first. (Ground first => remainder after)
#define DRAW_ALL_GROUND_FIRST 0

 // Define 1, to draw creature information after light.
#define DRAW_CREATURE_INFORMATION_AFTER_LIGHT 0

 // Define 1 so that a last creature that entered SQM is rendered behind the previous one.
#define RENDER_CREATURE_BEHIND 0

// Define 1 to force the rendering of animated things without waiting for the minimum mandatory interval.
#define FORCE_ANIMATED_RENDERING 0

// #Define 1 to increase "flush time" when there are many rendering requests,
// thus avoiding CPU consumption, however there will be a delay in rendering.
#define FLUSH_CONTROL_FOR_RENDERING 1