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

#ifndef SHOUNDCHANNEL_H
#define SHOUNDCHANNEL_H

#include "soundsource.h"

// @bindclass
class SoundChannel : public LuaObject
{
public:
    SoundChannel(int id) : m_id(id), m_gain(1) { }

    SoundSourcePtr play(const std::string& filename, float fadetime = 0, float gain = 1.0f);
    void stop(float fadetime = 0);
    void enqueue(const std::string& filename, float fadetime = 0, float gain = 1.0f);
    void enable() { setEnabled(true); }
    void disable() { setEnabled(false); }

    void setGain(float gain);
    float getGain() { return m_gain; }

    void setEnabled(bool enable);
    bool isEnabled() { return m_enabled; }

    int getId() { return m_id; }

protected:
    void update();
    friend class SoundManager;

private:
    struct QueueEntry {
        std::string filename;
        float fadetime;
        float gain;
    };
    std::deque<QueueEntry> m_queue;
    SoundSourcePtr m_currentSource;
    stdext::boolean<true> m_enabled;
    int m_id;
    float m_gain;
};

#endif
