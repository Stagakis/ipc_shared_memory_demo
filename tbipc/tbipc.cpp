#include "tbipc.h"
#include <sys/mman.h>
#include <cstring>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

void report_and_exit(const char* msg) {
    perror(msg);
    exit(-1);
}

void report(const char* msg) {
    perror(msg);
}


MEMPTR create_shared_memory(const char * const name, unsigned int single_buffer_size){
    unsigned int final_size = 3*single_buffer_size + sizeof(BufferFlags);

    int fd = shm_open(name,              /* name from smem.h */
                      O_RDWR | O_CREAT,   /* read/write, create if needed */
                      0666);             /* access permissions */
    if (fd < 0) report_and_exit("Can't open shared mem segment...");

    int ft = ftruncate(fd, final_size); /* get the bytes */
    if (ft < 0) report_and_exit("Can't resize file with ftruncate...");

    auto buffers = static_cast<void *>(mmap(NULL,       /* let system pick where to put segment */
                                            final_size,   /* how many bytes */
                                            PROT_READ | PROT_WRITE, /* access protections */
                                            MAP_SHARED, /* mapping visible to other processes */
                                            fd,         /* file descriptor */
                                            0));         /* offset: start at 1st byte */

    MEMPTR shm = new SharedMemory {buffers, single_buffer_size, final_size, name};

    //Initialize flags
    auto flags = static_cast<BufferFlags*>(shm->buffers);
    flags->read_buffer_offset = 0;
    flags->back_buffer_offset.store(single_buffer_size);
    flags->write_buffer_offset = 2*single_buffer_size;
    flags->dirty.store(0);

    return shm;
}

void mark_dirty(MEMPTR mem){
    auto flags = static_cast<BufferFlags*>(mem->buffers);
    flags->dirty.store(1);
}

void write_to_shared_memory(MEMPTR mem, const void * data, const unsigned long int size, const unsigned long int offset, bool mark_dirty){ //Producer Method
    auto flags = static_cast<BufferFlags*>(mem->buffers);
    char * buffers_data = &static_cast<char *>(mem->buffers)[sizeof(BufferFlags)];
    memcpy(
            &buffers_data[flags->write_buffer_offset + offset],
            data,
            size
    );


    if(mark_dirty){
        flags->write_buffer_offset = flags->back_buffer_offset.exchange(flags->write_buffer_offset);

        flags->dirty.store(1);
    }
}

void * read_from_shared_memory(MEMPTR mem, unsigned long int offset){ //Consumer Method
    auto flags = static_cast<BufferFlags*>(mem->buffers);
    char * buffers_data = &static_cast<char *>(mem->buffers)[sizeof(BufferFlags)];

    if(flags->dirty.load()){
        flags->read_buffer_offset = flags->back_buffer_offset.exchange(flags->read_buffer_offset);
        flags->dirty.store(0);
    }
    return (void *)&buffers_data[flags->read_buffer_offset + offset];
}

int check_dirty_bit(MEMPTR mem){
    return static_cast<BufferFlags*>(mem->buffers)->dirty;
}


void close_shared_memory(MEMPTR mem){
    auto munmap_res = munmap(mem->buffers, mem->total_size);  /* unmap the storage */
    if(munmap_res == -1) report("Can't munmap_res");

    auto unlink_res = shm_unlink(mem->filename); /* unlink from the backing file */
    if(unlink_res == -1) report("Can't shm_unlink");

    delete mem; //delete thyself
}

