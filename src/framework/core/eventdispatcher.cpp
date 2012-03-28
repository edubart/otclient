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

EventDispatcher g_eventDispatcher;

void EventDispatcher::flush()
{
    while(!m_eventList.empty())
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

    // execute events list up to 3 times, this is needed because some events can schedule new events that would
    // change the UIWidgets layout, in this case we must execute these new events before we continue rendering,
    // we can't loop until the event list is empty, because this could lead to infinite loops
    // if someone write a module with bad code
    for(int i=0;i<3;++i) {
        m_pollEventsSize = m_eventList.size();
        if(m_pollEventsSize == 0)
            break;
        for(int i=0;i<m_pollEventsSize;++i) {
            EventPtr event = m_eventList.front();
            m_eventList.pop_front();
            event->execute();
        }
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
    // front pushing is a way to execute an event before others
    if(pushFront) {
        m_eventList.push_front(event);
        // the poll event list only grows when pushing into front
        m_pollEventsSize++;
    } else
        m_eventList.push_back(event);
    return event;
}
