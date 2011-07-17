#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <global.h>
#include <queue>

#include <boost/bind.hpp>
#include <boost/function.hpp>

struct ScheduledTask {
    ScheduledTask(const boost::function<void()>& _callback) : ticks(0), callback(_callback) { }
    ScheduledTask(int _ticks, const boost::function<void()>& _callback) : ticks(_ticks), callback(_callback)  { }
    bool operator<(const ScheduledTask& other) const { return ticks > other.ticks; }
    int ticks;
    boost::function<void()> callback;
};

struct lessScheduledTask : public std::binary_function<ScheduledTask*&, ScheduledTask*&, bool> {
    bool operator()(ScheduledTask*& t1,ScheduledTask*& t2) { return (*t1) < (*t2); }
};

class Dispatcher
{
public:
    Dispatcher() { }

    /// Cleanup scheduled events
    void cleanup();

    /// Execute scheduled events
    void poll();

    /// Add an event
    void addTask(const boost::function<void()>& callback, bool pushFront = false);

    /// Schedula an event
    void scheduleTask(const boost::function<void()>& callback, int delay);

private:
    std::list<boost::function<void()>> m_taskList;
    std::priority_queue<ScheduledTask*, std::vector<ScheduledTask*>, lessScheduledTask> m_scheduledTaskList;
};

extern Dispatcher g_dispatcher;

#endif // DISPATCHER_H
