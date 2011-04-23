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

#include <prerequisites.h>

class Task {
public:
    inline Task(const SimpleCallback& _callback) : ticks(0), callback(_callback) { }
    inline Task(int _ticks, const SimpleCallback& _callback) : ticks(_ticks), callback(_callback)  { }
    inline bool operator<(const Task& other) const { return ticks > other.ticks; }
    int ticks;
    SimpleCallback callback;
};

class lessTask : public std::binary_function<Task*&, Task*&, bool> {
public:
    bool operator()(Task*& t1,Task*& t2) { return (*t1) < (*t2); }
};

class Dispatcher
{
public:
    Dispatcher() { }

    /// Execute scheduled events
    void poll();

    /// Add an event
    void addTask(const SimpleCallback& callback);

    /// Schedula an event
    void scheduleTask(const SimpleCallback& callback, int delay);

private:
    std::priority_queue<Task*, std::vector<Task*>, lessTask> m_taskList;
};

extern Dispatcher g_dispatcher;

#endif // DISPATCHER_H
