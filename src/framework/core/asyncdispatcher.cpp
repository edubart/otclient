/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include "asyncdispatcher.h"

AsyncDispatcher g_asyncDispatcher;

void AsyncDispatcher::init()
{
    spawn_thread();
}

void AsyncDispatcher::terminate()
{
    stop();
    m_tasks.clear();
}

void AsyncDispatcher::spawn_thread()
{
    m_running = true;
    m_threads.emplace_back([this] () {
        exec_loop();
    });
}

void AsyncDispatcher::stop()
{
    m_mutex.lock();
    m_running = false;
    m_condition.notify_all();
    m_mutex.unlock();
    for(std::thread& thread : m_threads)
        thread.join();
    m_threads.clear();
};

void AsyncDispatcher::exec_loop() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while(true) {
        while(m_tasks.empty() && m_running)
            m_condition.wait(lock);

        if(!m_running)
            return;

        std::function<void()> task = m_tasks.front();
        m_tasks.pop_front();

        lock.unlock();
        task();
        lock.lock();
    }
}
