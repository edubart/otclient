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

#include "eventdispatcher.h"

#include <framework/core/clock.h>

EventDispatcher g_dispatcher;

void EventDispatcher::flush()
{
    poll();

    while(!m_scheduledEventList.empty())
        m_scheduledEventList.pop();
}

void EventDispatcher::poll()
{
    while(!m_scheduledEventList.empty()) {
        ScheduledEventPtr scheduledEvent = m_scheduledEventList.top();
        if(scheduledEvent->reamaningTicks() > 0)
            break;
        m_scheduledEventList.pop();
        scheduledEvent->execute();
    }

    while(!m_eventList.empty()) {
        EventPtr event = m_eventList.front();
        m_eventList.pop_front();
        event->execute();
    }
}

ScheduledEventPtr EventDispatcher::scheduleEvent(const SimpleCallback& callback, int delay)
{
    assert(delay >= 0);
    ScheduledEventPtr scheduledEvent(new ScheduledEvent(callback, delay));
    m_scheduledEventList.push(scheduledEvent);
    return scheduledEvent;
}

EventPtr EventDispatcher::addEvent(const SimpleCallback& callback, bool pushFront)
{
    EventPtr event(new Event(callback));
    if(pushFront)
        m_eventList.push_front(event);
    else
        m_eventList.push_back(event);
    return event;
}
