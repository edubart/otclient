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

#include "scheduledevent.h"

ScheduledEvent::ScheduledEvent(const std::function<void()>& callback, int delay, int maxCycles) : Event(callback)
{
    m_ticks = g_clock.millis() + delay;
    m_delay = delay;
    m_maxCycles = maxCycles;
    m_cyclesExecuted = 0;
}

void ScheduledEvent::execute()
{
    if(!m_canceled && m_callback && (m_maxCycles == 0 || m_cyclesExecuted < m_maxCycles)) {
        m_callback();
        m_executed = true;
        // callback may be used in the next cycle
    } else {
        // reset callback to free object refs
        m_callback = nullptr;
    }

    m_cyclesExecuted++;
}

bool ScheduledEvent::nextCycle()
{
    if(m_callback && !m_canceled && (m_maxCycles == 0 || m_cyclesExecuted < m_maxCycles)) {
        m_ticks += m_delay;
        return true;
    }

    // reset callback to free object refs
    m_callback = nullptr;
    return false;
}
