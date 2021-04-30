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

#ifndef FRAMEWORK_SOUND_DECLARATIONS_H
#define FRAMEWORK_SOUND_DECLARATIONS_H

#include <framework/global.h>

#define AL_LIBTYPE_STATIC

#if defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

class SoundManager;
class SoundSource;
class SoundBuffer;
class SoundFile;
class SoundChannel;
class StreamSoundSource;
class CombinedSoundSource;
class OggSoundFile;

typedef stdext::shared_object_ptr<SoundSource> SoundSourcePtr;
typedef stdext::shared_object_ptr<SoundFile> SoundFilePtr;
typedef stdext::shared_object_ptr<SoundBuffer> SoundBufferPtr;
typedef stdext::shared_object_ptr<SoundChannel> SoundChannelPtr;
typedef stdext::shared_object_ptr<StreamSoundSource> StreamSoundSourcePtr;
typedef stdext::shared_object_ptr<CombinedSoundSource> CombinedSoundSourcePtr;
typedef stdext::shared_object_ptr<OggSoundFile> OggSoundFilePtr;

#endif
