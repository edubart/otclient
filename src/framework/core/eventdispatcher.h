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
