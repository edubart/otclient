#include "allocator.h"

#include <ucontext.h>
#include <execinfo.h>

#if defined(REG_EIP)
# define MACHINE_X86
# define REGFORMAT "0x%08x"
# define ADDRTYPE unsigned int
#elif defined(REG_RIP)
# define MACHINE_X86_64
# define REGFORMAT "0x%016lx"
# define ADDRTYPE long unsigned int
#endif

Allocator Allocator::m_instance __attribute__((init_priority(101)));

static void addr2line(ADDRTYPE address, const char* name, bool viewSource = false)
{
    char tmpbuf[1024];
    char *pos;

    printf(REGFORMAT": ", address);

    snprintf(tmpbuf, sizeof(tmpbuf), "addr2line --functions --demangle -e %s "REGFORMAT, name, address);

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

Allocator::Allocator() : m_inside(false)
{

}

Allocator::~Allocator()
{
    boost::recursive_mutex::scoped_lock lock(m_allocatorLock);

    dumpLeaks();

    m_inside = true;

    for(AllocationBlocksList::iterator it = m_allocationsBlocks.begin(), end = m_allocationsBlocks.end(); it != end; ++it) {
        AllocationBlock* block = (*it);
        block->~AllocationBlock();
        free(block);
    }
    m_allocationsBlocks.clear();
    m_allocationAddresses.clear();

    m_inside = false;
}

//TODO: dump leaks to a file
void Allocator::dumpLeaks()
{
    boost::recursive_mutex::scoped_lock lock(m_allocatorLock);

    m_inside = true;

    if(m_allocationAddresses.size() > 0) {
        uint32_t definitelyLostBytes = 0;
        uint32_t blockNumber = 1;
        uint32_t countRecords = 0;
        uint32_t numberOfLeakedBlocks = 0;
        uint32_t numberOfBlocks = m_allocationsBlocks.size();


        for(AllocationBlocksList::iterator it = m_allocationsBlocks.begin(), end = m_allocationsBlocks.end(); it != end; ++it) {
            AllocationBlock* block = (*it);
            if(block->records != 0) {
                numberOfLeakedBlocks++;
            }
        }

        printf("== LOST BLOCKS:\n");
        for(AllocationBlocksList::iterator it = m_allocationsBlocks.begin(), end = m_allocationsBlocks.end(); it != end; ++it) {
            AllocationBlock* block = (*it);
            if(block->records != 0) {
                uint32_t lostBytes = block->bytes * block->records;

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
                    addr2line((ADDRTYPE)block->backtraceBuffer[i], str.substr(0, str.find('(')).c_str());
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

    m_inside = false;
}

void printBacktrace()
{
    void *buffer[128];
    int size = backtrace(buffer, 128);
    char **strings = backtrace_symbols(buffer, size);
    for(int i = 1; i < size; i++) {
        if(i == 1) {
            printf("\tfrom ");
        } else {
            printf("\tat ");
        }
        std::string str = strings[i];
        addr2line((ADDRTYPE)buffer[i], str.substr(0, str.find('(')).c_str());
    }
    printf("\n");
    free(strings);
}

//TODO: dump allocator memory stats by blocks to a file

AllocationBlock* Allocator::findBlock(void **backtraceBuffer, int backtraceSize, uint32_t bytes)
{
    for(AllocationBlocksList::iterator it = m_allocationsBlocks.begin(), end = m_allocationsBlocks.end(); it != end; ++it) {
        AllocationBlock* block = (*it);

        if(block->bytes != bytes || block->backtraceSize != backtraceSize) {
            continue;
        }

        bool isSame = true;
        for(int i = 0; i < backtraceSize; i++) {
            if(block->backtraceBuffer[i] != backtraceBuffer[i]) {
                isSame = false;
                break;
            }
        }

        if(isSame) {
            return block;
        }
    }
    return NULL;
}

void *Allocator::allocate(size_t bytes)
{
    boost::recursive_mutex::scoped_lock lock(m_allocatorLock);

    if(m_inside) {
        return malloc(bytes);
    } else {
        void* p = malloc(bytes + sizeof(ADDRTYPE));
        void* p2 = (void*)((ADDRTYPE)p + sizeof(ADDRTYPE));

        m_inside = true;

        static void* buffer[128];
        int size = backtrace(buffer, 128);

        AllocationBlock* block = findBlock(&buffer[1], size-1, bytes);
        if(!block) {
            block = (AllocationBlock *) malloc(sizeof(AllocationBlock));

            block->backtraceBuffer = (void**) malloc((sizeof(void*)) * (size - 1));
            block->backtraceSize = size-1;
            block->bytes = bytes;
            block->records = 0;

            for(int i=0;i<size-1;i++) {
                block->backtraceBuffer[i] = buffer[i+1];
            }

            m_allocationsBlocks.push_front(block);
        }
        block->records += 1;

        m_allocationAddresses.push_front(block);
        *(ADDRTYPE *)p = (ADDRTYPE)m_allocationAddresses.begin()._M_node;

        m_inside = false;

        return p2;
    }
}

void Allocator::deallocate(void* p)
{
    boost::recursive_mutex::scoped_lock lock(m_allocatorLock);

    if(m_inside) {
        free(p);
    } else if(p == NULL) {
        m_inside = true;

        printf("attempt to delete NULL address\n");
        printBacktrace();

        m_inside = false;
        return;
    } else {
        void *p2 = (void*)((ADDRTYPE)p - sizeof(ADDRTYPE));

        if(!m_inside) {
            m_inside = true;

            AllocationsAddressesMap::iterator it((std::__detail::_List_node_base*)(*(ADDRTYPE*)p2));

            AllocationBlock* block = (*it);
            block->records -= 1;
            memset(p2, 0x00, block->bytes);

            m_allocationAddresses.erase(it);

            m_inside = false;

            free(p2);
        } else {
            free(p);
        }
    }
}

void* operator new(size_t bytes, int dummy)
{
    return Allocator::instance()->allocate(bytes);
}

void* operator new(size_t bytes)
{
    return Allocator::instance()->allocate(bytes);
}

void* operator new[](size_t bytes)
{
    return Allocator::instance()->allocate(bytes);
}

void operator delete(void *p)
{
    Allocator::instance()->deallocate(p);
}

void operator delete[](void* p)
{
    Allocator::instance()->deallocate(p);
}
