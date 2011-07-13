#ifdef _DEBUG_MEMORY

#include "allocator.h"

#include <cstdio>
#include <cstdlib>
#include <execinfo.h>

bool allocatorEnabled = false;
Allocator *allocator = NULL;

void disableAllocator() { allocatorEnabled = false; }
void enableAllocator() { allocatorEnabled = true; }
bool isAllocatorEnabled() { return allocatorEnabled; }

class Initializer {
public:
    Initializer() {
        allocator = new Allocator;
        enableAllocator();
    }
    ~Initializer() {
        disableAllocator();
        Allocator *tmp = allocator;
        allocator = NULL;
        delete tmp;
    }
};
Initializer initializer __attribute__((init_priority(101)));

static void addr2line(void *address, const char* name, bool viewSource = false)
{
    char tmpbuf[1024];
    char *pos;

    snprintf(tmpbuf, sizeof(tmpbuf), "addr2line --functions --demangle -e %s %p", name, address);

    FILE *output = popen(tmpbuf, "r");
    if(output) {
        char filesource[1024];
        int fileline = 0;
        filesource[0] = '\0';
        char *line;

        while(!feof(output)) {
            line = fgets(tmpbuf, sizeof(tmpbuf) - 1, output);
            if(!line)
                break;

            pos = strchr(line, '\n');
            if(pos > 0)
                pos[0] = '\0';

            int func = 1;
            pos = strchr(line, ':');
            if(pos > 0 && pos[1] != ':')
                func = 0;

            if(func) {
                printf("%s ", line);
            } else if(strncmp(line,"??:0",4) != 0) {
                if(pos > 0 && fileline == 0) {
                    strncpy(filesource, line, (int)(pos - line));
                    filesource[(int)(pos - line)] = '\0';
                    fileline = atoi(line + (int)(pos - line) + 1);
                }
                pos = strrchr(line, '/');
                if(!pos)
                    pos = line;
                else
                    pos++;
                printf("(%s) ", pos);
            }
        }
        pclose(output);

        pos = (char*)strrchr(name, '/');
        if(!pos)
            pos = (char*) name;
        else
            pos++;
        printf("(%s)", pos);

        if(viewSource) {
            if(filesource[0] && fileline > 0) {
                output = fopen(filesource,"rb");
                if(output) {
                    int i;
                    for(i=0; i<fileline+1; i++) {
                        line = fgets(tmpbuf, sizeof(tmpbuf) - 1, output);

                        if(i >= fileline-1 && strlen(line) > 1) {
                            pos = strchr(line, '\n');
                            if(pos > 0)
                                pos[0] = '\0';
                            printf("\n\t\t%d:%s",i,line);
                        }
                    }

                    fclose(output);
                }
            }
        }
    }

    printf("\n");
}

void printBacktrace(int skips)
{
    void *buffer[128];
    int size = backtrace(buffer, 128);
    char **strings = backtrace_symbols(buffer, size);
    for(int i = 1 + skips; i < size; i++) {
        if(i == 1 + skips) {
            printf("\tfrom ");
        } else {
            printf("\tat ");
        }
        std::string str = strings[i];
        addr2line(buffer[i], str.substr(0, str.find('(')).c_str());
    }
    printf("\n");
    free(strings);
}


Allocator::Allocator()
{
}

Allocator::~Allocator()
{
#ifdef _REENTRANT
    boost::recursive_mutex::scoped_lock lock(m_allocatorLock);
#endif

    dumpLeaks();

    for(AllocationBlocksList::iterator it = m_allocationsBlocks.begin(), end = m_allocationsBlocks.end(); it != end; ++it) {
        AllocationBlock* block = (*it);
        free(block->backtraceBuffer);
        free(block);
    }
    m_allocationsBlocks.clear();
}

void Allocator::dumpLeaks()
{
#ifdef _REENTRANT
    boost::recursive_mutex::scoped_lock lock(m_allocatorLock);
#endif

    bool shouldRenable = false;
    if(isAllocatorEnabled()) {
        disableAllocator();
        shouldRenable = true;
    }

    unsigned int definitelyLostBytes = 0;
    unsigned int blockNumber = 1;
    unsigned int countRecords = 0;
    unsigned int numberOfLeakedBlocks = 0;
    unsigned int numberOfBlocks = m_allocationsBlocks.size();

    for(AllocationBlocksList::iterator it = m_allocationsBlocks.begin(), end = m_allocationsBlocks.end(); it != end; ++it) {
        AllocationBlock* block = (*it);
        if(block->records != 0) {
            numberOfLeakedBlocks++;
        }
    }

    if(numberOfLeakedBlocks > 0) {
        printf("== LOST BLOCKS:\n");
        for(AllocationBlocksList::iterator it = m_allocationsBlocks.begin(), end = m_allocationsBlocks.end(); it != end; ++it) {
            AllocationBlock* block = (*it);
            if(block->records != 0) {
                unsigned int lostBytes = block->bytes * block->records;

                definitelyLostBytes += lostBytes;
                countRecords += block->records;

                printf("%d bytes in %d block\'s records are definitely lost in loss %d of %d\n",
                        lostBytes, block->records, blockNumber, numberOfLeakedBlocks);

                char **strings = backtrace_symbols(block->backtraceBuffer, block->backtraceSize);
                for(int i = 0; i < block->backtraceSize; i++) {
                    if(i == 0) {
                        printf("\tat ");
                    } else {
                        printf("\tby ");
                    }
                    std::string str = strings[i];
                    addr2line(block->backtraceBuffer[i], str.substr(0, str.find('(')).c_str());
                }
                printf("\n");
                free(strings);
                blockNumber++;
            }
        }


        printf("== LEAK SUMMARY:\n");
        printf("definitely lost: %d bytes in %d blocks\n", definitelyLostBytes, numberOfLeakedBlocks);
        printf("unallocated records: %d records in %d blocks\n", countRecords, numberOfLeakedBlocks);
        printf("leaked blocks: %d in %d blocks\n", numberOfLeakedBlocks, numberOfBlocks);
    }

    if(shouldRenable)
        enableAllocator();
}

AllocationBlock* Allocator::findBlock(void **backtraceBuffer, int backtraceSize, unsigned int bytes)
{
    AllocationBlock blockToFind;
    blockToFind.backtraceBuffer = backtraceBuffer;
    blockToFind.backtraceSize = backtraceSize;
    blockToFind.bytes = bytes;

    auto it = m_allocationsBlocks.find(&blockToFind);
    if(it != m_allocationsBlocks.end())
        return (*it);
    return NULL;
}

void *Allocator::allocate(size_t bytes)
{
#ifdef _REENTRANT
    boost::recursive_mutex::scoped_lock lock(m_allocatorLock);
#endif

    if(!isAllocatorEnabled()) {
        return malloc(bytes);
    } else {
        disableAllocator();

        char *allocatedBuffer = (char *)malloc(sizeof(AllocationHead) + bytes);
        AllocationHead *head = (AllocationHead *)allocatedBuffer;
        void *p = (void *)(allocatedBuffer + sizeof(AllocationHead));

        static void *buffer[128];
        int size = backtrace(buffer, 128);
        AllocationBlock* block = findBlock(&buffer[1], size - 1, bytes);

        if(!block) {
            block = (AllocationBlock *)malloc(sizeof(AllocationBlock));

            block->backtraceBuffer = (void **)malloc(sizeof(void*) * (size - 1));
            block->backtraceSize = size - 1;
            block->bytes = bytes;
            block->records = 0;

            for(int i=0;i<size-1;i++)
                block->backtraceBuffer[i] = buffer[i+1];

            m_allocationsBlocks.insert(block);
        }
        block->records++;

        head->block = block;
        head->magicNumber = MAGIC_NUMBER;

        enableAllocator();
        return p;
    }
}

void Allocator::deallocate(void *p)
{
#ifdef _REENTRANT
    boost::recursive_mutex::scoped_lock lock(m_allocatorLock);
#endif

    if(!isAllocatorEnabled()) {
        free(p);
    } else if(p == NULL) {
        disableAllocator();

        printf("attempt to delete NULL address\n");
        printBacktrace(1);

        enableAllocator();
    } else {
        disableAllocator();

        p = (void*)((char *)p - sizeof(AllocationHead));
        AllocationHead *head = (AllocationHead *)p;

        if(head->magicNumber == MAGIC_NUMBER) {
            head->block->records--;

            memset(p, 0, head->block->bytes + sizeof(AllocationHead));
            free(p);
        } else {
            printf("invalid delete address\n");
            printBacktrace(1);
        }

        enableAllocator();
    }
}

void *operator new(size_t bytes, int dummy)
{
    if(allocator)
        return allocator->allocate(bytes);
    return malloc(bytes);
}

void *operator new(size_t bytes)
{
    if(allocator)
        return allocator->allocate(bytes);
    return malloc(bytes);
}

void *operator new[](size_t bytes)
{
    if(allocator)
        return allocator->allocate(bytes);
    return malloc(bytes);
}

void operator delete(void *p)
{
    if(allocator)
        allocator->deallocate(p);
    else
        free(p);
}

void operator delete[](void *p)
{
    if(allocator)
        allocator->deallocate(p);
    else
        free(p);
}

#endif // _DEBUG_MEMORY
