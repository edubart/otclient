/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#ifdef _DEBUG_MEMORY

#include <unordered_set>
#include <boost/functional/hash.hpp>

#ifdef _REENTRANT
#include <boost/thread.hpp>
#endif

#define MAGIC_NUMBER 0xA1B2C3D4

struct AllocationBlock
{
    unsigned int bytes;
    void** backtraceBuffer;
    unsigned char backtraceSize;
    unsigned int records;
};

struct AllocationHead
{
    unsigned int magicNumber;
    AllocationBlock *block;
};

struct block_hash : std::unary_function<AllocationBlock *, std::size_t> {
    std::size_t operator()(const AllocationBlock *block) const {
        std::size_t seed = 0;

        boost::hash_combine(seed, block->bytes);
        for(int i=0;i<4;++i) {
            if(i < block->backtraceSize)
                boost::hash_combine(seed, block->backtraceBuffer[i]);
            else
                boost::hash_combine(seed, 0);
        }
        boost::hash_combine(seed, block->backtraceSize);
        return seed;
    }
};

struct block_equal_to : std::binary_function<AllocationBlock *, AllocationBlock *, bool> {
    bool operator()(const AllocationBlock* a, const AllocationBlock *b) const
    {
        if(a->bytes != b->bytes || a->backtraceSize != b->backtraceSize)
            return false;
        for(int i=0;i<a->backtraceSize;++i)
            if(a->backtraceBuffer[i] != b->backtraceBuffer[i])
                return false;
        return true;
    }
};

class Allocator
{
public:
    Allocator();
    ~Allocator();

    void *allocate(size_t bytes);
    void deallocate(void *p);

    void dumpLeaks();

private:
    AllocationBlock* findBlock(void** backtraceBuffer, int backtraceSize, unsigned int bytes);

    typedef std::unordered_set<AllocationBlock*, block_hash, block_equal_to> AllocationBlocksList;
    AllocationBlocksList m_allocationsBlocks;

#ifdef _REENTRANT
    boost::recursive_mutex m_allocatorLock;
#endif
};

void* operator new(size_t bytes, int dummy);
void* operator new(size_t bytes);
void* operator new[](size_t bytes);
void operator delete(void *p);
void operator delete[](void *p);

#endif // _DEBUG_MEMORY

#endif

