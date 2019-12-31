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

#include "soundchannel.h"
#include "streamsoundsource.h"
#include "soundmanager.h"

SoundSourcePtr SoundChannel::play(const std::string& filename, float fadetime, float gain)
{
    if(!g_sounds.isAudioEnabled() || !m_enabled)
        return nullptr;

    if(m_currentSource)
        m_currentSource->stop();

    m_currentSource = g_sounds.play(filename, fadetime, m_gain*gain);
    return m_currentSource;
}

void SoundChannel::stop(float fadetime)
{
    m_queue.clear();

    if(m_currentSource) {
        if(fadetime > 0)
            m_currentSource->setFading(StreamSoundSource::FadingOff, fadetime);
        else {
            m_currentSource->stop();
            m_currentSource = nullptr;
        }
    }
}

void SoundChannel::enqueue(const std::string& filename, float fadetime, float gain)
{
    if(gain == 0)
        gain = 1.0f;
    m_queue.push_back(QueueEntry{g_sounds.resolveSoundFile(filename), fadetime, gain});
    std::random_shuffle(m_queue.begin(), m_queue.end());
    //update();
}

void SoundChannel::update()
{
    if(m_currentSource && !m_currentSource->isPlaying())
        m_currentSource = nullptr;

    if(!m_currentSource && !m_queue.empty() && g_sounds.isAudioEnabled() && m_enabled) {
        QueueEntry entry = m_queue.front();
        m_queue.pop_front();
        m_queue.push_back(entry);
        play(entry.filename, entry.fadetime, entry.gain);
    }
}

void SoundChannel::setEnabled(bool enable)
{
    if(m_enabled == enable)
        return;

    if(enable) {
        m_enabled = true;
        update();
    } else {
        m_enabled = false;
        if(m_currentSource) {
            m_currentSource->stop();
            m_currentSource = nullptr;
        }
    }
}

void SoundChannel::setGain(float gain)
{
    if(m_currentSource)
        m_currentSource->setGain(gain);
    m_gain = gain;
}
