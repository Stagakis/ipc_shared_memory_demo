#ifndef IPC_LIBRARY_TBIPC_H
#define IPC_LIBRARY_TBIPC_H
#include <atomic>

struct BufferFlags{
    unsigned long int read_buffer_offset;
    std::atomic<unsigned long int> back_buffer_offset;
    unsigned long int write_buffer_offset;
    std::atomic<unsigned long int> dirty;
};

struct SharedMemory{
    void * const buffers;
    const unsigned int single_size; //Size of a single buffer
    const unsigned int total_size;  //3*single_size + bufferFlags
    const char * const filename;
};

typedef const SharedMemory * const MEMPTR;

extern "C"
{
MEMPTR create_shared_memory(const char * name , unsigned int single_buffer_size);

void write_to_shared_memory(MEMPTR mem, const void *data, unsigned long int offset = 0,
                            bool mark_dirty = true); //Producer Method

void *read_from_shared_memory(MEMPTR mem); //Consumer Method
int check_dirty_bit(MEMPTR mem);
void close_shared_memory(MEMPTR mem);

}
#endif //IPC_LIBRARY_TBIPC_H
