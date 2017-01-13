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


#ifndef HARDWAREBUFFER_H
#define HARDWAREBUFFER_H

#include "declarations.h"

class HardwareBuffer
{
public:
    enum Type {
        VertexBuffer = GL_ARRAY_BUFFER,
        IndexBuffer  = GL_ELEMENT_ARRAY_BUFFER
    };

    enum UsagePattern {
        StreamDraw          = GL_STREAM_DRAW,
        StaticDraw          = GL_STATIC_DRAW,
        DynamicDraw         = GL_DYNAMIC_DRAW
    };

    HardwareBuffer(Type type);
    ~HardwareBuffer();

    void bind() { glBindBuffer(m_type, m_id); }
    static void unbind(Type type) { glBindBuffer(type, 0); }
    void write(void *data, int count, UsagePattern usage) { glBufferData(m_type, count, data, usage); }

private:
    Type m_type;
    uint m_id;
};

#endif
