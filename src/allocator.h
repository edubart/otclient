#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <stdint.h>
#include <stdlib.h>
#include <list>
#include <boost/thread.hpp>

class Allocator;

struct  AllocationBlock
{
    ~AllocationBlock() { free(backtraceBuffer); }

    uint32_t bytes;
    uint32_t records;
    void** backtraceBuffer;
    uint8_t backtraceSize;

    friend class Allocator;
};

//TODO: allocattion tags
/*
struct AllocationTag
{
    std::_List_node_base *listNode;
};*/

class Allocator
{
public:
    Allocator();
    ~Allocator();

    void *allocate(size_t bytes);
    void deallocate(void* p);

    void dumpLeaks();

    static Allocator* instance() {
        return &m_instance;
    }

private:
    AllocationBlock* findBlock(void **backtrace, int backtraceSize, uint32_t bytes);

    typedef std::list<AllocationBlock*> AllocationBlocksList;
    AllocationBlocksList m_allocationsBlocks;

    typedef std::list<AllocationBlock*> AllocationsAddressesMap;
    AllocationsAddressesMap m_allocationAddresses;

    boost::recursive_mutex m_allocatorLock;
    bool m_inside;

    static Allocator m_instance;
};

void* operator new(size_t bytes, int dummy);
void* operator new(size_t bytes);
void* operator new[](size_t bytes);
void operator delete(void *p);
void operator delete[](void* p);

#endif

