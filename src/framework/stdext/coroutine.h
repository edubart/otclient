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

#ifndef COROUTINE_H
#define COROUTINE_H

#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <cassert>

namespace stdext {

//
class coroutine
{
    struct codata : public stdext::shared_object {
        std::unique_ptr<std::thread> thread;
        std::condition_variable conditionVar;
        std::mutex mutex;
        std::function<void()> func;
    };

    stdext::shared_object_ptr<codata> m;

    void run() {
        assert(m->func);
        m->func();
        std::lock_guard<std::mutex> lock(m->mutex);
        m->func = nullptr;
        m->conditionVar.notify_all();
    }

public:
    coroutine() : m(stdext::make_shared_object<codata>()) { }
    coroutine(const coroutine& other) : m(other.m) { }

    coroutine& operator=(const coroutine& other) { m = other.m; return *this; }

    ~coroutine() {
        if(m.is_unique()) {
            m->conditionVar.notify_all();
            m->thread->join();
            assert(!m->func);
        }
    }

    void create(const std::function<void()>& func) {
        assert(!!func && !m->func);
        m->func = func;
    }

    void yield() const {
        std::unique_lock<std::mutex> lock(m->mutex);
        m->conditionVar.notify_all();
        m->conditionVar.wait(lock);
    }

    void resume() {
        if(!m->thread) {
            std::unique_lock<std::mutex> lock(m->mutex);
            m->thread.reset(new std::thread(std::bind(&coroutine::run, this)));
            m->conditionVar.wait(lock); // wait for the thread to start
        } else {
            std::unique_lock<std::mutex> lock(m->mutex);
            assert(m->func);
            m->conditionVar.notify_all();
            m->conditionVar.wait(lock);
        }
    }

    void join() {
        while(!is_finished())
            resume();
    }

    bool is_suspended() { return !!m->func; }
    bool is_finished() { return !m->func; }
};

};

#endif
