#include "eventdispatcher.h"

#include <framework/platform/platform.h>

EventDispatcher g_dispatcher;

void EventDispatcher::init()
{
    // nothing to do
}

void EventDispatcher::terminate()
{
    // clean scheduled events
    while(!m_scheduledEventList.empty())
        m_scheduledEventList.pop();
}

void EventDispatcher::poll()
{
    while(!m_eventList.empty()) {
        m_eventList.front()();
        m_eventList.pop_front();
    }

    while(!m_scheduledEventList.empty()) {
        if(g_platform.getTicks() < m_scheduledEventList.top().ticks)
            break;
        SimpleCallback callback = std::move(m_scheduledEventList.top().callback);
        m_scheduledEventList.pop();
        callback();
    }
}

void EventDispatcher::scheduleEvent(const SimpleCallback& callback, int delay)
{
    m_scheduledEventList.push(ScheduledEvent(g_platform.getTicks() + delay, callback));
}

void EventDispatcher::addEvent(const SimpleCallback& callback, bool pushFront)
{
    if(pushFront)
        m_eventList.push_front(callback);
    else
        m_eventList.push_back(callback);
}
