/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <global.h>
#include <queue>

#include <boost/bind.hpp>
#include <boost/function.hpp>

class ScheduledTask {
public:
    inline ScheduledTask(const boost::function<void()>& _callback) : ticks(0), callback(_callback) { }
    inline ScheduledTask(int _ticks, const boost::function<void()>& _callback) : ticks(_ticks), callback(_callback)  { }
    inline bool operator<(const ScheduledTask& other) const { return ticks > other.ticks; }
    int ticks;
    boost::function<void()> callback;
};

class lessScheduledTask : public std::binary_function<ScheduledTask*&, ScheduledTask*&, bool> {
public:
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
