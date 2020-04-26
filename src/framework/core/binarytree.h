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

#ifndef BINARYTREE_H
#define BINARYTREE_H

#include "declarations.h"
#include <framework/util/databuffer.h>

enum {
    BINARYTREE_ESCAPE_CHAR = 0xFD,
    BINARYTREE_NODE_START = 0xFE,
    BINARYTREE_NODE_END = 0xFF
};

class BinaryTree : public stdext::shared_object
{
public:
    BinaryTree(const FileStreamPtr& fin);
    ~BinaryTree();

    void seek(uint pos);
    void skip(uint len);
    uint tell() { return m_pos; }
    uint size() { unserialize(); return m_buffer.size(); }

    uint8 getU8();
    uint16 getU16();
    uint32 getU32();
    uint64 getU64();
    std::string getString(uint16 len = 0);
    Point getPoint();

    BinaryTreeVec getChildren();
    bool canRead() { unserialize(); return m_pos < m_buffer.size(); }

private:
    void unserialize();
    void skipNodes();

    FileStreamPtr m_fin;
    DataBuffer<uint8> m_buffer;
    uint m_pos;
    uint m_startPos;
};

class OutputBinaryTree : public stdext::shared_object
{
public:
    OutputBinaryTree(const FileStreamPtr& finish);

    void addU8(uint8 v);
    void addU16(uint16 v);
    void addU32(uint32 v);
    void addString(const std::string& v);
    void addPos(uint16 x, uint16 y, uint8 z);
    void addPoint(const Point& point);

    void startNode(uint8 node);
    void endNode();

private:
    FileStreamPtr m_fin;

protected:
    void write(const uint8* data, size_t size);
};

#endif

