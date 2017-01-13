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

#include "eventdispatcher.h"

#include <framework/core/clock.h>
#include "timer.h"

EventDispatcher g_dispatcher;

void EventDispatcher::shutdown()
{
    while(!m_eventList.empty())
        poll();

    while(!m_scheduledEventList.empty()) {
        ScheduledEventPtr scheduledEvent = m_scheduledEventList.top();
        scheduledEvent->cancel();
        m_scheduledEventList.pop();
    }
    m_disabled = true;
}

void EventDispatcher::poll()
{
    int loops = 0;
    for(int count = 0, max = m_scheduledEventList.size(); count < max && !m_scheduledEventList.empty(); ++count) {
        ScheduledEventPtr scheduledEvent = m_scheduledEventList.top();
        if(scheduledEvent->remainingTicks() > 0)
            break;
        m_scheduledEventList.pop();
        scheduledEvent->execute();

        if(scheduledEvent->nextCycle())
            m_scheduledEventList.push(scheduledEvent);
    }

    // execute events list until all events are out, this is needed because some events can schedule new events that would
    // change the UIWidgets layout, in this case we must execute these new events before we continue rendering,
    m_pollEventsSize = m_eventList.size();
    loops = 0;
    while(m_pollEventsSize > 0) {
        if(loops > 50) {
            static Timer reportTimer;
            if(reportTimer.running() && reportTimer.ticksElapsed() > 100) {
                g_logger.error("ATTENTION the event list is not getting empty, this could be caused by some bad code");
                reportTimer.restart();
            }
            break;
        }

        for(int i=0;i<m_pollEventsSize;++i) {
            EventPtr event = m_eventList.front();
            m_eventList.pop_front();
            event->execute();
        }
        m_pollEventsSize = m_eventList.size();
        
        loops++;
    }
}

ScheduledEventPtr EventDispatcher::scheduleEvent(const std::function<void()>& callback, int delay)
{
    if(m_disabled)
        return ScheduledEventPtr(new ScheduledEvent(nullptr, delay, 1));

    assert(delay >= 0);
    ScheduledEventPtr scheduledEvent(new ScheduledEvent(callback, delay, 1));
    m_scheduledEventList.push(scheduledEvent);
    return scheduledEvent;
}

ScheduledEventPtr EventDispatcher::cycleEvent(const std::function<void()>& callback, int delay)
{
    if(m_disabled)
        return ScheduledEventPtr(new ScheduledEvent(nullptr, delay, 0));

    assert(delay > 0);
    ScheduledEventPtr scheduledEvent(new ScheduledEvent(callback, delay, 0));
    m_scheduledEventList.push(scheduledEvent);
    return scheduledEvent;
}

EventPtr EventDispatcher::addEvent(const std::function<void()>& callback, bool pushFront)
{
    if(m_disabled)
        return EventPtr(new Event(nullptr));

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

