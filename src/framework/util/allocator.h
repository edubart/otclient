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

