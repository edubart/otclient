/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

#ifndef THREAD_H
#define THREAD_H

// hack to enable std::thread on mingw32 4.6
#if 1
//#if !defined(_GLIBCXX_HAS_GTHREADS) && defined(__GNUG__)
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/future.hpp>
#include <boost/thread/condition_variable.hpp>
namespace stdext {
    using boost::thread;
    using boost::future;
    using boost::future_status;
    using boost::promise;

    using boost::mutex;
    using boost::timed_mutex;
    using boost::recursive_mutex;
    using boost::recursive_timed_mutex;

    using boost::lock_guard;
    using boost::unique_lock;

    using boost::condition_variable;
    using boost::condition_variable_any;

    template<typename R>
    bool is_ready(stdext::future<R>& f)
    { return f.wait_for(boost::chrono::seconds(0)) == future_status::ready; }
}

#else

#include <thread>
#include <condition_variable>
#include <mutex>
#include <future>

namespace stdext {
    template<typename R>
    bool is_ready(std::future<R>& f)
    { return f.wait_for(chrono::seconds(0)) == future_status::ready; }
};
namespace stdext {
    using std::thread;
    using std::future;
    using std::future_status;
    using std::promise;

    using std::mutex;
    using std::timed_mutex;
    using std::recursive_mutex;
    using std::recursive_timed_mutex;

    using std::lock_guard;
    using std::unique_lock;

    using std::condition_variable;
    using std::condition_variable_any;

    template<typename R>
    bool is_ready(std::future<R>& f)
    { return f.wait_for(boost::chrono::seconds(0)) == future_status::ready; }
}

#endif

#endif
