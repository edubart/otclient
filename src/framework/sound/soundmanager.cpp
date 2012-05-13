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

#include "soundmanager.h"
#include "soundsource.h"
#include "soundbuffer.h"
#include "soundfile.h"
#include "streamsoundsource.h"
#include "combinedsoundsource.h"

#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>

SoundManager g_sounds;

void SoundManager::init()
{
    m_device = alcOpenDevice(NULL);
    if(!m_device) {
        logError("unable to open audio device");
        return;
    }

    m_context = alcCreateContext(m_device, NULL);
    if(!m_context) {
        logError("unable to create audio context: ", alcGetString(m_device, alcGetError(m_device)));
        return;
    }
    alcMakeContextCurrent(m_context);

    m_musicEnabled = true;
    m_soundEnabled = true;
}

void SoundManager::terminate()
{
    m_sources.clear();
    m_buffers.clear();
    m_musicSource = nullptr;
    m_currentMusic = "";

    m_musicEnabled = false;
    m_soundEnabled = false;

    alcMakeContextCurrent(nullptr);

    if(m_context) {
        alcDestroyContext(m_context);
        m_context = nullptr;
    }

    if(m_device) {
        alcCloseDevice(m_device);
        m_device = nullptr;
    }
}

void SoundManager::poll()
{
    static ticks_t lastUpdate = 0;
    ticks_t now = g_clock.ticks();

    if(now - lastUpdate < POLL_DELAY)
        return;

    lastUpdate = now;

    for(auto it = m_sources.begin(); it != m_sources.end();) {
        SoundSourcePtr source = *it;

        source->update();

        if(!source->isPlaying())
            it = m_sources.erase(it);
        else
            ++it;
    }

    if(m_musicSource) {
        m_musicSource->update();
        if(!m_musicSource->isPlaying())
            m_musicSource = nullptr;
    }

    if(m_context) {
        alcProcessContext(m_context);
    }
}

void SoundManager::preload(const std::string& filename)
{
    auto it = m_buffers.find(filename);
    if(it != m_buffers.end())
        return;

    SoundFilePtr soundFile = SoundFile::loadSoundFile(filename);

    // only keep small files
    if(soundFile->getSize() > MAX_CACHE_SIZE)
        return;

    SoundBufferPtr buffer = SoundBufferPtr(new SoundBuffer);
    if(buffer->fillBuffer(soundFile))
        m_buffers[filename] = buffer;
}

void SoundManager::enableSound(bool enable)
{
    if(!isAudioEnabled())
        return;
}

void SoundManager::play(const std::string& filename)
{
    if(!m_soundEnabled)
        return;

    SoundSourcePtr soundSource = createSoundSource(filename);
    if(!soundSource) {
        logError("unable to play '", filename, "'");
        return;
    }

    soundSource->setRelative(true);
    soundSource->play();

    m_sources.push_back(soundSource);
}

void SoundManager::enableMusic(bool enable)
{
    if(!isAudioEnabled())
        return;

    m_musicEnabled = enable;

    if(enable)
        playMusic(m_currentMusic, 3.0f);
    else
        m_musicSource = nullptr;
}

void SoundManager::playMusic(const std::string& filename, float fadetime)
{
    if(m_currentMusic == filename && m_musicSource)
        return;
    m_currentMusic = filename;

    if(!m_musicEnabled)
        return;

    if(filename.empty()) {
        m_musicSource = nullptr;
        return;
    }

    m_musicSource = createSoundSource(filename);
    if(!m_musicSource) {
        logError("unable to play '", filename, "'");
        return;
    }

    m_musicSource->setRelative(true);

    if(fadetime > 0) {
        m_musicSource->setGain(0);
        m_musicSource->setFading(StreamSoundSource::FadingOn, fadetime);
    }

    m_musicSource->play();
}

void SoundManager::stopMusic(float fadetime)
{
    if(m_musicSource) {
        if(fadetime > 0)
            m_musicSource->setFading(StreamSoundSource::FadingOff, 3.0f);
        else
            m_musicSource->stop();
    }
}

SoundSourcePtr SoundManager::createSoundSource(const std::string& filename)
{
    SoundSourcePtr source;

    auto it = m_buffers.find(filename);
    if(it != m_buffers.end()) {
        source = SoundSourcePtr(new SoundSource);
        source->setBuffer(it->second);
    } else {
        SoundFilePtr soundFile = SoundFile::loadSoundFile(filename);
        if(!soundFile)
            return nullptr;

        if(soundFile->getSize() <= MAX_CACHE_SIZE) {
            source = SoundSourcePtr(new SoundSource);
            SoundBufferPtr buffer = SoundBufferPtr(new SoundBuffer);
            buffer->fillBuffer(soundFile);
            source->setBuffer(buffer);
            m_buffers[filename] = buffer;
            logWarning("uncached sound '", filename, "' requested to be played");
        } else {
            StreamSoundSourcePtr streamSource(new StreamSoundSource);
            streamSource->setSoundFile(soundFile);
            source = streamSource;

#if defined __linux && !defined OPENGLES
            // due to OpenAL implementation bug, stereo buffers are always downmixed to mono on linux systems
            // this is hack to work around the issue
            // solution taken from http://opensource.creative.com/pipermail/openal/2007-April/010355.html
            if(soundFile->getSampleFormat() == AL_FORMAT_STEREO16) {
                CombinedSoundSourcePtr combinedSource(new CombinedSoundSource);

                streamSource->downMix(StreamSoundSource::DownMixLeft);
                streamSource->setRelative(true);
                streamSource->setPosition(Point(-128, 0));
                combinedSource->addSource(streamSource);

                streamSource = StreamSoundSourcePtr(new StreamSoundSource);
                streamSource->setSoundFile(SoundFile::loadSoundFile(filename));
                streamSource->downMix(StreamSoundSource::DownMixRight);
                streamSource->setRelative(true);
                streamSource->setPosition(Point(128,0));
                combinedSource->addSource(streamSource);

                source = combinedSource;
            }
#endif
        }
    }

    return source;
}
