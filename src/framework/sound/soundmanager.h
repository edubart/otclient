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

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "declarations.h"

class SoundManager
{
    enum {
        MAX_CACHE_SIZE = 10000000
    };

public:
    void init();
    void terminate();

    void audioThread();

    void update();

    void preload(const std::string& filename);

    void enableSound(bool enable);
    void play(const std::string& filename);

    void enableMusic(bool enable);
    void playMusic(const std::string& filename, bool fade = false);
    void stopMusic(float fadetime = 0);

    bool isMusicEnabled() { return m_musicEnabled; }
    bool isSoundEnabled() { return m_soundEnabled; }
    bool isAudioEnabled() { return m_device && m_context; }
    std::string getCurrentMusic() { return m_currentMusic; }

private:
    SoundSourcePtr createSoundSource(const std::string& filename);
    ALuint loadFileIntoBuffer(const SoundFilePtr& soundFile);

    std::map<std::string, SoundBufferPtr> m_buffers;
    std::vector<SoundSourcePtr> m_sources;
    StreamSoundSourcePtr m_musicSource;
    ALCdevice *m_device;
    ALCcontext *m_context;
    std::thread m_thread;
    std::atomic<bool> m_run;
    std::recursive_mutex m_mutex;
    Boolean<false> m_musicEnabled;
    Boolean<false> m_soundEnabled;
    std::string m_currentMusic;
};

extern SoundManager g_sounds;

#endif
