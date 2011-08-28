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

#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include "declarations.h"

struct ScheduledEvent {
    ScheduledEvent(int ticks, const SimpleCallback& callback) : ticks(ticks), callback(callback) { }
    bool operator<(const ScheduledEvent& other) const { return ticks > other.ticks; }
    int ticks;
    SimpleCallback callback;
};

class EventDispatcher
{
public:
    /// Initialize dispatcher
    void init();

    /// Cleanup scheduled events
    void terminate();

    /// Execute scheduled events
    void poll();

    /// Add an event
    void addEvent(const SimpleCallback& callback, bool pushFront = false);

    /// Schedule an event
    void scheduleEvent(const SimpleCallback& callback, int delay);

private:
    std::list<SimpleCallback> m_eventList;
    std::priority_queue<ScheduledEvent> m_scheduledEventList;
};

extern EventDispatcher g_dispatcher;

#endif
