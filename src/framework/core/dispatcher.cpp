#include "dispatcher.h"
#include "engine.h"

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

void Dispatcher::scheduleTask(const std::function<void()>& callback, int delay)
{
    m_scheduledTaskList.push(new ScheduledTask(g_engine.getCurrentFrameTicks() + delay, callback));
}

void Dispatcher::addTask(const std::function<void()>& callback, bool pushFront)
{
    if(pushFront)
        m_taskList.push_front(callback);
    else
        m_taskList.push_back(callback);
}
