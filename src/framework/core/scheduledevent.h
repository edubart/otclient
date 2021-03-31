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

#ifndef SCHEDULEDEVENT_H
#define SCHEDULEDEVENT_H

#include "event.h"
#include "clock.h"

// @bindclass
class ScheduledEvent : public Event
{
public:
    ScheduledEvent(const std::function<void()>& callback, int delay, int maxCycles);
    void execute();
    bool nextCycle();

    int ticks() { return m_ticks; }
    int remainingTicks() { return m_ticks - g_clock.millis(); }
    int delay() { return m_delay; }
    int cyclesExecuted() { return m_cyclesExecuted; }
    int maxCycles() { return m_maxCycles; }

    struct Compare {
        bool operator() (const ScheduledEventPtr &a, const ScheduledEventPtr &b) const {
            return b->ticks() < a->ticks();
        }
    };

private:
    ticks_t m_ticks;
    int m_delay;
    int m_maxCycles;
    int m_cyclesExecuted;
};

#endif
