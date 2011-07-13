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


#include <global.h>
#include <core/dispatcher.h>
#include <core/engine.h>

Dispatcher g_dispatcher;

void Dispatcher::cleanup()
{
    while(!m_scheduledTaskList.empty()) {
        ScheduledTask *task = m_scheduledTaskList.top();
        m_scheduledTaskList.pop();
        delete task;
    }
}

void Dispatcher::poll()
{
    while(!m_taskList.empty()) {
        m_taskList.front()();
        m_taskList.pop_front();
    }

    while(!m_scheduledTaskList.empty()) {
        ScheduledTask *task = m_scheduledTaskList.top();
        if(g_engine.getCurrentFrameTicks() < task->ticks)
            break;
        m_scheduledTaskList.pop();
        task->callback();
        delete task;
    }
}

void Dispatcher::scheduleTask(const boost::function<void()>& callback, int delay)
{
    m_scheduledTaskList.push(new ScheduledTask(g_engine.getCurrentFrameTicks() + delay, callback));
}

void Dispatcher::addTask(const boost::function<void()>& callback, bool pushFront)
{
    if(pushFront)
        m_taskList.push_front(callback);
    else
        m_taskList.push_back(callback);
}
