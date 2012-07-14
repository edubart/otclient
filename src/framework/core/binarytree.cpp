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

void BinaryTree::unserialize(const FileStreamPtr& fin)
{
    while(true) {
        uint8 byte = fin->getU8();
        switch(byte) {
            case BINARYTREE_NODE_START: {
                BinaryTreePtr node(new BinaryTree(shared_from_this()));
                m_children.push_back(node);
                node->unserialize(fin);
                break;
            }
            case BINARYTREE_NODE_END:
                return;
            case BINARYTREE_ESCAPE_CHAR:
                m_buffer.add(fin->getU8());
                break;
            default:
                m_buffer.add(byte);
                break;
        }
    }
}

void BinaryTree::serialize(const FileStreamPtr& fin)
{

}

void BinaryTree::seek(uint pos)
{
    if(pos > m_buffer.size())
        stdext::throw_exception("BinaryTree: seek failed");
    m_pos = pos;
}

uint8 BinaryTree::getU8()
{
    if(m_pos+1 > m_buffer.size())
        stdext::throw_exception("BinaryTree: getU8 failed");
    uint8 v = m_buffer[m_pos];
    m_pos += 1;
    return v;
}

uint16 BinaryTree::getU16()
{
    if(m_pos+2 > m_buffer.size())
        stdext::throw_exception("BinaryTree: getU16 failed");
    uint16 v = stdext::readLE16(&m_buffer[m_pos]);
    m_pos += 2;
    return v;
}

uint32 BinaryTree::getU32()
{
    if(m_pos+4 > m_buffer.size())
        stdext::throw_exception("BinaryTree: getU32 failed");
    uint32 v = stdext::readLE32(&m_buffer[m_pos]);
    m_pos += 4;
    return v;
}

uint64 BinaryTree::getU64()
{
    if(m_pos+8 > m_buffer.size())
        stdext::throw_exception("BinaryTree: getU64 failed");
    uint64 v = stdext::readLE64(&m_buffer[m_pos]);
    m_pos += 8;
    return v;
}

std::string BinaryTree::getString()
{
    uint16 len = getU16();
    if(len == 0 || len > 8192)
        stdext::throw_exception("BinaryTree: getString failed: invalid or too large string length");

    if(m_pos+len > m_buffer.size())
        stdext::throw_exception("BinaryTree: getString failed: string length exceeded buffer size.");

    std::string ret((char *)&m_buffer[m_pos], len);
    m_pos += len;
    return ret;
}
