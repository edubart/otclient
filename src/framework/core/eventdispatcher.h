/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include "declarations.h"
#include "clock.h"
#include <framework/luascript/luaobject.h>

class Event : public LuaObject
{
public:
    Event(const std::function<void()>& callback) : m_callback(callback), m_canceled(false), m_executed(false) { }
    virtual ~Event() {
        // assure that we lost callback refs
        assert(m_callback == nullptr);
    }

    virtual void execute() {
        if(!m_canceled && !m_executed && m_callback) {
            m_callback();
            m_executed = true;
        }

        // reset callback to free object refs
        m_callback = nullptr;
    }
    void cancel() {
        m_canceled = true;
        m_callback = nullptr;
    }

    bool isCanceled() { return m_canceled; }
    bool isExecuted() { return m_executed; }

protected:
    std::function<void()> m_callback;
    bool m_canceled;
    bool m_executed;
};

class ScheduledEvent : public Event
{
public:
    ScheduledEvent(const std::function<void()>& callback, int delay, int maxCycles) : Event(callback) {
        m_ticks = g_clock.millis() + delay;
        m_delay = delay;
        m_maxCycles = maxCycles;
        m_cyclesExecuted = 0;
    }

    void execute() {
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

    bool nextCycle() {
        if(m_callback && !m_canceled && (m_maxCycles == 0 || m_cyclesExecuted < m_maxCycles)) {
            m_ticks += m_delay;
            return true;
        }
        // reset callback to free object refs
        m_callback = nullptr;
        return false;
    }

    int ticks() { return m_ticks; }
    int reamaningTicks() { return m_ticks - g_clock.millis(); }
    int delay() { return m_delay; }
    int cyclesExecuted() { return m_cyclesExecuted; }
    int maxCycles() { return m_maxCycles; }

private:
    ticks_t m_ticks;
    int m_delay;
    int m_maxCycles;
    int m_cyclesExecuted;
};

struct lessScheduledEvent : std::binary_function<ScheduledEventPtr, ScheduledEventPtr&, bool> {
    bool operator()(const ScheduledEventPtr& a, const ScheduledEventPtr& b) {
        return  b->ticks() < a->ticks();
    }
};

class EventDispatcher
{
public:
    void flush();
    void poll();

    EventPtr addEvent(const std::function<void()>& callback, bool pushFront = false);
    ScheduledEventPtr scheduleEvent(const std::function<void()>& callback, int delay);
    ScheduledEventPtr cycleEvent(const std::function<void()>& callback, int delay);

private:
    std::list<EventPtr> m_eventList;
    int m_pollEventsSize;
    Boolean<false> m_disabled;
    std::priority_queue<ScheduledEventPtr, std::vector<ScheduledEventPtr>, lessScheduledEvent> m_scheduledEventList;
};

extern EventDispatcher g_eventDispatcher;

#endif
