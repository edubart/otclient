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

#ifndef __COMPILER_H__
#define __COMPILER_H__

#if !(__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
#error "sorry, you need gcc 4.6 or greater to compile"
#endif

#if !defined(__GXX_EXPERIMENTAL_CXX0X__)
#error "sorry, you must enable C++0x to compile"
#endif

// hack to enable std::thread on mingw32 4.6
/*
#if !defined(_GLIBCXX_HAS_GTHREADS) && defined(__GNUG__)
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition_variable.hpp>
namespace std {
  using boost::thread;

  using boost::mutex;
  using boost::timed_mutex;
  using boost::recursive_mutex;
  using boost::recursive_timed_mutex;

  using boost::lock_guard;
  using boost::unique_lock;

  using boost::condition_variable;
  using boost::condition_variable_any;
}
#endif
*/

#endif
