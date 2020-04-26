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

#include "soundfile.h"
#include "oggsoundfile.h"
#include <framework/core/resourcemanager.h>

SoundFile::SoundFile(const FileStreamPtr& fileStream)
{
    m_file = fileStream;
}

SoundFilePtr SoundFile::loadSoundFile(const std::string& filename)
{
    stdext::timer t;
    FileStreamPtr file = g_resources.openFile(filename);
    if(!file)
        stdext::throw_exception(stdext::format("unable to open %s", filename));

    // cache file buffer to avoid lags from hard drive
    file->cache();

    char magic[4];
    file->read(magic, 4);
    file->seek(0);

    SoundFilePtr soundFile;
    if(strncmp(magic, "OggS", 4) == 0) {
        OggSoundFilePtr oggSoundFile = OggSoundFilePtr(new OggSoundFile(file));
        if(oggSoundFile->prepareOgg())
            soundFile = oggSoundFile;
    } else
        stdext::throw_exception(stdext::format("unknown sound file format %s", filename));

    return soundFile;
}

ALenum SoundFile::getSampleFormat()
{
    if(m_channels == 2) {
        if(m_bps == 16)
            return AL_FORMAT_STEREO16;
        else if(m_bps == 8)
            return AL_FORMAT_STEREO8;
    } else if(m_channels == 1) {
        if(m_bps == 16)
            return AL_FORMAT_MONO16;
        else if(m_bps == 8)
            return AL_FORMAT_MONO8;
    }
    return AL_UNDETERMINED;
}
