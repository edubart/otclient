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

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "declarations.h"

//@bindsingleton g_sounds
class SoundManager
{
    enum {
        MAX_CACHE_SIZE = 100000,
        POLL_DELAY = 100
    };

public:
    void init();
    void terminate();
    void poll();

    void preload(std::string filename);
    void enableSound(bool enable);
    void play(std::string filename);

    void enableMusic(bool enable);
    void playMusic(std::string filename, float fadetime);
    void stopMusic(float fadetime = 0);

    bool isMusicEnabled() { return m_musicEnabled; }
    bool isSoundEnabled() { return m_soundEnabled; }
    bool isAudioEnabled() { return m_device && m_context; }
    std::string getCurrentMusic() { return m_currentMusic; }

private:
    StreamSoundSourcePtr createStreamSoundSource(const std::string& filename);
    SoundSourcePtr createSoundSource(const std::string& filename);
    uint loadFileIntoBuffer(const SoundFilePtr& soundFile);

    std::unordered_map<std::string, SoundBufferPtr> m_buffers;
    std::vector<SoundSourcePtr> m_sources;
    SoundSourcePtr m_musicSource;
    ALCdevice *m_device;
    ALCcontext *m_context;
    stdext::boolean<false> m_musicEnabled;
    stdext::boolean<false> m_soundEnabled;
    std::string m_currentMusic;
};

extern SoundManager g_sounds;

#endif
