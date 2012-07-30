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
    uint16 v = stdext::readLE16(&m_buffer[m_pos]);
    m_pos += 2;
    return v;
}

uint32 BinaryTree::getU32()
{
    unserialize();
    if(m_pos+4 > m_buffer.size())
        stdext::throw_exception("BinaryTree: getU32 failed");
    uint32 v = stdext::readLE32(&m_buffer[m_pos]);
    m_pos += 4;
    return v;
}

uint64 BinaryTree::getU64()
{
    unserialize();
    if(m_pos+8 > m_buffer.size())
        stdext::throw_exception("BinaryTree: getU64 failed");
    uint64 v = stdext::readLE64(&m_buffer[m_pos]);
    m_pos += 8;
    return v;
}

std::string BinaryTree::getString()
{
    unserialize();
    uint16 len = getU16();
    if(m_pos+len > m_buffer.size())
        stdext::throw_exception("BinaryTree: getString failed: string length exceeded buffer size.");

    std::string ret((char *)&m_buffer[m_pos], len);
    m_pos += len;
    return ret;
}

std::string BinaryTree::getString(uint16 len)
{
    unserialize();
    if(m_pos+len > m_buffer.size())
        stdext::throw_exception("BinaryTree: getString failed: string length exceeded buffer size.");

    std::string ret((char *)&m_buffer[m_pos], len);
    m_pos += len;
    return ret;
}

Position BinaryTree::getPosition()
{
    Position ret;
    ret.x = getU16();
    ret.y = getU16();
    ret.z = getU8();
    return ret;
}

Point BinaryTree::getPoint()
{
    Point ret;
    ret.x = getU8();
    ret.y = getU8();
    return ret;
}

BinaryTreePtr BinaryTree::makeChild(uint8 type)
{
    BinaryTreePtr child(new BinaryTree(m_fin));
    child->setType(type);
    //children.append(child);
    return child;
}

void BinaryTree::setType(uint8 type)
{
    writeU8(0xFE);
    writeU8(type);
}

void BinaryTree::writeU8(uint8 u8)
{
    m_buffer.add(u8);
}

void BinaryTree::writeU16(uint16 u16)
{
    stdext::writeLE16(m_buffer.data(), u16);
}

void BinaryTree::writeU32(uint32 u32)
{
    stdext::writeLE32(m_buffer.data(), u32);
}

void BinaryTree::writeString(const std::string& s)
{
    size_t len = s.length();
    writeU16(len);
    m_buffer.grow(m_pos + len);
    memcpy(&m_buffer[m_pos], s.c_str(), len);
    m_pos += len;
}

void BinaryTree::writePos(const Position& p)
{
    if(!p.isValid())
      stdext::throw_exception("invalid position passed  to BinaryTree::writePos");

    writeU16(p.x);
    writeU16(p.y);
    writeU8(p.z);
}

void BinaryTree::writePoint(const Point& p)
{
    if(p.isNull())
      stdext::throw_exception("invalid point passed to BinaryTree::writePoint");

    writeU8(p.x);
    writeU8(p.y);
}

void BinaryTree::writeToFile()
{
    if(!m_fin)
        stdext::throw_exception("attempt to write binary node to closed file");

    /// first write self data
    m_fin->write(&m_buffer[0], m_buffer.size());
#if 0
    /// write children data
    for(const BinaryTreePtr& child : m_children)
        child->writeToFile();
#endif
}
