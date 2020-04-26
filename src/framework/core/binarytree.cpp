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

#include "binarytree.h"
#include "filestream.h"

BinaryTree::BinaryTree(const FileStreamPtr& fin) :
    m_fin(fin), m_pos(0xFFFFFFFF)
{
    m_startPos = fin->tell();
}

BinaryTree::~BinaryTree()
{
}

void BinaryTree::skipNodes()
{
    while(true) {
        uint8 byte = m_fin->getU8();
        switch(byte) {
            case BINARYTREE_NODE_START: {
                skipNodes();
                break;
            }
            case BINARYTREE_NODE_END:
                return;
            case BINARYTREE_ESCAPE_CHAR:
                m_fin->getU8();
                break;
            default:
                break;
        }
    }
}

void BinaryTree::unserialize()
{
    if(m_pos != 0xFFFFFFFF)
        return;
    m_pos = 0;

    m_fin->seek(m_startPos);
    while(true) {
        uint8 byte = m_fin->getU8();
        switch(byte) {
            case BINARYTREE_NODE_START: {
                skipNodes();
                break;
            }
            case BINARYTREE_NODE_END:
                return;
            case BINARYTREE_ESCAPE_CHAR:
                m_buffer.add(m_fin->getU8());
                break;
            default:
                m_buffer.add(byte);
                break;
        }
    }
}

BinaryTreeVec BinaryTree::getChildren()
{
    BinaryTreeVec children;
    m_fin->seek(m_startPos);
    while(true) {
        uint8 byte = m_fin->getU8();
        switch(byte) {
            case BINARYTREE_NODE_START: {
                BinaryTreePtr node(new BinaryTree(m_fin));
                children.push_back(node);
                node->skipNodes();
                break;
            }
            case BINARYTREE_NODE_END:
                return children;
            case BINARYTREE_ESCAPE_CHAR:
                m_fin->getU8();
                break;
            default:
                break;
        }
    }
}

void BinaryTree::seek(uint pos)
{
    unserialize();
    if(pos > m_buffer.size())
        stdext::throw_exception("BinaryTree: seek failed");
    m_pos = pos;
}

void BinaryTree::skip(uint len)
{
    unserialize();
    seek(tell() + len);
}

uint8 BinaryTree::getU8()
{
    unserialize();
    if(m_pos+1 > m_buffer.size())
        stdext::throw_exception("BinaryTree: getU8 failed");
    uint8 v = m_buffer[m_pos];
    m_pos += 1;
    return v;
}

uint16 BinaryTree::getU16()
{
    unserialize();
    if(m_pos+2 > m_buffer.size())
        stdext::throw_exception("BinaryTree: getU16 failed");
    uint16 v = stdext::readULE16(&m_buffer[m_pos]);
    m_pos += 2;
    return v;
}

uint32 BinaryTree::getU32()
{
    unserialize();
    if(m_pos+4 > m_buffer.size())
        stdext::throw_exception("BinaryTree: getU32 failed");
    uint32 v = stdext::readULE32(&m_buffer[m_pos]);
    m_pos += 4;
    return v;
}

uint64 BinaryTree::getU64()
{
    unserialize();
    if(m_pos+8 > m_buffer.size())
        stdext::throw_exception("BinaryTree: getU64 failed");
    uint64 v = stdext::readULE64(&m_buffer[m_pos]);
    m_pos += 8;
    return v;
}

std::string BinaryTree::getString(uint16 len)
{
    unserialize();
    if(len == 0)
        len = getU16();

    if(m_pos+len > m_buffer.size())
        stdext::throw_exception("BinaryTree: getString failed: string length exceeded buffer size.");

    std::string ret((char *)&m_buffer[m_pos], len);
    m_pos += len;
    return ret;
}

Point BinaryTree::getPoint()
{
    Point ret;
    ret.x = getU8();
    ret.y = getU8();
    return ret;
}

OutputBinaryTree::OutputBinaryTree(const FileStreamPtr& fin)
    : m_fin(fin)
{
    startNode(0);
}

void OutputBinaryTree::addU8(uint8 v)
{
    write(&v, 1);
}

void OutputBinaryTree::addU16(uint16 v)
{
    uint8 data[2];
    stdext::writeULE16(data, v);
    write(data, 2);
}

void OutputBinaryTree::addU32(uint32 v)
{
    uint8 data[4];
    stdext::writeULE32(data, v);
    write(data, 4);
}

void OutputBinaryTree::addString(const std::string& v)
{
    if(v.size() > 0xFFFF)
        stdext::throw_exception("too long string");

    addU16(v.length());
    write((const uint8*)v.c_str(), v.length());
}

void OutputBinaryTree::addPos(uint16 x, uint16 y, uint8 z)
{
    addU16(x);
    addU16(y);
    addU8(z);
}

void OutputBinaryTree::addPoint(const Point& point)
{
    addU8(point.x);
    addU8(point.y);
}

void OutputBinaryTree::startNode(uint8 node)
{
    m_fin->addU8(BINARYTREE_NODE_START);
    write(&node, 1);
}

void OutputBinaryTree::endNode()
{
    m_fin->addU8(BINARYTREE_NODE_END);
}

void OutputBinaryTree::write(const uint8 *data, size_t size)
{
    for(size_t i=0;i<size;++i) {
        if(data[i]==BINARYTREE_NODE_START || data[i]==BINARYTREE_NODE_END||data[i]==BINARYTREE_ESCAPE_CHAR)
            m_fin->addU8(BINARYTREE_ESCAPE_CHAR);
        m_fin->addU8(data[i]);
    }
}

