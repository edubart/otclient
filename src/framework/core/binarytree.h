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

#ifndef BINARYTREE_H
#define BINARYTREE_H

#include "declarations.h"
#include <framework/util/databuffer.h>

enum {
    BINARYTREE_ESCAPE_CHAR = 0xFD,
    BINARYTREE_NODE_START = 0xFE,
    BINARYTREE_NODE_END = 0xFF
};

class BinaryTree : public std::enable_shared_from_this<BinaryTree>
{
public:
    BinaryTree(const BinaryTreePtr& parent = nullptr) : m_type(0), m_pos(0), m_parent(parent) { }

    void unserialize(const FileStreamPtr& fin);

    void seek(uint pos);
    void skip(uint len) { seek(tell() + len); }
    uint tell() { return m_pos; }
    uint size() { return m_buffer.size(); }

    uint8 getU8();
    uint16 getU16();
    uint32 getU32();
    uint64 getU64();
    std::string getString();

    BinaryTreeVec getChildren() { return m_children; }
    BinaryTreePtr getParent() { return m_parent.lock(); }
    uint32 getType() { return m_type; }
    bool canRead() { return m_pos < m_buffer.size(); }

private:
    void setParent(const BinaryTreePtr& parent) { m_parent = parent; }
    uint m_type;
    uint m_pos;

    BinaryTreeVec m_children;
    BinaryTreeWeakPtr m_parent;
    DataBuffer<uint8> m_buffer;
};

#endif
