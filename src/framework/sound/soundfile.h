/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#ifndef SOUNDFILE_H
#define SOUNDFILE_H

#include "declarations.h"
#include <framework/core/filestream.h>

class SoundFile : public stdext::shared_object
{
public:
    SoundFile(const FileStreamPtr& fileStream);
    virtual ~SoundFile() { }
    static SoundFilePtr loadSoundFile(const std::string& filename);

    virtual int read(void *buffer, int bufferSize) { return -1; }
    virtual void reset() { }
    bool eof() { return m_file->eof(); }

    ALenum getSampleFormat();

    int getChannels() { return m_channels; }
    int getRate() { return m_rate; }
    int getBps() { return m_bps; }
    int getSize() { return m_size; }
    std::string getName() { return m_file ? m_file->name() : std::string(); }

protected:
    FileStreamPtr m_file;
    int m_channels;
    int m_rate;
    int m_bps;
    int m_size;
};

#endif
